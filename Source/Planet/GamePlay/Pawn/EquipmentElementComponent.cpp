
#include "EquipmentElementComponent.h"

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

UEquipmentElementComponent::UEquipmentElementComponent(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 1.f / 10;
}

void UEquipmentElementComponent::BeginPlay()
{
	Super::BeginPlay();

#pragma region 结算效果修正
	// 输出

	// 群体伤害或治疗减益
	AddSendGroupEffectModify();
	// 伤害类型
	AddSendWuXingModify();

	// 接收

	// 五行之间的减免
	AddReceivedWuXingModify();

	// 基础属性
	AddReceivedModify();
#pragma endregion 结算效果修正
}

void UEquipmentElementComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RetractputWeapon();

	Super::EndPlay(EndPlayReason);
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

		// 五行技能
		GASPtr->GiveAbility(
			FGameplayAbilitySpec(
				Skill_Element_GoldClass
			)
		);
	}
}

void UEquipmentElementComponent::OnSendEventModifyData(FGameplayAbilityTargetData_GASendEvent& OutGAEventData)
{
	for (auto Iter : SendEventModifysMap)
	{
		Iter->Modify(OutGAEventData);
	}
}

void UEquipmentElementComponent::OnReceivedEventModifyData(FGameplayAbilityTargetData_GAReceivedEvent& OutGAEventData)
{
	for (auto Iter : ReceivedEventModifysMap)
	{
		Iter->Modify(OutGAEventData);
	}
}

void UEquipmentElementComponent::RegisterMultiGAs(
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
		GenerationCanbeActiveSkills();
	}
}

void UEquipmentElementComponent::GenerationCanbeActiveTools()
{
	CanbeActiveToolsAry.Empty();

	// 激活对应的工具
	for (const auto& Iter : ToolsMap)
	{
		TSharedPtr < FCanbeActivedInfo > CanbeActivedInfoSPtr = MakeShared<FCanbeActivedInfo>();

		CanbeActivedInfoSPtr->Type = FCanbeActivedInfo::EType::kSwitchToTool;
		CanbeActivedInfoSPtr->Key = Iter.Value->Key;
		CanbeActivedInfoSPtr->Socket = Iter.Value->SkillSocket;

		CanbeActiveToolsAry.Add(CanbeActivedInfoSPtr);
	}

	// “使用”一次这个工具
	{
		TSharedPtr < FCanbeActivedInfo > CanbeActivedInfoSPtr = MakeShared<FCanbeActivedInfo>();
		CanbeActivedInfoSPtr->Type = FCanbeActivedInfo::EType::kActiveTool;
		CanbeActivedInfoSPtr->Key = EKeys::LeftMouseButton;

		CanbeActiveToolsAry.Add(CanbeActivedInfoSPtr);
	}
}

const TArray<TSharedPtr<FCanbeActivedInfo>>& UEquipmentElementComponent::GetCanbeActivedTools() const
{
	return CanbeActiveToolsAry;
}

void UEquipmentElementComponent::RetractputTool()
{
	if (CurrentEquipmentPtr)
	{
		CurrentEquipmentPtr->Destroy();
		CurrentEquipmentPtr = nullptr;
	}

	PreviousTool = FGameplayTag::EmptyTag;
}

void UEquipmentElementComponent::RegisterTool(
	const TMap <FGameplayTag, TSharedPtr < FToolsSocketInfo>>& InToolInfoMap, bool bIsGenerationEvent
)
{
	for (const auto Iter : InToolInfoMap)
	{
		ToolsMap.Add(Iter.Key, Iter.Value);
	}

	if (bIsGenerationEvent)
	{
		GenerationCanbeActiveTools();
	}
}

void UEquipmentElementComponent::GenerationCanbeActiveSkills()
{
	CanbeActiveSkillsAry.Empty();

	// 响应武器
	{
		TSharedPtr < FCanbeActivedInfo > CanbeActivedInfoSPtr = MakeShared<FCanbeActivedInfo>();
		CanbeActivedInfoSPtr->Type = FCanbeActivedInfo::EType::kWeaponActiveSkill;
		CanbeActivedInfoSPtr->Key = EKeys::LeftMouseButton;
		CanbeActiveSkillsAry.Add(CanbeActivedInfoSPtr);
	}

	// 响应主动技能
	for (const auto& Iter : SkillsMap)
	{
		TSharedPtr < FCanbeActivedInfo > CanbeActivedInfoSPtr = MakeShared<FCanbeActivedInfo>();

		CanbeActivedInfoSPtr->Type = FCanbeActivedInfo::EType::kActiveSkill;
		CanbeActivedInfoSPtr->Key = Iter.Value->Key;
		CanbeActivedInfoSPtr->Socket = Iter.Value->SkillSocket;

		CanbeActiveSkillsAry.Add(CanbeActivedInfoSPtr);
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

EWeaponSocket UEquipmentElementComponent::GetActivedWeaponType()
{
	return CurrentActivedWeaponSocket;
}

TSharedPtr < FWeaponSocketInfo > UEquipmentElementComponent::GetActivedWeapon() const
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
		WeaponUnit = MakeShared<FWeaponSocketInfo>();
	}
	}
	return WeaponUnit;
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

ATool_Base* UEquipmentElementComponent::GetCurrentTool() const
{
	return CurrentEquipmentPtr;
}

const TArray<TSharedPtr<FCanbeActivedInfo>>& UEquipmentElementComponent::GetCanbeActivedSkills() const
{
	return CanbeActiveSkillsAry;
}

void UEquipmentElementComponent::AddSendEventModify(const TSharedPtr<IGAEventModifySendInterface>& GAEventModifySPtr)
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

void UEquipmentElementComponent::RemoveSendEventModify(const TSharedPtr<IGAEventModifySendInterface>& GAEventModifySPtr)
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

void UEquipmentElementComponent::AddReceviedEventModify(const TSharedPtr<IGAEventModifyReceivedInterface>& GAEventModifySPtr)
{
	for (bool bIsContinue = true; bIsContinue;)
	{
		bIsContinue = false;
		GAEventModifySPtr->ID = FMath::RandRange(1, std::numeric_limits<int32>::max());
		for (const auto& Iter : ReceivedEventModifysMap)
		{
			if (Iter->ID == GAEventModifySPtr->ID)
			{
				bIsContinue = true;
				break;
			}
		}
	}
	ReceivedEventModifysMap.emplace(GAEventModifySPtr);
}

void UEquipmentElementComponent::RemoveReceviedEventModify(const TSharedPtr<IGAEventModifyReceivedInterface>& GAEventModifySPtr)
{
	for (auto Iter = ReceivedEventModifysMap.begin(); Iter != ReceivedEventModifysMap.end(); Iter++)
	{
		if ((*Iter)->ID == GAEventModifySPtr->ID)
		{
			ReceivedEventModifysMap.erase(Iter);
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

bool UEquipmentElementComponent::ActiveSkill(
	const TSharedPtr<FCanbeActivedInfo>& CanbeActivedInfoSPtr, bool bIsAutomaticStop
)
{
	switch (CanbeActivedInfoSPtr->Type)
	{
	case FCanbeActivedInfo::EType::kSwitchToTool:
	{
		return ActiveSkill_SwitchToTool(CanbeActivedInfoSPtr, bIsAutomaticStop);
	}
	break;
	case FCanbeActivedInfo::EType::kActiveTool:
	{
		return ActiveSkill_ActiveTool(CanbeActivedInfoSPtr, bIsAutomaticStop);
	}
	break;
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

void UEquipmentElementComponent::CancelSkill_SwitchToTool(const TSharedPtr < FCanbeActivedInfo>& CanbeActivedInfoSPtr)
{

}

bool UEquipmentElementComponent::ActiveSkill_SwitchToTool(
	const TSharedPtr <FCanbeActivedInfo>& CanbeActivedInfoSPtr, bool bIsAutomaticStop /*= false*/
)
{
	if (PreviousTool == CanbeActivedInfoSPtr->Socket)
	{
		RetractputTool();
		UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<HumanProcessor::FHumanRegularProcessor>();
		return false;
	}

	auto ToolIter = ToolsMap.Find(CanbeActivedInfoSPtr->Socket);
	if (!ToolIter)
	{
		return  false;
	}

	PreviousTool = CanbeActivedInfoSPtr->Socket;

	FGameplayEventData Payload;
	switch ((*ToolIter)->ToolUnitPtr->GetSceneElementType<EToolUnitType>())
	{
	case EToolUnitType::kPickAxe:
	{
		auto OnwerActorPtr = GetOwner<ACharacterBase>();
		if (!OnwerActorPtr)
		{
			return  false;
		}
		OnwerActorPtr->SwitchAnimLink(EAnimLinkClassType::kPickAxe);

		FActorSpawnParameters ActorSpawnParameters;
		ActorSpawnParameters.Owner = OnwerActorPtr;
		auto AxePtr = GetWorld()->SpawnActor<ATool_PickAxe>((*ToolIter)->ToolUnitPtr->ToolActorClass, ActorSpawnParameters);
		if (AxePtr)
		{
			CurrentEquipmentPtr = AxePtr;
			return true;
		}
	}
	break;
	}


	return false;
}

void UEquipmentElementComponent::CancelSkill_ActiveTool(const TSharedPtr < FCanbeActivedInfo>& CanbeActivedInfoSPtr)
{
	auto OnwerActorPtr = GetOwner<AHumanCharacter>();
	if (OnwerActorPtr)
	{
		if (CurrentEquipmentPtr)
		{
			CurrentEquipmentPtr->DoActionByCharacter(OnwerActorPtr, EEquipmentActionType::kStopAction);
		}
	}
}

bool UEquipmentElementComponent::ActiveSkill_ActiveTool(
	const TSharedPtr <FCanbeActivedInfo>& CanbeActivedInfoSPtr, bool bIsAutomaticStop /*= false */
)
{
	auto OnwerActorPtr = GetOwner<AHumanCharacter>();
	if (OnwerActorPtr)
	{
		if (CurrentEquipmentPtr)
		{
			CurrentEquipmentPtr->DoActionByCharacter(OnwerActorPtr, EEquipmentActionType::kStartAction);

			return true;
		}
	}
	return false;
}

void UEquipmentElementComponent::CancelSkill_WeaponActive(const TSharedPtr < FCanbeActivedInfo>& CanbeActivedInfoSPtr)
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

bool UEquipmentElementComponent::ActiveSkill_WeaponActive(
	const TSharedPtr <FCanbeActivedInfo>& CanbeActivedInfoSPtr, bool bIsAutomaticStop /*= false*/
)
{
	TSharedPtr < FWeaponSocketInfo > WeaponUnit = GetActivedWeapon();
	if (!WeaponUnit)
	{
		return false;
	}
	if (!WeaponUnit->WeaponUnitPtr)
	{
		return false;
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
	auto OnwerActorPtr = GetOwner<ACharacterBase>();
	if (!OnwerActorPtr)
	{
		return  false;
	}

	auto ASCPtr = OnwerActorPtr->GetAbilitySystemComponent();
	return ASCPtr->TriggerAbilityFromGameplayEvent(
		WeaponUnit->Handle,
		ASCPtr->AbilityActorInfo.Get(),
		FGameplayTag::EmptyTag,
		&Payload,
		*ASCPtr
	);
}

bool UEquipmentElementComponent::ActiveSkill_Active(
	const TSharedPtr <FCanbeActivedInfo>& CanbeActivedInfoSPtr, bool bIsAutomaticStop /*= false*/
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

void UEquipmentElementComponent::CancelSkill(const TSharedPtr<FCanbeActivedInfo>& CanbeActivedInfoSPtr)
{
	switch (CanbeActivedInfoSPtr->Type)
	{
	case FCanbeActivedInfo::EType::kWeaponActiveSkill:
	{
		CancelSkill_WeaponActive(CanbeActivedInfoSPtr);
	}
	break;
	case FCanbeActivedInfo::EType::kSwitchToTool:
	{
		CancelSkill_SwitchToTool(CanbeActivedInfoSPtr);
	}
	break;
	case FCanbeActivedInfo::EType::kActiveTool:
	{
		CancelSkill_ActiveTool(CanbeActivedInfoSPtr);
	}
	break;
	default:
		break;
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

void UEquipmentElementComponent::AddSendGroupEffectModify()
{
	struct GAEventModify_MultyTarget : public IGAEventModifySendInterface
	{
		GAEventModify_MultyTarget(int32 InPriority) :
			IGAEventModifySendInterface(InPriority)
		{
		}

		virtual void Modify(FGameplayAbilityTargetData_GASendEvent& GameplayAbilityTargetData_GAEvent)override
		{
			if (GameplayAbilityTargetData_GAEvent.DataAry.Num() > 1)
			{
				for (auto& Iter : GameplayAbilityTargetData_GAEvent.DataAry)
				{
					Iter.TrueDamage =
						Iter.TrueDamage / GameplayAbilityTargetData_GAEvent.DataAry.Num();

					Iter.BaseDamage =
						Iter.BaseDamage / GameplayAbilityTargetData_GAEvent.DataAry.Num();

					for (auto& ElementIter : Iter.ElementSet)
					{
						ElementIter.Get<2>() =
							ElementIter.Get<2>() / GameplayAbilityTargetData_GAEvent.DataAry.Num();
					}

					Iter.HP =
						Iter.HP / GameplayAbilityTargetData_GAEvent.DataAry.Num();
				}
			}
		}
	};
	AddSendEventModify(MakeShared<GAEventModify_MultyTarget>(9999));
}

void UEquipmentElementComponent::AddSendWuXingModify()
{
	struct GAEventModify_MultyTarget : public IGAEventModifySendInterface
	{
		GAEventModify_MultyTarget(int32 InPriority) :
			IGAEventModifySendInterface(InPriority)
		{
		}

		virtual void Modify(FGameplayAbilityTargetData_GASendEvent& GameplayAbilityTargetData_GAEvent)override
		{
			for (auto& Iter : GameplayAbilityTargetData_GAEvent.DataAry)
			{
				if (Iter.ElementSet.IsEmpty())
				{
					const auto& CharacterAttributes =
						GameplayAbilityTargetData_GAEvent.TriggerCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();

					std::map<int32, EWuXingType, std::greater<int>> ElementMap;
					ElementMap.emplace(CharacterAttributes.Element.GoldElement.GetCurrentValue(), EWuXingType::kGold);
					ElementMap.emplace(CharacterAttributes.Element.WoodElement.GetCurrentValue(), EWuXingType::kWood);
					ElementMap.emplace(CharacterAttributes.Element.WaterElement.GetCurrentValue(), EWuXingType::kWater);
					ElementMap.emplace(CharacterAttributes.Element.FireElement.GetCurrentValue(), EWuXingType::kFire);
					ElementMap.emplace(CharacterAttributes.Element.SoilElement.GetCurrentValue(), EWuXingType::kSoil);

					if (ElementMap.begin()->first > 0)
					{
						const auto Tuple = MakeTuple(
							ElementMap.begin()->second,
							ElementMap.begin()->first,
							Iter.BaseDamage
						);
						Iter.ElementSet.Add(Tuple);
					}
				}
			}
		}
	};
	AddSendEventModify(MakeShared<GAEventModify_MultyTarget>(9998));
}

void UEquipmentElementComponent::AddReceivedWuXingModify()
{
	struct GAEventModify_MultyTarget : public IGAEventModifyReceivedInterface
	{
		GAEventModify_MultyTarget(int32 InPriority) :
			IGAEventModifyReceivedInterface(InPriority)
		{
		}

		virtual void Modify(FGameplayAbilityTargetData_GAReceivedEvent& GameplayAbilityTargetData_GAEvent)override
		{
			const auto Caculation_Effective_Rate = [](int32 SelfLevel, int32 TargetLevel) {

				// 
				const auto Offset = (SelfLevel - TargetLevel) / 3;
				const auto Effective_Rate = 1.f + (Offset * 0.25f);
				return Effective_Rate;
				};

			auto& DataRef = GameplayAbilityTargetData_GAEvent.Data;
			if (DataRef.ElementSet.IsEmpty() && DataRef.TargetCharacterPtr.IsValid())
			{
				const auto& CharacterAttributes =
					DataRef.TargetCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();

				std::map<int32, EWuXingType, std::greater<int>> ElementMap;
				ElementMap.emplace(CharacterAttributes.Element.GoldElement.GetCurrentValue(), EWuXingType::kGold);
				ElementMap.emplace(CharacterAttributes.Element.WoodElement.GetCurrentValue(), EWuXingType::kWood);
				ElementMap.emplace(CharacterAttributes.Element.WaterElement.GetCurrentValue(), EWuXingType::kWater);
				ElementMap.emplace(CharacterAttributes.Element.FireElement.GetCurrentValue(), EWuXingType::kFire);
				ElementMap.emplace(CharacterAttributes.Element.SoilElement.GetCurrentValue(), EWuXingType::kSoil);

				const auto Effective_Rate = Caculation_Effective_Rate(ElementMap.begin()->first, 0);

				DataRef.BaseDamage = DataRef.BaseDamage * Effective_Rate;
			}
			else
			{
				const auto& CharacterAttributes =
					GameplayAbilityTargetData_GAEvent.TriggerCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();

				for (auto& ElementIter : DataRef.ElementSet)
				{
					// 木克土，土克水，水克火，火克金，金克木
					switch (ElementIter.Get<0>())
					{
					case EWuXingType::kGold:
					{
						const auto Effective_Rate = Caculation_Effective_Rate(CharacterAttributes.Element.FireElement.GetCurrentValue(), ElementIter.Get<1>());
						ElementIter.Get<2>() = ElementIter.Get<2>() * Effective_Rate;
					}
					break;
					case EWuXingType::kWood:
					{
						const auto Effective_Rate = Caculation_Effective_Rate(CharacterAttributes.Element.GoldElement.GetCurrentValue(), ElementIter.Get<1>());
						ElementIter.Get<2>() = ElementIter.Get<2>() * Effective_Rate;
					}
					break;
					case EWuXingType::kWater:
					{
						const auto Effective_Rate = Caculation_Effective_Rate(CharacterAttributes.Element.SoilElement.GetCurrentValue(), ElementIter.Get<1>());
						ElementIter.Get<2>() = ElementIter.Get<2>() * Effective_Rate;
					}
					break;
					case EWuXingType::kFire:
					{
						const auto Effective_Rate = Caculation_Effective_Rate(CharacterAttributes.Element.WaterElement.GetCurrentValue(), ElementIter.Get<1>());
						ElementIter.Get<2>() = ElementIter.Get<2>() * Effective_Rate;
					}
					break;
					case EWuXingType::kSoil:
					{
						const auto Effective_Rate = Caculation_Effective_Rate(CharacterAttributes.Element.WoodElement.GetCurrentValue(), ElementIter.Get<1>());
						ElementIter.Get<2>() = ElementIter.Get<2>() * Effective_Rate;
					}
					break;
					default:
						break;
					}
				}
			}
		}
	};
	AddReceviedEventModify(MakeShared<GAEventModify_MultyTarget>(9999));
}

void UEquipmentElementComponent::AddReceivedModify()
{
	struct GAEventModify_MultyTarget : public IGAEventModifyReceivedInterface
	{
		GAEventModify_MultyTarget(int32 InPriority) :
			IGAEventModifyReceivedInterface(InPriority)
		{
		}

		virtual void Modify(FGameplayAbilityTargetData_GAReceivedEvent& GameplayAbilityTargetData_GAEvent)override
		{
			if (GameplayAbilityTargetData_GAEvent.Data.TargetCharacterPtr.IsValid())
			{
				const auto& SelfCharacterAttributes =
					GameplayAbilityTargetData_GAEvent.Data.TargetCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();

				const auto& TargetCharacterAttributes =
					GameplayAbilityTargetData_GAEvent.TriggerCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();

				{
					const auto Rate = (TargetCharacterAttributes.HitRate.GetCurrentValue() - SelfCharacterAttributes.Evade.GetCurrentValue()) /
						static_cast<float>(TargetCharacterAttributes.HitRate.GetMaxValue());

					GameplayAbilityTargetData_GAEvent.Data.HitRate = FMath::FRand() <= Rate ? 100 : 0;
				}
			}
		}
	};
	AddReceviedEventModify(MakeShared<GAEventModify_MultyTarget>(9998));
}
