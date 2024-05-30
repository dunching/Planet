
#include "EquipmentElementComponent.h"

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

UEquipmentElementComponent::UEquipmentElementComponent(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 1.f / 10;
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

	auto OnwerActorPtr = GetOwner<ACharacterBase>();
	if (OnwerActorPtr)
	{
		auto GASPtr = OnwerActorPtr->GetAbilitySystemComponent();
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

FName UEquipmentElementComponent::ComponentName = TEXT("EquipmentItemsComponent");

void UEquipmentElementComponent::InitialBaseGAs()
{
	auto OnwerActorPtr = GetOwner<ACharacterBase>();
	if (OnwerActorPtr)
	{
		auto GASPtr = OnwerActorPtr->GetAbilitySystemComponent();

		GASPtr->ClearAllAbilities();
		GASPtr->InitAbilityActorInfo(OnwerActorPtr, OnwerActorPtr);

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
				auto GAInsPtr = Cast<USkill_Base>(GASpecPtr->GetPrimaryInstance());
				if (GAInsPtr)
				{
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

void UEquipmentElementComponent::GenerationCanbeActivedInfo()
{
	CanbeActivedInfoAry.Empty();

	// 响应武器
	{
		TSharedPtr < FCanbeActivedInfo > CanbeActivedInfoSPtr = MakeShared<FCanbeActivedInfo>();
		CanbeActivedInfoSPtr->Type = FCanbeActivedInfo::EType::kWeaponActiveSkill;
		CanbeActivedInfoAry.Add(CanbeActivedInfoSPtr);
	}

	// 响应主动技能
	for (const auto & Iter : SkillsMap)
	{
		TSharedPtr < FCanbeActivedInfo > CanbeActivedInfoSPtr = MakeShared<FCanbeActivedInfo>();

		CanbeActivedInfoSPtr->Type = FCanbeActivedInfo::EType::kActiveSkill;
		CanbeActivedInfoSPtr->SkillSocket = Iter.Value->SkillSocket;

		CanbeActivedInfoAry.Add(CanbeActivedInfoSPtr);
	}
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

bool UEquipmentElementComponent::ActiveWeapon(EWeaponSocket InWeaponSocket)
{
	if (InWeaponSocket != CurrentActivedWeaponSocket)
	{
		auto OnwerActorPtr = GetOwner<ACharacterBase>();
		TSharedPtr < FWeaponSocketInfo > WeaponUnit;

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

bool UEquipmentElementComponent::SwitchWeapon()
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

bool UEquipmentElementComponent::ActiveSkill(const TSharedPtr<FCanbeActivedInfo>& CanbeActivedInfoSPtr)
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
			return false;
		}
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
		auto OnwerActorPtr = GetOwner<ACharacterBase>();
		if (!OnwerActorPtr)
		{
			return  false;
		}

		auto ASCPtr = OnwerActorPtr->GetAbilitySystemComponent();
		ASCPtr->TriggerAbilityFromGameplayEvent(
			WeaponUnit->Handle,
			ASCPtr->AbilityActorInfo.Get(),
			FGameplayTag::EmptyTag,
			&Payload,
			*ASCPtr
		);
	}
	else
	{
		auto SkillIter = SkillsMap.Find(CanbeActivedInfoSPtr->SkillSocket);
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
			auto GameplayAbilitySpecPtr = ASCPtr->FindAbilitySpecFromHandle((*SkillIter)->Handle);
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
				ASCPtr->TryActivateAbility((*SkillIter)->Handle);
				return true;
			}
			break;
			}
		}
		break;
		}
	}
	return false;
}

void UEquipmentElementComponent::CancelSkill(const TSharedPtr<FCanbeActivedInfo>& CanbeActivedInfoSPtr)
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
			ASCPtr->CancelAbilityHandle(WeaponUnit->Handle);
		}
	}
	else
	{
	}
}

bool UEquipmentElementComponent::ActivedCorrespondingWeapon(USkill_Active_Base* SkillGAPtr)
{
	if (SkillGAPtr && (SkillGAPtr->WeaponUnitType != EWeaponUnitType::kNone))
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
