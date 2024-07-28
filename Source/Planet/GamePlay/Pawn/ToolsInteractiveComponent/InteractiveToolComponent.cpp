
#include "InteractiveToolComponent.h"

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

FName UInteractiveToolComponent::ComponentName = TEXT("InteractiveToolComponent");

void UInteractiveToolComponent::GenerationCanbeActiveEvent()
{
	CanbeActiveToolsAry.Empty();

	// 激活对应的工具
	for (const auto& Iter : ToolsMap)
	{
		if (Iter.Value->UnitPtr)
		{
			TSharedPtr<FCanbeActivedInfo > CanbeActivedInfoSPtr = MakeShared<FCanbeActivedInfo>();

			CanbeActivedInfoSPtr->Type = FCanbeActivedInfo::EType::kSwitchToTool;
			CanbeActivedInfoSPtr->Key = Iter.Value->Key;
			CanbeActivedInfoSPtr->Socket = Iter.Value->SkillSocket;

			CanbeActiveToolsAry.Add(CanbeActivedInfoSPtr);
		}
	}

	// “使用”一次这个工具
	{
		TSharedPtr<FCanbeActivedInfo > CanbeActivedInfoSPtr = MakeShared<FCanbeActivedInfo>();
		CanbeActivedInfoSPtr->Type = FCanbeActivedInfo::EType::kActiveTool;
		CanbeActivedInfoSPtr->Key = EKeys::LeftMouseButton;

		CanbeActiveToolsAry.Add(CanbeActivedInfoSPtr);
	}
}

void UInteractiveToolComponent::CancelSkill_SwitchToTool(const TSharedPtr<FCanbeActivedInfo>& CanbeActivedInfoSPtr)
{

}

bool UInteractiveToolComponent::ActiveSkill_SwitchToTool(
	const TSharedPtr<FCanbeActivedInfo>& CanbeActivedInfoSPtr, bool bIsAutomaticStop /*= false*/
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
	switch ((*ToolIter)->UnitPtr->GetSceneElementType<EToolUnitType>())
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
		auto AxePtr = GetWorld()->SpawnActor<ATool_PickAxe>((*ToolIter)->UnitPtr->ToolActorClass, ActorSpawnParameters);
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

void UInteractiveToolComponent::CancelSkill_ActiveTool(const TSharedPtr<FCanbeActivedInfo>& CanbeActivedInfoSPtr)
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

bool UInteractiveToolComponent::ActiveSkill_ActiveTool(
	const TSharedPtr<FCanbeActivedInfo>& CanbeActivedInfoSPtr, bool bIsAutomaticStop /*= false */
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

const TMap<FGameplayTag, TSharedPtr<FToolsSocketInfo>>& UInteractiveToolComponent::GetTools() const
{
	return ToolsMap;
}

ATool_Base* UInteractiveToolComponent::GetCurrentTool() const
{
	return CurrentEquipmentPtr;
}

TArray<TSharedPtr<FCanbeActivedInfo>> UInteractiveToolComponent::GetCanbeActiveAction() const
{
	return CanbeActiveToolsAry;
}

bool UInteractiveToolComponent::ActiveAction(
	const TSharedPtr<FCanbeActivedInfo>& CanbeActivedInfoSPtr, bool bIsAutomaticStop /*= false */
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
	default:
		break;
	}
	return false;
}

void UInteractiveToolComponent::CancelAction(const TSharedPtr<FCanbeActivedInfo>& CanbeActivedInfoSPtr)
{
	switch (CanbeActivedInfoSPtr->Type)
	{
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

void UInteractiveToolComponent::RetractputTool()
{
	if (CurrentEquipmentPtr)
	{
		CurrentEquipmentPtr->Destroy();
		CurrentEquipmentPtr = nullptr;
	}

	auto OnwerActorPtr = GetOwner<ACharacterBase>();
	if (OnwerActorPtr)
	{
		OnwerActorPtr->SwitchAnimLink(EAnimLinkClassType::kUnarmed);
	}

	PreviousTool = FGameplayTag::EmptyTag;
}

void UInteractiveToolComponent::RegisterTool(
	const TMap <FGameplayTag, TSharedPtr<FToolsSocketInfo>>& InToolInfoMap, bool bIsGenerationEvent
)
{
	ToolsMap = InToolInfoMap;

	if (bIsGenerationEvent)
	{
		GenerationCanbeActiveEvent();
	}
}

TSharedPtr<FToolsSocketInfo> UInteractiveToolComponent::FindTool(const FGameplayTag& Tag)
{
	auto Iter = ToolsMap.Find(Tag);
	if (Iter)
	{
		return *Iter;
	}

	return nullptr;
}
