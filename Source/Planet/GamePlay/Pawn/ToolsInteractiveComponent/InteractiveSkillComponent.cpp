
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

FName UInteractiveSkillComponent::ComponentName = TEXT("InteractiveSkillComponent");

void UInteractiveSkillComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RetractputWeapon();

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
			auto GAInsPtr = Cast<USkill_Active_Base>(Iter.Value->SkillUnitPtr->GAInstPtr);
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

void UInteractiveSkillComponent::RegisterWeapon(
	const TSharedPtr<FWeaponSocketInfo>& FirstWeaponSocketInfo,
	const TSharedPtr<FWeaponSocketInfo>& SecondWeaponSocketInfo
)
{
	FirstWeaponUnit = FirstWeaponSocketInfo;
	SecondaryWeaponUnit = SecondWeaponSocketInfo;
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
		TSharedPtr<FWeaponSocketInfo > WeaponUnit;

		// 清除上一次的
		{
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
					return false;
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
					return false;
				}
			}
			break;
			}

			if (WeaponUnit)
			{
				auto GASpecPtr = OnwerActorPtr->GetAbilitySystemComponent()->FindAbilitySpecFromHandle(WeaponUnit->Handle);
				if (GASpecPtr)
				{
					auto GAInsPtr = Cast<USkill_WeaponActive_Base>(GASpecPtr->GetPrimaryInstance());
					if (GAInsPtr)
					{
						GAInsPtr->ForceCancel();
					}
					else
					{
						return false;
					}
				}

				OnwerActorPtr->GetAbilitySystemComponent()->ClearAbility(WeaponUnit->Handle);
			}
			if (ActivedWeaponPtr)
			{
				ActivedWeaponPtr->Destroy();
			}
		}
		CurrentActivedWeaponSocket = InWeaponSocket;
		{
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
			case EWeaponSocket::kNone:
			{
				if (ActivedWeaponPtr)
				{
					ActivedWeaponPtr->Destroy();
					ActivedWeaponPtr = nullptr;
				}
				WeaponUnit.Reset();
			}
			break;
			}

			if (WeaponUnit && WeaponUnit->WeaponUnitPtr)
			{
				WeaponUnit->Handle = OnwerActorPtr->GetAbilitySystemComponent()->GiveAbility(
					FGameplayAbilitySpec(
						WeaponUnit->WeaponUnitPtr->FirstSkill->GetSkillClass(),
						WeaponUnit->WeaponUnitPtr->FirstSkill->Level
					)
				);

				FActorSpawnParameters ActorSpawnParameters;
				ActorSpawnParameters.Owner = OnwerActorPtr;

				ActivedWeaponPtr = GetWorld()->SpawnActor<AWeapon_Base>(
					WeaponUnit->WeaponUnitPtr->GetTableRowUnit_WeaponExtendInfo()->ToolActorClass, ActorSpawnParameters
				);

				OnwerActorPtr->SwitchAnimLink(WeaponUnit->WeaponUnitPtr->GetTableRowUnit_WeaponExtendInfo()->AnimLinkClassType);
			}
			else
			{
				OnwerActorPtr->SwitchAnimLink(EAnimLinkClassType::kUnarmed);
			}
			OnActivedWeaponChangedContainer.ExcuteCallback(InWeaponSocket);
			return true;
		}
	}

	return false;
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

	auto ASCPtr = OnwerActorPtr->GetAbilitySystemComponent();
	auto GameplayAbilitySpecPtr = ASCPtr->FindAbilitySpecFromHandle(WeaponUnit->Handle);
	if (!GameplayAbilitySpecPtr)
	{
		return;
	}
	auto GAInsPtr = Cast<USkill_WeaponActive_Base>(GameplayAbilitySpecPtr->GetPrimaryInstance());
	if (!GAInsPtr)
	{
		return;
	}

	GAInsPtr->RequestCancel();
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

	auto ASCPtr = OnwerActorPtr->GetAbilitySystemComponent();
	auto GameplayAbilitySpecPtr = ASCPtr->FindAbilitySpecFromHandle(WeaponUnit->Handle);
	if (!GameplayAbilitySpecPtr)
	{
		return false;
	}
	auto GAInsPtr = Cast<USkill_WeaponActive_Base>(GameplayAbilitySpecPtr->GetPrimaryInstance());
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
		GameplayAbilityTargetDashPtr->bIsAutomaticStop = bIsAutomaticStop;
		Payload.TargetData.Add(GameplayAbilityTargetDashPtr);
	}
	else if (WeaponUnit->WeaponUnitPtr->FirstSkill->GetUnitType().MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Weapon_HandProtection))
	{
		auto GameplayAbilityTargetDashPtr = new FGameplayAbilityTargetData_Skill_WeaponHandProtection;
		GameplayAbilityTargetDashPtr->WeaponPtr = Cast<AWeapon_HandProtection>(ActivedWeaponPtr);
		GameplayAbilityTargetDashPtr->bIsAutomaticStop = bIsAutomaticStop;
		Payload.TargetData.Add(GameplayAbilityTargetDashPtr);
	}
	else if (WeaponUnit->WeaponUnitPtr->FirstSkill->GetUnitType().MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Weapon_RangeTest))
	{
		auto GameplayAbilityTargetDashPtr = new FGameplayAbilityTargetData_Skill_WeaponActive_RangeTest;
		GameplayAbilityTargetDashPtr->WeaponPtr = Cast<AWeapon_RangeTest>(ActivedWeaponPtr);
		GameplayAbilityTargetDashPtr->bIsAutomaticStop = bIsAutomaticStop;
		Payload.TargetData.Add(GameplayAbilityTargetDashPtr);
	}

	return ASCPtr->TriggerAbilityFromGameplayEvent(
		WeaponUnit->Handle,
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

	auto ActiveSkillUnitPtr = Cast<UActiveSkillUnit>((*SkillIter)->SkillUnitPtr);
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
				auto GAInsPtr = Cast<USkill_Active_Base>(ActiveSkillUnitPtr->GAInstPtr);
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
				auto GAInsPtr = Cast<USkill_Active_Base>(ActiveSkillUnitPtr->GAInstPtr);
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

bool UInteractiveSkillComponent::ActivedCorrespondingWeapon(UActiveSkillUnit* ActiveSkillUnitPtr)
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

void UInteractiveSkillComponent::RemoveSkill(const TMap<FGameplayTag, TSharedPtr<FSkillSocketInfo>>& InSkillsMap)
{
	auto OnwerActorPtr = GetOwner<ACharacterBase>();
	if (!OnwerActorPtr)
	{
		return;
	}

	for (const auto& Iter : SkillsMap)
	{
		auto PreviouIter = InSkillsMap.Find(Iter.Key);
		if (!PreviouIter)
		{
			if (
				(Iter.Value->SkillUnitPtr) &&
				(Iter.Value->SkillUnitPtr->Level > 0)
				)
			{
				if (
					Iter.Value->SkillUnitPtr->GetUnitType().MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Passve)
					)
				{
					auto PassiveSkillUnitPtr = Cast<UPassiveSkillUnit>((*PreviouIter)->SkillUnitPtr);
					if (PassiveSkillUnitPtr)
					{
						auto PassiveSkillExtendInfoPtr = PassiveSkillUnitPtr->GetTableRowUnit_PassiveSkillExtendInfo();
						for (const auto& ElementIter : PassiveSkillExtendInfoPtr->AddtionalElementMap)
						{
							switch (ElementIter.Key)
							{
							case EWuXingType::kGold:
							{
								OnwerActorPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().Element.GoldElement.RemoveCurrentValue(
									PassiveSkillUnitPtr->PropertuModify_GUID
								);
							}
							break;
							}
						}
					}
				}

				if (
					Iter.Value->SkillUnitPtr->GetUnitType().MatchesTag(UGameplayTagsSubSystem::GetInstance()->TalentSocket)
					)
				{
					OnwerActorPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().TalentSPtr.Reset();
				}
				else
				{
					OnwerActorPtr->GetAbilitySystemComponent()->ClearAbility(Iter.Value->SkillUnitPtr->GAInstPtr->GetCurrentAbilitySpecHandle());
				}
			}
			else
			{
			}
		}
	}
}

void UInteractiveSkillComponent::AddSkill(const TMap<FGameplayTag, TSharedPtr<FSkillSocketInfo>>& InSkillsMap)
{
	auto OnwerActorPtr = GetOwner<ACharacterBase>();
	if (!OnwerActorPtr)
	{
		return;
	}

	auto MakeGameplayAbilitySpec = [OnwerActorPtr](USkillUnit* SkillUnitPtr)
		{
			FGameplayAbilitySpec GameplayAbilitySpec(
				SkillUnitPtr->GetSkillClass(),
				SkillUnitPtr->Level
			);

			GameplayAbilitySpec.GameplayEventData = MakeShared<FGameplayEventData>();

			auto GameplayAbilityTargetDataPtr = new FGameplayAbilityTargetData_Skill;

			GameplayAbilityTargetDataPtr->SkillUnitPtr = SkillUnitPtr;

			GameplayAbilitySpec.GameplayEventData->TargetData.Add(GameplayAbilityTargetDataPtr);

			const auto Handle = OnwerActorPtr->GetAbilitySystemComponent()->GiveAbility(GameplayAbilitySpec);

			auto GameplayAbilitySpecPtr = OnwerActorPtr->GetAbilitySystemComponent()->FindAbilitySpecFromHandle(Handle);
			if (!GameplayAbilitySpecPtr)
			{
				return;
			}
			auto GAInsPtr = Cast<USkill_Base>(GameplayAbilitySpecPtr->GetPrimaryInstance());
			if (!GAInsPtr)
			{
				return;
			}
			SkillUnitPtr->GAInstPtr = GAInsPtr;
		};

	for (const auto& Iter : InSkillsMap)
	{
		auto PreviouIter = SkillsMap.Find(Iter.Key);
		if (!PreviouIter)
		{
			if (Iter.Value && Iter.Value->SkillUnitPtr)
			{
				if (Iter.Value->SkillUnitPtr->Level <= 0)
				{
					continue;
				}
				if (
					Iter.Value->SkillUnitPtr->GetUnitType().MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Passve)
					)
				{
					auto PassiveSkillUnitPtr = Cast<UPassiveSkillUnit>(Iter.Value->SkillUnitPtr);
					if (PassiveSkillUnitPtr)
					{
						auto PassiveSkillExtendInfoPtr = PassiveSkillUnitPtr->GetTableRowUnit_PassiveSkillExtendInfo();
						if (PassiveSkillExtendInfoPtr->AddtionalElementMap.IsEmpty())
						{
							SkillsMap.Add(Iter.Value->SkillSocket, Iter.Value);
							MakeGameplayAbilitySpec(Iter.Value->SkillUnitPtr);
						}
						else
						{
							for (const auto& ElementIter : PassiveSkillExtendInfoPtr->AddtionalElementMap)
							{
								switch (ElementIter.Key)
								{
								case EWuXingType::kGold:
								{
									OnwerActorPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().Element.GoldElement.AddCurrentValue(
										ElementIter.Value,
										PassiveSkillUnitPtr->PropertuModify_GUID
									);
								}
								break;
								}
							}
						}
					}
				}
				else if (
					Iter.Value->SkillUnitPtr->GetUnitType().MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Active) ||
					Iter.Value->SkillUnitPtr->GetUnitType().MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Weapon)
					)
				{
					SkillsMap.Add(Iter.Value->SkillSocket, Iter.Value);
					MakeGameplayAbilitySpec(Iter.Value->SkillUnitPtr);
				}
				else if (
					Iter.Value->SkillUnitPtr->GetUnitType().MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Talent)
					)
				{
					if (
						Iter.Value->SkillUnitPtr->GetUnitType().MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Talent_NuQi)
						)
					{
						OnwerActorPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().TalentSPtr = MakeShared<FTalent_NuQi>();
						SkillsMap.Add(Iter.Value->SkillSocket, Iter.Value);
						MakeGameplayAbilitySpec(Iter.Value->SkillUnitPtr);
					}
					else if (
						Iter.Value->SkillUnitPtr->GetUnitType().MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Talent_YinYang)
						)
					{
						OnwerActorPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().TalentSPtr = MakeShared<FTalent_YinYang>();
						SkillsMap.Add(Iter.Value->SkillSocket, Iter.Value);
						MakeGameplayAbilitySpec(Iter.Value->SkillUnitPtr);
					}
				}
			}
		}
	}

	SkillsMap = InSkillsMap;
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

void UInteractiveSkillComponent::RegisterMultiGAs(
	const TMap<FGameplayTag, TSharedPtr<FSkillSocketInfo>>& InSkillsMap, bool bIsGenerationEvent
)
{
	auto OnwerActorPtr = GetOwner<ACharacterBase>();
	if (!OnwerActorPtr)
	{
		return;
	}

	// 移除未配置的
	RemoveSkill(InSkillsMap);

	// 注册新添加的
	AddSkill(InSkillsMap);

	if (bIsGenerationEvent)
	{
		GenerationCanbeActiveEvent();
	}
}

const TMap<FGameplayTag, TSharedPtr<FSkillSocketInfo>>& UInteractiveSkillComponent::GetSkills() const
{
	return SkillsMap;
}
