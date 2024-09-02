
#include "InteractiveSkillComponent.h"

#include <queue>
#include <map>

#include "GameplayAbilitySpec.h"

#include "GAEvent_Helper.h"
#include "CharacterBase.h"
#include "CharacterAttributesComponent.h"
#include "GenerateType.h"
#include "PlanetPlayerState.h"
#include "BasicFuturesBase.h"
#include "GAEvent_Send.h"
#include "GAEvent_Received.h"
#include "SceneElement.h"
#include "Weapon_Base.h"
#include "Skill_Base.h"
#include "Skill_Active_Base.h"
#include "Skill_WeaponActive_PickAxe.h"
#include "Skill_WeaponActive_HandProtection.h"
#include "Skill_WeaponActive_RangeTest.h"
#include "Weapon_HandProtection.h"
#include "Weapon_PickAxe.h"
#include "Weapon_RangeTest.h"
#include "AssetRefMap.h"
#include "Skill_Talent_NuQi.h"
#include "Skill_Talent_YinYang.h"
#include "Skill_Element_Gold.h"
#include "InputProcessorSubSystem.h"
#include "Tool_PickAxe.h"
#include "HumanRegularProcessor.h"
#include "HumanCharacter.h"
#include "GameplayTagsSubSystem.h"
#include "SceneUnitTable.h"
#include "Skill_Active_Control.h"
#include "InteractiveBaseGAComponent.h"
#include "Skill_WeaponActive_Base.h"

FName UInteractiveSkillComponent::ComponentName = TEXT("InteractiveSkillComponent");

void UInteractiveSkillComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UInteractiveSkillComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	auto OnwerActorPtr = GetOwner<ACharacterBase>();
	if (OnwerActorPtr)
	{
		auto GASPtr = OnwerActorPtr->GetAbilitySystemComponent();
		for (auto& Iter : SkillsMap)
		{
			auto GAInsPtr = Cast<USkill_Active_Base>(Iter.Value->SkillUnitPtr->GetGAInst());
			if (!GAInsPtr)
			{
				continue;
			}

			GAInsPtr->Tick(DeltaTime);
		}

		TSharedPtr<FWeaponSocketInfo > WeaponUnit;
		switch (CurrentActivedWeaponSocket)
		{
		case EWeaponSocket::kMain:
		{
			WeaponUnit = FirstWeaponUnit;
		}
		break;
		case EWeaponSocket::kSecondary:
		{
			WeaponUnit = SecondaryWeaponUnit;
		}
		break;
		}
		if (WeaponUnit)
		{
		}
	}
}

void UInteractiveSkillComponent::InitialBaseGAs()
{
#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		auto OnwerActorPtr = GetOwner<ACharacterBase>();
		if (OnwerActorPtr)
		{
			auto GASPtr = OnwerActorPtr->GetAbilitySystemComponent();

			// 五行技能
			GASPtr->GiveAbility(
				FGameplayAbilitySpec(
					Skill_Element_GoldClass
				)
			);
		}
	}
#endif
}

void UInteractiveSkillComponent::RegisterWeapon(
	const TSharedPtr<FWeaponSocketInfo>& FirstWeaponSocketInfo,
	const TSharedPtr<FWeaponSocketInfo>& SecondWeaponSocketInfo
)
{
	auto OnwerActorPtr = GetOwner<ACharacterBase>();

	FirstWeaponUnit = FirstWeaponSocketInfo;

	SecondaryWeaponUnit = SecondWeaponSocketInfo;
}

void UInteractiveSkillComponent::RegisterMultiGAs(
	const TMap<FGameplayTag, TSharedPtr<FSkillSocketInfo>>& InSkillsMap, bool bIsGenerationEvent
)
{
	auto OnwerActorPtr = GetOwner<ACharacterBase>();
	if (!OnwerActorPtr)
	{
		return;
	}

	SkillsMap = InSkillsMap;

	if (bIsGenerationEvent)
	{
		GenerationCanbeActiveEvent();
	}
}

void UInteractiveSkillComponent::GetWeapon(
	TSharedPtr<FWeaponSocketInfo>& FirstWeaponSocketInfo,
	TSharedPtr<FWeaponSocketInfo>& SecondWeaponSocketInfo
)const
{
	FirstWeaponSocketInfo = FirstWeaponUnit;
	SecondWeaponSocketInfo = SecondaryWeaponUnit;
}

bool UInteractiveSkillComponent::ActiveWeapon(EWeaponSocket InWeaponSocket)
{
	if (InWeaponSocket != CurrentActivedWeaponSocket)
	{
		auto OnwerActorPtr = GetOwner<ACharacterBase>(); 
		auto ASCPtr = OnwerActorPtr->GetAbilitySystemComponent();

		// 清除上一次的
		auto Swith = [this, OnwerActorPtr, ASCPtr]
			{
				TSharedPtr<FWeaponSocketInfo > WeaponUnit;
				switch (CurrentActivedWeaponSocket)
				{
				case EWeaponSocket::kMain:
				{
					if (FirstWeaponUnit->WeaponUnitPtr)
					{
						WeaponUnit = FirstWeaponUnit;
					}
					else
					{
						return;
					}
				}
				break;
				case EWeaponSocket::kSecondary:
				{
					if (FirstWeaponUnit->WeaponUnitPtr)
					{
						WeaponUnit = SecondaryWeaponUnit;
					}
					else
					{
						return;
					}
				}
				break;
				}

				if (WeaponUnit)
				{
					Cast<USkill_WeaponActive_Base>(WeaponUnit->WeaponUnitPtr->FirstSkill->GetGAInst())->StopContinueActive();
				}

				if (ActivedWeaponPtr)
				{
					ActivedWeaponPtr->Destroy();
				}
			};

		TSharedPtr<FWeaponSocketInfo > WeaponUnit;
		switch (InWeaponSocket)
		{
		case EWeaponSocket::kMain:
		{
			WeaponUnit = FirstWeaponUnit;
		}
		break;
		case EWeaponSocket::kSecondary:
		{
			WeaponUnit = SecondaryWeaponUnit;
		}
		break;
		case EWeaponSocket::kNone:
		{
			WeaponUnit.Reset();
		}
		break;
		}

		switch (InWeaponSocket)
		{
		case EWeaponSocket::kMain:
		case EWeaponSocket::kSecondary:
		{
			if (WeaponUnit && WeaponUnit->WeaponUnitPtr)
			{
				// 可以切换时，清除上一次的
				Swith();

				CurrentActivedWeaponSocket = InWeaponSocket;

				FActorSpawnParameters ActorSpawnParameters;
				ActorSpawnParameters.Owner = OnwerActorPtr;
				ActorSpawnParameters.CustomPreSpawnInitalization = [WeaponUnit](AActor* ActorPtr)
					{
						Cast<AWeapon_Base>(ActorPtr)->WeaponUnitPtr = WeaponUnit->WeaponUnitPtr;
					};

				ActivedWeaponPtr = GetWorld()->SpawnActor<AWeapon_Base>(
					WeaponUnit->WeaponUnitPtr->GetTableRowUnit_WeaponExtendInfo()->ToolActorClass, ActorSpawnParameters
				);

				OnwerActorPtr->SwitchAnimLink(WeaponUnit->WeaponUnitPtr->GetTableRowUnit_WeaponExtendInfo()->AnimLinkClassType);

				OnActivedWeaponChangedContainer.ExcuteCallback(InWeaponSocket);
				return true;
			}
		}
		break;
		case EWeaponSocket::kNone:
		{
			CurrentActivedWeaponSocket = InWeaponSocket;

			OnwerActorPtr->SwitchAnimLink(EAnimLinkClassType::kUnarmed);

			Swith();

			WeaponUnit.Reset();

			OnActivedWeaponChangedContainer.ExcuteCallback(InWeaponSocket);

			return true;
		}
		break;
		}
	}

	return false;
}

void UInteractiveSkillComponent::ActiveWeapon()
{
	if (FirstWeaponUnit && FirstWeaponUnit->WeaponUnitPtr)
	{
		ActiveWeapon(EWeaponSocket::kMain);
	}
	else if (SecondaryWeaponUnit && SecondaryWeaponUnit->WeaponUnitPtr)
	{
		ActiveWeapon(EWeaponSocket::kSecondary);
	}
	else
	{
		RetractputWeapon();
	}
}

bool UInteractiveSkillComponent::SwitchWeapon()
{
	switch (CurrentActivedWeaponSocket)
	{
	case EWeaponSocket::kMain:
	{
		return ActiveWeapon(EWeaponSocket::kSecondary);
	}
	break;
	case EWeaponSocket::kSecondary:
	{
		return ActiveWeapon(EWeaponSocket::kMain);
	}
	case EWeaponSocket::kNone:
	default:
	{
		if (FirstWeaponUnit && FirstWeaponUnit->WeaponUnitPtr)
		{
			return ActiveWeapon(EWeaponSocket::kMain);
		}
		else if (SecondaryWeaponUnit && SecondaryWeaponUnit->WeaponUnitPtr)
		{
			return ActiveWeapon(EWeaponSocket::kSecondary);
		}
		else
		{
			return ActiveWeapon(EWeaponSocket::kNone);
		}
	}
	break;
	}
}

void UInteractiveSkillComponent::RetractputWeapon()
{
	ActiveWeapon(EWeaponSocket::kNone);
}

int32 UInteractiveSkillComponent::GetCurrentWeaponAttackDistance() const
{
	auto ActivedWeaponSPtr = GetActivedWeapon();
	if (ActivedWeaponSPtr)
	{
		return ActivedWeaponSPtr->WeaponUnitPtr->GetMaxAttackDistance();
	}
	else
	{

	}

	return 100;
}

EWeaponSocket UInteractiveSkillComponent::GetActivedWeaponType()
{
	return CurrentActivedWeaponSocket;
}

TSharedPtr<FWeaponSocketInfo > UInteractiveSkillComponent::GetActivedWeapon() const
{
	TSharedPtr<FWeaponSocketInfo > WeaponUnit;
	switch (CurrentActivedWeaponSocket)
	{
	case EWeaponSocket::kMain:
	{
		WeaponUnit = FirstWeaponUnit;
	}
	break;
	case EWeaponSocket::kSecondary:
	{
		WeaponUnit = SecondaryWeaponUnit;
	}
	break;
	default:
	{
		WeaponUnit = MakeShared<FWeaponSocketInfo>();
	}
	}
	return WeaponUnit;
}

void UInteractiveSkillComponent::CancelSkill_WeaponActive(const TSharedPtr<FCanbeInteractionInfo>& CanbeActivedInfoSPtr)
{
	TSharedPtr<FWeaponSocketInfo > WeaponUnit = GetActivedWeapon();
	if (!WeaponUnit)
	{
		return;
	}
	if (!WeaponUnit->WeaponUnitPtr)
	{
		return;
	}

	auto OnwerActorPtr = GetOwner<ACharacterBase>();
	if (!OnwerActorPtr)
	{
		return;
	}

	auto GAInsPtr = Cast<USkill_WeaponActive_Base>(WeaponUnit->WeaponUnitPtr->FirstSkill->GetGAInst());
	if (!GAInsPtr)
	{
		return;
	}

	GAInsPtr->StopContinueActive();
}

bool UInteractiveSkillComponent::ActiveSkill_WeaponActive(
	const TSharedPtr<FCanbeInteractionInfo>& CanbeActivedInfoSPtr, bool bIsAutomaticStop /*= false*/
)
{
	TSharedPtr<FWeaponSocketInfo > WeaponUnit = GetActivedWeapon();
	if (!WeaponUnit)
	{
		return false;
	}
	if (!WeaponUnit->WeaponUnitPtr)
	{
		return false;
	}

	auto OnwerActorPtr = GetOwner<ACharacterBase>();
	if (!OnwerActorPtr)
	{
		return false;
	}

	auto GAInsPtr = Cast<USkill_WeaponActive_Base>(WeaponUnit->WeaponUnitPtr->FirstSkill->GetGAInst());
	if (!GAInsPtr)
	{
		return false;
	}

	if (GAInsPtr->IsActive())
	{
		GAInsPtr->ContinueActive();
		return true;
	}

	FGameplayEventData Payload;
	if (WeaponUnit->WeaponUnitPtr->FirstSkill->GetUnitType().MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Weapon_Axe))
	{
		auto GameplayAbilityTargetDashPtr = new FGameplayAbilityTargetData_Skill_PickAxe;
		GameplayAbilityTargetDashPtr->WeaponPtr = Cast<AWeapon_PickAxe>(ActivedWeaponPtr);
		GameplayAbilityTargetDashPtr->bIsAutoContinue = !bIsAutomaticStop;
		Payload.TargetData.Add(GameplayAbilityTargetDashPtr);
	}
	else if (WeaponUnit->WeaponUnitPtr->FirstSkill->GetUnitType().MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Weapon_HandProtection))
	{
		auto GameplayAbilityTargetDashPtr = new FGameplayAbilityTargetData_Skill_WeaponHandProtection;
		GameplayAbilityTargetDashPtr->WeaponPtr = Cast<AWeapon_HandProtection>(ActivedWeaponPtr);
		GameplayAbilityTargetDashPtr->bIsAutoContinue = !bIsAutomaticStop;
		Payload.TargetData.Add(GameplayAbilityTargetDashPtr);
	}
	else if (WeaponUnit->WeaponUnitPtr->FirstSkill->GetUnitType().MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Weapon_RangeTest))
	{
		auto GameplayAbilityTargetDashPtr = new FGameplayAbilityTargetData_Skill_WeaponActive_RangeTest;
		GameplayAbilityTargetDashPtr->WeaponPtr = Cast<AWeapon_RangeTest>(ActivedWeaponPtr);
		GameplayAbilityTargetDashPtr->bIsAutoContinue = !bIsAutomaticStop;
		Payload.TargetData.Add(GameplayAbilityTargetDashPtr);
	}

	auto ASCPtr = OnwerActorPtr->GetAbilitySystemComponent();
	return ASCPtr->TriggerAbilityFromGameplayEvent(
		GAInsPtr->GetCurrentAbilitySpecHandle(),
		ASCPtr->AbilityActorInfo.Get(),
		FGameplayTag::EmptyTag,
		&Payload,
		*ASCPtr
	);
}

bool UInteractiveSkillComponent::ActiveSkill_Active(
	const TSharedPtr<FCanbeInteractionInfo>& CanbeActivedInfoSPtr, bool bIsAutomaticStop /*= false*/
)
{
	auto SkillIter = SkillsMap.Find(CanbeActivedInfoSPtr->Socket);
	if (!SkillIter)
	{
		return false;
	}

	auto ActiveSkillUnitPtr = DynamicCastSharedPtr<FActiveSkillProxy>((*SkillIter)->SkillUnitPtr);
	if (!ActiveSkillUnitPtr)
	{
		return false;
	}

	if (ActiveSkillUnitPtr->GetUnitType().MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Active))
	{
		auto OnwerActorPtr = GetOwner<ACharacterBase>();
		if (!OnwerActorPtr)
		{
			return false;
		}

		auto ASCPtr = OnwerActorPtr->GetAbilitySystemComponent();
		if (!ActivedCorrespondingWeapon(ActiveSkillUnitPtr))
		{
			return false;
		}

		if (
			(*SkillIter)->SkillUnitPtr->GetUnitType().MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Active)
			)
		{
			// 需要特殊参数的
			if (
				(*SkillIter)->SkillUnitPtr->GetUnitType().MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Active)
				)
			{
				auto GAInsPtr = Cast<USkill_Active_Base>(ActiveSkillUnitPtr->GetGAInst());
				if (!GAInsPtr)
				{
					return false;
				}

				if (GAInsPtr->IsActive())
				{
					GAInsPtr->ContinueActive();
				}
				else
				{
					auto GameplayAbilityTargetPtr =
						new FGameplayAbilityTargetData_Control;

					GameplayAbilityTargetPtr->CanbeActivedInfoSPtr = CanbeActivedInfoSPtr;

					// Test
					GameplayAbilityTargetPtr->TargetCharacterPtr = OnwerActorPtr;

					FGameplayEventData Payload;
					Payload.TargetData.Add(GameplayAbilityTargetPtr);

					return ASCPtr->TriggerAbilityFromGameplayEvent(
						GAInsPtr->GetCurrentAbilitySpecHandle(),
						ASCPtr->AbilityActorInfo.Get(),
						FGameplayTag(),
						&Payload,
						*ASCPtr
					);
				}
			}
			else
			{
				auto GAInsPtr = Cast<USkill_Active_Base>(ActiveSkillUnitPtr->GetGAInst());
				if (!GAInsPtr)
				{
					return false;
				}

				if (GAInsPtr->IsActive())
				{
					GAInsPtr->ContinueActive();
				}
				else
				{
					auto GameplayAbilityTargetPtr =
						new FGameplayAbilityTargetData_ActiveSkill;

					GameplayAbilityTargetPtr->CanbeActivedInfoSPtr = CanbeActivedInfoSPtr;

					FGameplayEventData Payload;
					Payload.TargetData.Add(GameplayAbilityTargetPtr);

					return ASCPtr->TriggerAbilityFromGameplayEvent(
						GAInsPtr->GetCurrentAbilitySpecHandle(),
						ASCPtr->AbilityActorInfo.Get(),
						FGameplayTag(),
						&Payload,
						*ASCPtr
					);
				}
			}
		}
	}

	return false;
}

bool UInteractiveSkillComponent::ActiveAction(
	const TSharedPtr<FCanbeInteractionInfo>& CanbeActivedInfoSPtr, bool bIsAutomaticStop /*= false */
)
{
	switch (CanbeActivedInfoSPtr->Type)
	{
	case FCanbeInteractionInfo::EType::kActiveSkill:
	{
		return ActiveSkill_Active(CanbeActivedInfoSPtr, bIsAutomaticStop);
	}
	break;
	case FCanbeInteractionInfo::EType::kWeaponActiveSkill:
	{
		return ActiveSkill_WeaponActive(CanbeActivedInfoSPtr, bIsAutomaticStop);
	}
	break;
	default:
		break;
	}
	return false;
}

void UInteractiveSkillComponent::CancelAction(const TSharedPtr<FCanbeInteractionInfo>& CanbeActivedInfoSPtr)
{
	switch (CanbeActivedInfoSPtr->Type)
	{
	case FCanbeInteractionInfo::EType::kWeaponActiveSkill:
	{
		CancelSkill_WeaponActive(CanbeActivedInfoSPtr);
	}
	break;
	default:
		break;
	}
}

bool UInteractiveSkillComponent::ActivedCorrespondingWeapon(const TSharedPtr<FActiveSkillProxy>& ActiveSkillUnitPtr)
{
	if (ActiveSkillUnitPtr)
	{
		const auto RequireWeaponUnitType = ActiveSkillUnitPtr->GetTableRowUnit_ActiveSkillExtendInfo()->RequireWeaponUnitType;
		if (RequireWeaponUnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Weapon))
		{
			TSharedPtr<FWeaponSocketInfo > WeaponUnit;
			TSharedPtr<FWeaponSocketInfo > OtherWeaponUnit;
			switch (CurrentActivedWeaponSocket)
			{
			case EWeaponSocket::kMain:
			{
				WeaponUnit = FirstWeaponUnit;
				OtherWeaponUnit = SecondaryWeaponUnit;
			}
			break;
			case EWeaponSocket::kSecondary:
			{
				WeaponUnit = SecondaryWeaponUnit;
				OtherWeaponUnit = FirstWeaponUnit;
			}
			break;
			}

			if (
				WeaponUnit &&
				WeaponUnit->WeaponUnitPtr &&
				(RequireWeaponUnitType.MatchesTag(WeaponUnit->WeaponUnitPtr->GetUnitType()))
				)
			{
				return true;
			}
			else if (
				OtherWeaponUnit &&
				OtherWeaponUnit->WeaponUnitPtr &&
				(RequireWeaponUnitType.MatchesTag(OtherWeaponUnit->WeaponUnitPtr->GetUnitType()))
				)
			{
				return SwitchWeapon();
			}
		}
	}

	return false;
}

void UInteractiveSkillComponent::GenerationCanbeActiveEvent()
{
	CanbeInteractionAry.Empty();

	// 响应武器
	{
		TSharedPtr<FCanbeInteractionInfo > CanbeActivedInfoSPtr = MakeShared<FCanbeInteractionInfo>();
		CanbeActivedInfoSPtr->Type = FCanbeInteractionInfo::EType::kWeaponActiveSkill;
		CanbeActivedInfoSPtr->Key = EKeys::LeftMouseButton;
		CanbeInteractionAry.Add(CanbeActivedInfoSPtr);
	}

	// 响应主动技能
	for (const auto& Iter : SkillsMap)
	{
		TSharedPtr<FCanbeInteractionInfo > CanbeActivedInfoSPtr = MakeShared<FCanbeInteractionInfo>();

		CanbeActivedInfoSPtr->Type = FCanbeInteractionInfo::EType::kActiveSkill;
		CanbeActivedInfoSPtr->Key = Iter.Value->Key;
		CanbeActivedInfoSPtr->Socket = Iter.Value->SkillSocket;

		CanbeInteractionAry.Add(CanbeActivedInfoSPtr);
	}
}

TSharedPtr<FSkillSocketInfo >UInteractiveSkillComponent::FindSkill(const FGameplayTag& Tag)
{
	auto Iter = SkillsMap.Find(Tag);
	if (Iter)
	{
		return *Iter;
	}

	return nullptr;
}

const TMap<FGameplayTag, TSharedPtr<FSkillSocketInfo>>& UInteractiveSkillComponent::GetSkills() const
{
	return SkillsMap;
}
