
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
			auto GameplayAbilitySpecPtr = GASPtr->FindAbilitySpecFromHandle(Iter.Value->Handle);
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

void UEquipmentElementComponent::RegisterMultiGAs(const TMap<FGameplayTag, TSharedPtr<FSkillSocketInfo>>& InSkillsMap)
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
			if (Iter.Value->SkillUnit == SecondIter.Value->SkillUnit)
			{
				bIsHave = true;
				break;
			}
		}
		if (!bIsHave)
		{
			auto GASpecPtr = OnwerActorPtr->GetAbilitySystemComponent()->FindAbilitySpecFromHandle(Iter.Value->Handle);
			if (GASpecPtr)
			{
				auto GAInsPtr = Cast<UPlanetGameplayAbility>(GASpecPtr->GetPrimaryInstance());
				if (GAInsPtr)
				{
					GAInsPtr->ResetListLock();
				}
			}

			OnwerActorPtr->GetAbilitySystemComponent()->ClearAbility(Iter.Value->Handle);
		}
	}

	// 添加新增的技能
	decltype(SkillsMap)NewSkillMap;
	for (const auto& Iter : InSkillsMap)
	{
		bool bIsHave = false;
		for (const auto& SecondIter : SkillsMap)
		{
			if (Iter.Value->SkillUnit == SecondIter.Value->SkillUnit)
			{
				NewSkillMap.Add(SecondIter);
				bIsHave = true;
				break;
			}
		}
		if (!bIsHave)
		{
			switch (Iter.Value->SkillUnit->SkillType)
			{
			case ESkillType::kActive:
			case ESkillType::kPassive:
			case ESkillType::kTalentPassive:
			case ESkillType::kWeaponActive:
			{
				auto& Ref = NewSkillMap.Add(Iter.Value->SkillSocket, Iter.Value);
				Ref->Handle = OnwerActorPtr->GetAbilitySystemComponent()->GiveAbility(
					FGameplayAbilitySpec(
						Iter.Value->SkillUnit->SkillClass,
						Iter.Value->SkillUnit->Level
					)
				);
			}
			break;
			default:
				break;
			}
		}
	}
	SkillsMap = NewSkillMap;
}

void UEquipmentElementComponent::RegisterTool(const TSharedPtr < FToolsSocketInfo>& InToolInfo)
{
	ToolsMap.Add(InToolInfo->SkillSocket, InToolInfo);
}

void UEquipmentElementComponent::RegisterCanbeActivedInfo(const TArray< TSharedPtr<FCanbeActivedInfo>>& InCanbeActivedInfoAry)
{
	CanbeActivedInfoAry = InCanbeActivedInfoAry;
}

void UEquipmentElementComponent::RegisterWeapon(
	const TSharedPtr < FWeaponSocketInfo>& FirstWeaponSocketInfo,
	const TSharedPtr < FWeaponSocketInfo>& SecondWeaponSocketInfo
)
{
	FirstWeaponUnit = FirstWeaponSocketInfo;
	SecondaryWeaponUnit = SecondWeaponSocketInfo;
}

void UEquipmentElementComponent::GetWeapon(
	TSharedPtr < FWeaponSocketInfo>& FirstWeaponSocketInfo,
	TSharedPtr < FWeaponSocketInfo>& SecondWeaponSocketInfo
)const
{
	FirstWeaponSocketInfo = FirstWeaponUnit;
	SecondWeaponSocketInfo = SecondaryWeaponUnit;
}

AWeapon_Base* UEquipmentElementComponent::ActiveWeapon(EWeaponSocket InWeaponSocket)
{
	if (InWeaponSocket != CurrentActivedWeaponSocket)
	{
		auto OnwerActorPtr = GetOwner<ACharacterBase>();
		TSharedPtr < FWeaponSocketInfo > WeaponUnit;
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
					return ActivedWeaponPtr;
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
					return ActivedWeaponPtr;
				}
			}
			break;
			}

			auto GASpecPtr = OnwerActorPtr->GetAbilitySystemComponent()->FindAbilitySpecFromHandle(WeaponUnit->Handle);
			if (GASpecPtr)
			{
				auto GAInsPtr = Cast<UPlanetGameplayAbility>(GASpecPtr->GetPrimaryInstance());
				if (GAInsPtr)
				{
					GAInsPtr->ResetListLock();
				}
			}

			OnwerActorPtr->GetAbilitySystemComponent()->ClearAbility(WeaponUnit->Handle);
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
			}
			break;
			}

			if (WeaponUnit->WeaponUnitPtr)
			{
				WeaponUnit->Handle = OnwerActorPtr->GetAbilitySystemComponent()->GiveAbility(
					FGameplayAbilitySpec(
						WeaponUnit->WeaponUnitPtr->FirstSkill->SkillClass,
						WeaponUnit->WeaponUnitPtr->FirstSkill->Level
					)
				);
			}
		}
		{
			FActorSpawnParameters ActorSpawnParameters;
			ActorSpawnParameters.Owner = OnwerActorPtr;

			if (ActivedWeaponPtr)
			{
				if (ActivedWeaponPtr->IsA(WeaponUnit->WeaponUnitPtr->ToolActorClass))
				{
				}
				else
				{
					ActivedWeaponPtr->Destroy();

					ActivedWeaponPtr = GetWorld()->SpawnActor<AWeapon_Base>(WeaponUnit->WeaponUnitPtr->ToolActorClass, ActorSpawnParameters);

					OnwerActorPtr->SwitchAnimLink(WeaponUnit->WeaponUnitPtr->AnimLinkClassType);
				}
			}
			else
			{
				OnwerActorPtr->SwitchAnimLink(EAnimLinkClassType::kUnarmed);
			}
		}
		OnActivedWeaponChangedContainer.ExcuteCallback(InWeaponSocket);
	}

	return ActivedWeaponPtr;
}

AWeapon_Base* UEquipmentElementComponent::SwitchWeapon()
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

void UEquipmentElementComponent::RetractputWeapon()
{
	ActiveWeapon(EWeaponSocket::kNone);
}

EWeaponSocket UEquipmentElementComponent::GetActivedWeapon()
{
	return CurrentActivedWeaponSocket;
}

TSharedPtr < FSkillSocketInfo >UEquipmentElementComponent::FindSkill(const FGameplayTag& Tag)
{
	auto Iter = SkillsMap.Find(Tag);
	if (Iter)
	{
		return *Iter;
	}

	return nullptr;
}

TSharedPtr < FToolsSocketInfo> UEquipmentElementComponent::FindTool(const FGameplayTag& Tag)
{
	auto Iter = ToolsMap.Find(Tag);
	if (Iter)
	{
		return *Iter;
	}

	return nullptr;
}

const TMap<FGameplayTag, TSharedPtr<FSkillSocketInfo>>& UEquipmentElementComponent::GetSkills() const
{
	return SkillsMap;
}

const TMap<FGameplayTag, TSharedPtr<FToolsSocketInfo>>& UEquipmentElementComponent::GetTools() const
{
	return ToolsMap;
}

const TArray<TSharedPtr<FCanbeActivedInfo>>& UEquipmentElementComponent::GetCanbeActivedInfo() const
{
	return CanbeActivedInfoAry;
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

void UEquipmentElementComponent::ActiveSkill(const TSharedPtr<FCanbeActivedInfo>& CanbeActivedInfoSPtr)
{
	if (CanbeActivedInfoSPtr->Type == FCanbeActivedInfo::EType::kWeaponActiveSkill)
	{
		TSharedPtr < FWeaponSocketInfo > WeaponUnit;
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
			return;
		}
		}

		auto OnwerActorPtr = GetOwner<ACharacterBase>();
		if (OnwerActorPtr)
		{
			auto ASCPtr = OnwerActorPtr->GetAbilitySystemComponent();
			auto GameplayAbilitySpecPtr = ASCPtr->FindAbilitySpecFromHandle(WeaponUnit->Handle);
			if (!GameplayAbilitySpecPtr)
			{
				return;
			}
			auto GAInsPtr = Cast<USkill_Base>(GameplayAbilitySpecPtr->GetPrimaryInstance());
			if (!GAInsPtr)
			{
				return;
			}

			if (!ActivedCorrespondingWeapon(GAInsPtr))
			{
				return;
			}

			FGameplayEventData Payload;
			switch (WeaponUnit->WeaponUnitPtr->FirstSkill->GetSceneElementType<ESkillUnitType>())
			{
			case ESkillUnitType::kHumanSkill_WeaponActive_PickAxe_Attack1:
			{
				auto GameplayAbilityTargetDashPtr = new FGameplayAbilityTargetData_Skill_PickAxe;
				GameplayAbilityTargetDashPtr->WeaponPtr = Cast<AWeapon_PickAxe>(ActivedWeaponPtr);
				Payload.TargetData.Add(GameplayAbilityTargetDashPtr);
			}
			break;
			case ESkillUnitType::kHumanSkill_WeaponActive_HandProtection_Attack1:
			{
				auto GameplayAbilityTargetDashPtr = new FGameplayAbilityTargetData_Skill_WeaponHandProtection;
				GameplayAbilityTargetDashPtr->WeaponPtr = Cast<AWeapon_HandProtection>(ActivedWeaponPtr);
				Payload.TargetData.Add(GameplayAbilityTargetDashPtr);
			}
			break;
			case ESkillUnitType::kHumanSkill_WeaponActive_RangeTest:
			{
				auto GameplayAbilityTargetDashPtr = new FGameplayAbilityTargetData_Skill_WeaponActive_RangeTest;
				GameplayAbilityTargetDashPtr->WeaponPtr = Cast<AWeapon_RangeTest>(ActivedWeaponPtr);
				Payload.TargetData.Add(GameplayAbilityTargetDashPtr);
			}
			break;
			}
			ASCPtr->TriggerAbilityFromGameplayEvent(
				WeaponUnit->Handle,
				ASCPtr->AbilityActorInfo.Get(),
				FGameplayTag::EmptyTag,
				&Payload,
				*ASCPtr
			);
		}
	}
	else
	{
		// 		auto SkillIter = SkillsMap.Find(CanbeActivedInfoSPtr->SkillSocket);
		// 		switch (SkillIter->SkillUnit->SkillType)
		// 		{
		// 		case ESkillType::kActive:
		// 		{
		// 			switch (SkillsSocketInfo.SkillUnit->GetSceneElementType<ESkillUnitType>())
		// 			{
		// 			case ESkillUnitType::kHumanSkill_Active_Displacement:
		// 			case ESkillUnitType::kHumanSkill_Active_GroupTherapy:
		// 			{
		// 				auto OnwerActorPtr = GetOwner<ACharacterBase>();
		// 				if (OnwerActorPtr)
		// 				{
		// 					auto ASCPtr = OnwerActorPtr->GetAbilitySystemComponent();
		// 					ASCPtr->TryActivateAbility(SkillsSocketInfo.Handle);
		// 				}
		// 			}
		// 			break;
		// 			}
		// 		}
		// 		break;
		// 		}
	}
}

void UEquipmentElementComponent::CancelSkill(const TSharedPtr<FCanbeActivedInfo>& CanbeActivedInfoSPtr)
{
	// 	if (SkillsSocketInfo.bIsWeaponSkill)
	// 	{
	// 		FWeaponSocketInfo WeaponUnit;
	// 		switch (CurrentActivedWeaponSocket)
	// 		{
	// 		case EWeaponSocket::kMain:
	// 		{
	// 			WeaponUnit = FirstWeaponUnit;
	// 		}
	// 		break;
	// 		case EWeaponSocket::kSecondary:
	// 		{
	// 			WeaponUnit = SecondaryWeaponUnit;
	// 		}
	// 		break;
	// 		default:
	// 		{
	// 			return;
	// 		}
	// 		}
	// 
	// 	}
	// 	else
	// 	{
	// 		auto OnwerActorPtr = GetOwner<ACharacterBase>();
	// 		if (OnwerActorPtr)
	// 		{
	// 			auto ASCPtr = OnwerActorPtr->GetAbilitySystemComponent();
	// 			ASCPtr->CancelAbilityHandle(SkillsSocketInfo.Handle);
	// 		}
	// 	}
}

bool UEquipmentElementComponent::ActivedCorrespondingWeapon(USkill_Base* SkillGAPtr)
{
	if (SkillGAPtr)
	{
		TSharedPtr < FWeaponSocketInfo > WeaponUnit;
		TSharedPtr < FWeaponSocketInfo > OtherWeaponUnit;
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
			WeaponUnit->WeaponUnitPtr &&
			(WeaponUnit->WeaponUnitPtr->GetSceneElementType<EWeaponUnitType>() == SkillGAPtr->WeaponUnitType)
			)
		{
			return true;
		}
		else if (
			OtherWeaponUnit->WeaponUnitPtr &&
			(OtherWeaponUnit->WeaponUnitPtr->GetSceneElementType<EWeaponUnitType>() == SkillGAPtr->WeaponUnitType)
			)
		{
			SwitchWeapon();
			return true;
		}
	}

	return false;
}
