
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
#include "InputComponent/InputProcessorSubSystem.h"
#include "Tool_PickAxe.h"
#include "HumanRegularProcessor.h"
#include "HumanCharacter.h"

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
			for (const auto SkillHandleIter : Iter.Value->HandleAry)
			{
				auto GameplayAbilitySpecPtr = GASPtr->FindAbilitySpecFromHandle(SkillHandleIter);
				if (!GameplayAbilitySpecPtr)
				{
					continue;
				}
				auto GAInsPtr = Cast<USkill_Base>(GameplayAbilitySpecPtr->GetPrimaryInstance());
				if (!GAInsPtr)
				{
					continue;
				}

				GAInsPtr->Tick(DeltaTime);
			}
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
			auto GASpecPtr = OnwerActorPtr->GetAbilitySystemComponent()->FindAbilitySpecFromHandle(WeaponUnit->Handle);
			if (GASpecPtr)
			{
				auto GAInsPtr = Cast<USkill_Base>(GASpecPtr->GetPrimaryInstance());
				if (!GAInsPtr)
				{
					return;
				}
				GAInsPtr->Tick(DeltaTime);
			}
		}
	}
}

void UInteractiveSkillComponent::InitialBaseGAs()
{
	auto OnwerActorPtr = GetOwner<ACharacterBase>();
	if (OnwerActorPtr)
	{
		auto GASPtr = OnwerActorPtr->GetAbilitySystemComponent();

		GASPtr->InitAbilityActorInfo(OnwerActorPtr, OnwerActorPtr);

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
						WeaponUnit->WeaponUnitPtr->FirstSkill->SkillClass,
						WeaponUnit->WeaponUnitPtr->FirstSkill->Level
					)
				);

				FActorSpawnParameters ActorSpawnParameters;
				ActorSpawnParameters.Owner = OnwerActorPtr;

				ActivedWeaponPtr = GetWorld()->SpawnActor<AWeapon_Base>(WeaponUnit->WeaponUnitPtr->ToolActorClass, ActorSpawnParameters);

				OnwerActorPtr->SwitchAnimLink(WeaponUnit->WeaponUnitPtr->AnimLinkClassType);
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
		if (FirstWeaponUnit->WeaponUnitPtr)
		{
			return ActiveWeapon(EWeaponSocket::kMain);
		}
		else if (FirstWeaponUnit->WeaponUnitPtr)
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

void UInteractiveSkillComponent::CancelSkill_WeaponActive(const TSharedPtr<FCanbeActivedInfo>& CanbeActivedInfoSPtr)
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
	const TSharedPtr<FCanbeActivedInfo>& CanbeActivedInfoSPtr, bool bIsAutomaticStop /*= false*/
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
	switch (WeaponUnit->WeaponUnitPtr->FirstSkill->GetSceneElementType<ESkillUnitType>())
	{
	case ESkillUnitType::kHumanSkill_WeaponActive_PickAxe_Attack1:
	{
		auto GameplayAbilityTargetDashPtr = new FGameplayAbilityTargetData_Skill_PickAxe;
		GameplayAbilityTargetDashPtr->WeaponPtr = Cast<AWeapon_PickAxe>(ActivedWeaponPtr);
		GameplayAbilityTargetDashPtr->bIsAutomaticStop = bIsAutomaticStop;
		Payload.TargetData.Add(GameplayAbilityTargetDashPtr);
	}
	break;
	case ESkillUnitType::kHumanSkill_WeaponActive_HandProtection_Attack1:
	{
		auto GameplayAbilityTargetDashPtr = new FGameplayAbilityTargetData_Skill_WeaponHandProtection;
		GameplayAbilityTargetDashPtr->WeaponPtr = Cast<AWeapon_HandProtection>(ActivedWeaponPtr);
		GameplayAbilityTargetDashPtr->bIsAutomaticStop = bIsAutomaticStop;
		Payload.TargetData.Add(GameplayAbilityTargetDashPtr);
	}
	break;
	case ESkillUnitType::kHumanSkill_WeaponActive_RangeTest:
	{
		auto GameplayAbilityTargetDashPtr = new FGameplayAbilityTargetData_Skill_WeaponActive_RangeTest;
		GameplayAbilityTargetDashPtr->WeaponPtr = Cast<AWeapon_RangeTest>(ActivedWeaponPtr);
		GameplayAbilityTargetDashPtr->bIsAutomaticStop = bIsAutomaticStop;
		Payload.TargetData.Add(GameplayAbilityTargetDashPtr);
	}
	break;
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
	const TSharedPtr<FCanbeActivedInfo>& CanbeActivedInfoSPtr, bool bIsAutomaticStop /*= false*/
)
{
	auto SkillIter = SkillsMap.Find(CanbeActivedInfoSPtr->Socket);
	if (!SkillIter)
	{
		return  false;
	}

	switch ((*SkillIter)->SkillUnit->SkillType)
	{
	case ESkillType::kActive:
	{
		auto OnwerActorPtr = GetOwner<ACharacterBase>();
		if (!OnwerActorPtr)
		{
			return  false;
		}

		auto ASCPtr = OnwerActorPtr->GetAbilitySystemComponent();
		for (const auto SkillHandleIter : (*SkillIter)->HandleAry)
		{
			auto GameplayAbilitySpecPtr = ASCPtr->FindAbilitySpecFromHandle(SkillHandleIter);
			if (!GameplayAbilitySpecPtr)
			{
				return false;
			}
			auto GAInsPtr = Cast<USkill_Active_Base>(GameplayAbilitySpecPtr->GetPrimaryInstance());
			if (!GAInsPtr)
			{
				return false;
			}

			if (!ActivedCorrespondingWeapon(GAInsPtr))
			{
				return false;
			}

			switch ((*SkillIter)->SkillUnit->GetSceneElementType<ESkillUnitType>())
			{
			case ESkillUnitType::kNone:
			{
			}
			break;
			default:
			{
				return ASCPtr->TryActivateAbility(SkillHandleIter);
			}
			break;
			}
		}
	}
	break;
	}
	return false;
}

TArray<TSharedPtr<FCanbeActivedInfo>> UInteractiveSkillComponent::GetCanbeActiveAction() const
{
	return CanbeActiveSkillsAry;
}

bool UInteractiveSkillComponent::ActiveAction(
	const TSharedPtr<FCanbeActivedInfo>& CanbeActivedInfoSPtr, bool bIsAutomaticStop /*= false */
)
{
	switch (CanbeActivedInfoSPtr->Type)
	{
	case FCanbeActivedInfo::EType::kActiveSkill:
	{
		return ActiveSkill_Active(CanbeActivedInfoSPtr, bIsAutomaticStop);
	}
	break;
	case FCanbeActivedInfo::EType::kWeaponActiveSkill:
	{
		return ActiveSkill_WeaponActive(CanbeActivedInfoSPtr, bIsAutomaticStop);
	}
	break;
	default:
		break;
	}
	return false;
}

void UInteractiveSkillComponent::CancelAction(const TSharedPtr<FCanbeActivedInfo>& CanbeActivedInfoSPtr)
{
	switch (CanbeActivedInfoSPtr->Type)
	{
	case FCanbeActivedInfo::EType::kWeaponActiveSkill:
	{
		CancelSkill_WeaponActive(CanbeActivedInfoSPtr);
	}
	break;
	default:
		break;
	}
}

bool UInteractiveSkillComponent::ActivedCorrespondingWeapon(USkill_Active_Base* SkillGAPtr)
{
	if (SkillGAPtr && (SkillGAPtr->WeaponUnitType != EWeaponUnitType::kNone))
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
			(WeaponUnit->WeaponUnitPtr->GetSceneElementType<EWeaponUnitType>() == SkillGAPtr->WeaponUnitType)
			)
		{
			return true;
		}
		else if (
			OtherWeaponUnit &&
			OtherWeaponUnit->WeaponUnitPtr &&
			(OtherWeaponUnit->WeaponUnitPtr->GetSceneElementType<EWeaponUnitType>() == SkillGAPtr->WeaponUnitType)
			)
		{
			return SwitchWeapon();
		}
	}

	return false;
}

void UInteractiveSkillComponent::GenerationCanbeActiveEvent()
{
	CanbeActiveSkillsAry.Empty();

	// 响应武器
	{
		TSharedPtr<FCanbeActivedInfo > CanbeActivedInfoSPtr = MakeShared<FCanbeActivedInfo>();
		CanbeActivedInfoSPtr->Type = FCanbeActivedInfo::EType::kWeaponActiveSkill;
		CanbeActivedInfoSPtr->Key = EKeys::LeftMouseButton;
		CanbeActiveSkillsAry.Add(CanbeActivedInfoSPtr);
	}

	// 响应主动技能
	for (const auto& Iter : SkillsMap)
	{
		TSharedPtr<FCanbeActivedInfo > CanbeActivedInfoSPtr = MakeShared<FCanbeActivedInfo>();

		CanbeActivedInfoSPtr->Type = FCanbeActivedInfo::EType::kActiveSkill;
		CanbeActivedInfoSPtr->Key = Iter.Value->Key;
		CanbeActivedInfoSPtr->Socket = Iter.Value->SkillSocket;

		CanbeActiveSkillsAry.Add(CanbeActivedInfoSPtr);
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

	// 所有插槽都会被注册；要移除会把Level设置为0，
	for (const auto& Iter : InSkillsMap)
	{
		auto PreviouIter = SkillsMap.Find(Iter.Key);
		if (PreviouIter && *PreviouIter && (*PreviouIter)->SkillUnit)
		{
			if (
				(Iter.Value->SkillUnit) &&
				(Iter.Value->SkillUnit->Level > 0)
				)
			{
				continue;
			}
			else
			{
				switch ((*PreviouIter)->SkillUnit->SkillType)
				{
				case ESkillType::kPassive:
				{
					auto PassiveSkillUnitPtr = Cast<UPassiveSkillUnit>((*PreviouIter)->SkillUnit);
					if (PassiveSkillUnitPtr)
					{
						for (const auto& ElementIter : PassiveSkillUnitPtr->AddtionalElementMap)
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
				break;
				}

				switch ((*PreviouIter)->SkillUnit->GetSceneElementType<ESkillUnitType>())
				{
				case ESkillUnitType::kHumanSkill_Talent_NuQi:
				case ESkillUnitType::kHumanSkill_Talent_YinYang:
				{
					OnwerActorPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().TalentSPtr.Reset();
				}
				default:
				{
					for (const auto SkillHandleIter : Iter.Value->HandleAry)
					{
						OnwerActorPtr->GetAbilitySystemComponent()->ClearAbility(SkillHandleIter);
					}
					SkillsMap.Remove(Iter.Value->SkillSocket);
				}
				break;
				}
			}
		}

		if (Iter.Value && Iter.Value->SkillUnit)
		{
			if (Iter.Value->SkillUnit->Level <= 0)
			{
				continue;
			}
			switch (Iter.Value->SkillUnit->SkillType)
			{
			case ESkillType::kPassive:
			{
				auto PassiveSkillUnitPtr = Cast<UPassiveSkillUnit>(Iter.Value->SkillUnit);
				if (PassiveSkillUnitPtr)
				{
					if (PassiveSkillUnitPtr->AddtionalElementMap.IsEmpty())
					{
						auto& Ref = SkillsMap.Add(Iter.Value->SkillSocket, Iter.Value);
						Ref->HandleAry.Add(OnwerActorPtr->GetAbilitySystemComponent()->GiveAbility(
							FGameplayAbilitySpec(
								Iter.Value->SkillUnit->SkillClass,
								Iter.Value->SkillUnit->Level
							)
						));
					}
					else
					{
						for (const auto& ElementIter : PassiveSkillUnitPtr->AddtionalElementMap)
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
			case ESkillType::kActive:
			case ESkillType::kWeaponActive:
			{
				auto& Ref = SkillsMap.Add(Iter.Value->SkillSocket, Iter.Value);
				Ref->HandleAry.Add(OnwerActorPtr->GetAbilitySystemComponent()->GiveAbility(
					FGameplayAbilitySpec(
						Iter.Value->SkillUnit->SkillClass,
						Iter.Value->SkillUnit->Level
					)
				));
			}
			break;
			case ESkillType::kTalentPassive:
			{
				switch (Iter.Value->SkillUnit->GetSceneElementType<ESkillUnitType>())
				{
				case ESkillUnitType::kHumanSkill_Talent_NuQi:
				{
					OnwerActorPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().TalentSPtr = MakeShared<FTalent_NuQi>();
				}
				break;
				case ESkillUnitType::kHumanSkill_Talent_YinYang:
				{
					OnwerActorPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().TalentSPtr = MakeShared<FTalent_YinYang>();
				}
				break;
				}
				switch (Iter.Value->SkillUnit->GetSceneElementType<ESkillUnitType>())
				{
				case ESkillUnitType::kHumanSkill_Talent_NuQi:
				case ESkillUnitType::kHumanSkill_Talent_YinYang:
				{
					auto& Ref = SkillsMap.Add(Iter.Value->SkillSocket, Iter.Value);
					Ref->HandleAry.Add(OnwerActorPtr->GetAbilitySystemComponent()->GiveAbility(
						FGameplayAbilitySpec(
							Iter.Value->SkillUnit->SkillClass,
							Iter.Value->SkillUnit->Level
						)
					));
				}
				break;
				}
			}
			break;
			default:
				break;
			}
		}
	}

	if (bIsGenerationEvent)
	{
		GenerationCanbeActiveEvent();
	}
}

const TMap<FGameplayTag, TSharedPtr<FSkillSocketInfo>>& UInteractiveSkillComponent::GetSkills() const
{
	return SkillsMap;
}
