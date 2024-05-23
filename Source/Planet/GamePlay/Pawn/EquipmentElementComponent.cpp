
#include "EquipmentElementComponent.h"

#include "GameplayAbilitySpec.h"

#include "GAEvent.h"
#include "CharacterBase.h"
#include "CharacterAttributesComponent.h"
#include "GenerateType.h"
#include "PlanetPlayerState.h"
#include "BasicFuturesBase.h"
#include "GAEvent_Send.h"
#include "GAEvent_Received.h"
#include "SceneElement.h"
#include "Skill_Base.h"
#include "Weapon_Base.h"
#include "Skill_WeaponActive_PickAxe.h"
#include "Skill_WeaponActive_HandProtection.h"
#include "Skill_WeaponActive_RangeTest.h"
#include "Weapon_HandProtection.h"
#include "Weapon_PickAxe.h"
#include "Weapon_RangeTest.h"

UEquipmentElementComponent::UEquipmentElementComponent(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UEquipmentElementComponent::BeginPlay()
{
	Super::BeginPlay();

	// 群体伤害或治疗减益
	{
		struct GAEventModify_MultyTarget : public IGAEventModifyInterface
		{
			GAEventModify_MultyTarget(int32 InPriority) :
				IGAEventModifyInterface(InPriority)
			{
			}

			virtual void Modify(FGameplayAbilityTargetData_GAEvent& GameplayAbilityTargetData_GAEvent)override
			{
				if (GameplayAbilityTargetData_GAEvent.TargetActorAry.Num() > 1)
				{
					GameplayAbilityTargetData_GAEvent.Data.ADDamage =
						GameplayAbilityTargetData_GAEvent.Data.ADDamage / GameplayAbilityTargetData_GAEvent.TargetActorAry.Num();
				}
			}
		};
		AddGAEventModify(MakeShared<GAEventModify_MultyTarget>(9999));
	}
}

void UEquipmentElementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	auto CharacterPtr = GetOwner<ACharacterBase>();
	if (CharacterPtr)
	{
		auto GASPtr = CharacterPtr->GetAbilitySystemComponent();
		for (auto& Iter : SkillsMap)
		{
			auto GameplayAbilitySpecPtr = GASPtr->FindAbilitySpecFromHandle(Iter.Value.Handle);
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
}

FName UEquipmentElementComponent::ComponentName = TEXT("EquipmentItemsComponent");

void UEquipmentElementComponent::InitialBaseGAs()
{
	auto CharacterPtr = GetOwner<ACharacterBase>();
	if (CharacterPtr)
	{
		auto GASPtr = CharacterPtr->GetAbilitySystemComponent();

		GASPtr->ClearAllAbilities();
		GASPtr->InitAbilityActorInfo(CharacterPtr, CharacterPtr);

		SendEventHandle = GASPtr->GiveAbility(
			FGameplayAbilitySpec(UGAEvent_Send::StaticClass(), 1)
		);

		ReceivedEventHandle = GASPtr->GiveAbility(
			FGameplayAbilitySpec(UGAEvent_Received::StaticClass(), 1)
		);

		for (auto Iter : CharacterAbilities)
		{
			GASPtr->GiveAbility(
				FGameplayAbilitySpec(Iter, 1)
			);
		}
	}
}

void UEquipmentElementComponent::OnSendEventModifyData(FGameplayAbilityTargetData_GAEvent& OutGAEventData)
{
	for (auto Iter : SendEventModifysMap)
	{
		Iter->Modify(OutGAEventData);
	}
}

void UEquipmentElementComponent::OnReceivedEventModifyData(FGameplayAbilityTargetData_GAEvent& OutGAEventData)
{
	for (auto Iter : ReceivedEventModifysMap)
	{
		Iter->Modify(OutGAEventData);
	}
}

void UEquipmentElementComponent::RegisterMultiGAs(const TMap<FGameplayTag, FSkillsSocketInfo>& InSkillsMap)
{
	auto OnwerActorPtr = GetOwner<ACharacterBase>();
	if (!OnwerActorPtr)
	{
		return;
	}

	// 移除缺失的技能
	for (const auto& Iter : SkillsMap)
	{
		bool bIsHave = false;
		for (const auto& SecondIter : InSkillsMap)
		{
			if (Iter.Value.SkillUnit == SecondIter.Value.SkillUnit)
			{
				bIsHave = true;
				break;
			}
		}
		if (!bIsHave)
		{
			auto GASpecPtr = OnwerActorPtr->GetAbilitySystemComponent()->FindAbilitySpecFromHandle(Iter.Value.Handle);
			if (GASpecPtr)
			{
				auto GAInsPtr = Cast<UPlanetGameplayAbility>(GASpecPtr->GetPrimaryInstance());
				if (GAInsPtr)
				{
					GAInsPtr->ResetListLock();
				}
			}

			OnwerActorPtr->GetAbilitySystemComponent()->ClearAbility(Iter.Value.Handle);
		}
	}

	// 添加新增的技能
	decltype(SkillsMap)NewSkillMap;
	for (const auto& Iter : InSkillsMap)
	{
		bool bIsHave = false;
		for (const auto& SecondIter : SkillsMap)
		{
			if (Iter.Value.SkillUnit == SecondIter.Value.SkillUnit)
			{
				NewSkillMap.Add(SecondIter);
				bIsHave = true;
				break;
			}
		}
		if (!bIsHave)
		{
			switch (Iter.Value.SkillUnit->SkillType)
			{
			case ESkillType::kActive:
			case ESkillType::kWeaponActive:
			case ESkillType::kPassive:
			case ESkillType::kTalentPassive:
			default:
			{
				auto& Ref = NewSkillMap.Add(Iter.Value.SkillSocket, Iter.Value);

				Ref.Handle = OnwerActorPtr->GetAbilitySystemComponent()->GiveAbility(
					FGameplayAbilitySpec(
						Iter.Value.SkillUnit->SkillClass,
						Iter.Value.SkillUnit->Level
					)
				);
			}
			break;
			}
		}
	}
	SkillsMap = NewSkillMap;
}

void UEquipmentElementComponent::RegisterTool(const FToolsSocketInfo& InToolInfo)
{
	ToolsMap.Add(InToolInfo.SkillSocket, InToolInfo);
}

void UEquipmentElementComponent::SetMainWeapon(UWeaponUnit* InMainWeaponSPtr)
{
	MainWeaponUnitPtr = InMainWeaponSPtr;
}

UWeaponUnit* UEquipmentElementComponent::GetMainWeaponUnit() const
{
	return MainWeaponUnitPtr;
}

UWeaponUnit* UEquipmentElementComponent::GetSecondaryWeaponUnit() const
{
	return SecondaryWeaponUnitPtr;
}

void UEquipmentElementComponent::SetSecondaryWeapon(UWeaponUnit* InMainWeaponSPtr)
{
	SecondaryWeaponUnitPtr = InMainWeaponSPtr;
}

AWeapon_Base* UEquipmentElementComponent::ActiveWeapon(EWeaponSocket WeaponSocket)
{
	switch (WeaponSocket)
	{
	case EWeaponSocket::kMain:
	{
		auto OnwerActorPtr = GetOwner<ACharacterBase>();

		FActorSpawnParameters ActorSpawnParameters;
		ActorSpawnParameters.Owner = OnwerActorPtr;

		if (ActivedWeaponPtr)
		{
			if (ActivedWeaponPtr->IsA(MainWeaponUnitPtr->ToolActorClass))
			{
				break;
			}
			else
			{
				ActivedWeaponPtr->Destroy();
			}
		}

		ActivedWeaponPtr = GetWorld()->SpawnActor<AWeapon_Base>(MainWeaponUnitPtr->ToolActorClass, ActorSpawnParameters);

		OnwerActorPtr->SwitchAnimLink(MainWeaponUnitPtr->AnimLinkClassType);
	}
	break;
	case EWeaponSocket::kSecondary:
	{
		auto OnwerActorPtr = GetOwner<ACharacterBase>();

		FActorSpawnParameters ActorSpawnParameters;
		ActorSpawnParameters.Owner = OnwerActorPtr;

		if (ActivedWeaponPtr)
		{
			if (ActivedWeaponPtr->IsA(SecondaryWeaponUnitPtr->ToolActorClass))
			{
				break;
			}
			else
			{
				ActivedWeaponPtr->Destroy();
			}
		}

		ActivedWeaponPtr = GetWorld()->SpawnActor<AWeapon_Base>(SecondaryWeaponUnitPtr->ToolActorClass, ActorSpawnParameters);

		OnwerActorPtr->SwitchAnimLink(SecondaryWeaponUnitPtr->AnimLinkClassType);
	}
	break;
	case EWeaponSocket::kNone:
	{
		if (ActivedWeaponPtr)
		{
			ActivedWeaponPtr->Destroy();
			ActivedWeaponPtr = nullptr;

			auto OnwerActorPtr = GetOwner<ACharacterBase>();
			OnwerActorPtr->SwitchAnimLink(EAnimLinkClassType::kUnarmed);
		}
	}
	break;
	}

	return ActivedWeaponPtr;
}

AWeapon_Base* UEquipmentElementComponent::GetActivedWeapon()
{
	return ActivedWeaponPtr;
}

FSkillsSocketInfo UEquipmentElementComponent::FindSkill(const FGameplayTag& Tag)
{
	auto Iter = SkillsMap.Find(Tag);
	if (Iter)
	{
		return *Iter;
	}

	return FSkillsSocketInfo();
}

FToolsSocketInfo UEquipmentElementComponent::FindTool(const FGameplayTag& Tag)
{
	auto Iter = ToolsMap.Find(Tag);
	if (Iter)
	{
		return *Iter;
	}

	return FToolsSocketInfo();
}

const TMap<FGameplayTag, FSkillsSocketInfo>& UEquipmentElementComponent::GetSkills() const
{
	return SkillsMap;
}

const TMap<FGameplayTag, FToolsSocketInfo>& UEquipmentElementComponent::GetTools() const
{
	return ToolsMap;
}

void UEquipmentElementComponent::AddGAEventModify(const TSharedPtr<IGAEventModifyInterface>& GAEventModifySPtr)
{
	for (bool bIsContinue = true; bIsContinue;)
	{
		bIsContinue = false;
		GAEventModifySPtr->ID = FMath::RandRange(1, std::numeric_limits<int32>::max());
		for (const auto& Iter : SendEventModifysMap)
		{
			if (Iter->ID == GAEventModifySPtr->ID)
			{
				bIsContinue = true;
				break;
			}
		}
	}
	SendEventModifysMap.emplace(GAEventModifySPtr);
}

void UEquipmentElementComponent::RemoveGAEventModify(const TSharedPtr<IGAEventModifyInterface>& GAEventModifySPtr)
{
	for (auto Iter = SendEventModifysMap.begin(); Iter != SendEventModifysMap.end(); Iter++)
	{
		if ((*Iter)->ID == GAEventModifySPtr->ID)
		{
			SendEventModifysMap.erase(Iter);
			break;
		}
	}
}

const FGameplayTagContainer& UEquipmentElementComponent::GetCharacterTags() const
{
	return CharacterTags;
}

void UEquipmentElementComponent::AddTag(const FGameplayTag& Tag)
{
	CharacterTags.AddTag(Tag);

	TagsModifyHandleContainer.ExcuteCallback(ETagChangeType::kAdd, Tag);
}

void UEquipmentElementComponent::RemoveTag(const FGameplayTag& Tag)
{
	CharacterTags.RemoveTag(Tag);

	TagsModifyHandleContainer.ExcuteCallback(ETagChangeType::kRemove, Tag);
}

void UEquipmentElementComponent::ActiveSkill(const FSkillsSocketInfo& SkillsSocketInfo, EWeaponSocket WeaponSocket)
{
	switch (SkillsSocketInfo.SkillUnit->SkillType)
	{
	case ESkillType::kWeaponActive:
	{
		AWeapon_Base* WeaponPtr = nullptr;

		auto OnwerActorPtr = GetOwner<ACharacterBase>();
		if (OnwerActorPtr)
		{
			switch (WeaponSocket)
			{
			case EWeaponSocket::kMain:
			{
				WeaponPtr = OnwerActorPtr->GetEquipmentItemsComponent()->ActiveWeapon(EWeaponSocket::kMain);
			}
			break;
			case EWeaponSocket::kSecondary:
			{
				WeaponPtr = OnwerActorPtr->GetEquipmentItemsComponent()->ActiveWeapon(EWeaponSocket::kSecondary);
			}
			break;
			default:
			{

			}
			break;
			}

			FGameplayEventData Payload;
			switch (SkillsSocketInfo.SkillUnit->GetSceneElementType<ESkillUnitType>())
			{
			case ESkillUnitType::kHumanSkill_WeaponActive_PickAxe_Attack1:
			{
				auto GameplayAbilityTargetDashPtr = new FGameplayAbilityTargetData_Skill_PickAxe;
				GameplayAbilityTargetDashPtr->WeaponPtr = Cast<AWeapon_PickAxe>(WeaponPtr);
				Payload.TargetData.Add(GameplayAbilityTargetDashPtr);
			}
			break;
			case ESkillUnitType::kHumanSkill_WeaponActive_HandProtection_Attack1:
			{
				auto GameplayAbilityTargetDashPtr = new FGameplayAbilityTargetData_Skill_WeaponHandProtection;
				GameplayAbilityTargetDashPtr->WeaponPtr = Cast<AWeapon_HandProtection>(WeaponPtr);
				Payload.TargetData.Add(GameplayAbilityTargetDashPtr);
			}
			break;
			case ESkillUnitType::kHumanSkill_WeaponActive_RangeTest:
			{
				auto GameplayAbilityTargetDashPtr = new FGameplayAbilityTargetData_Skill_WeaponActive_RangeTest;
				GameplayAbilityTargetDashPtr->WeaponPtr = Cast<AWeapon_RangeTest>(WeaponPtr);
				Payload.TargetData.Add(GameplayAbilityTargetDashPtr);
			}
			break;
			}
			auto ASCPtr = OnwerActorPtr->GetAbilitySystemComponent();
			ASCPtr->TriggerAbilityFromGameplayEvent(
				SkillsSocketInfo.Handle,
				ASCPtr->AbilityActorInfo.Get(),
				FGameplayTag(),
				&Payload,
				*ASCPtr
			);
		}
	}
	break;
	case ESkillType::kActive:
	{
		switch (SkillsSocketInfo.SkillUnit->GetSceneElementType<ESkillUnitType>())
		{
		case ESkillUnitType::kHumanSkill_Active_Displacement:
		case ESkillUnitType::kHumanSkill_Active_GroupTherapy:
		{
			auto OnwerActorPtr = GetOwner<ACharacterBase>();
			if (OnwerActorPtr)
			{
				auto ASCPtr = OnwerActorPtr->GetAbilitySystemComponent();
				ASCPtr->TryActivateAbility(SkillsSocketInfo.Handle);
			}
		}
		break;
		}
	}
	break;
	}
}

void UEquipmentElementComponent::CancelSkill(const FSkillsSocketInfo& SkillsSocketInfo)
{
	auto OnwerActorPtr = GetOwner<ACharacterBase>();
	if (OnwerActorPtr)
	{
		auto ASCPtr = OnwerActorPtr->GetAbilitySystemComponent();
		ASCPtr->CancelAbilityHandle(SkillsSocketInfo.Handle);
	}
}
