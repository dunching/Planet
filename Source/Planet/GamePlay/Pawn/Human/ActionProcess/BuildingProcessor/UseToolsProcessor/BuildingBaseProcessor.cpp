
#include "BuildingBaseProcessor.h"

#include "GameFramework/SpringArmComponent.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Async/Async.h"
#include "EnhancedInputSubsystems.h"
#include <Blueprint/WidgetBlueprintLibrary.h>
#include <GameFramework/PlayerController.h>

#include "Character/GravityMovementComponent.h"
//#include "VoxelSceneActor.h"

#include "HumanCharacter.h"

#include "GameplayAbilitySpec.h"
#include "AbilitySystemComponent.h"

#include "Animation/AnimInstanceBase.h"
#include "CacheAssetManager.h"
#include "ThreadSycAction.h"
#include "LogHelper/LogWriter.h"
#include "UIManagerSubSystem.h"
#include "ToolsMenu.h"
#include "ThreadPoolHelper/ThreadLibrary.h"
#include "InputProcessorSubSystem.h"
#include "HumanRegularProcessor.h"
#include "HorseProcessor.h"
#include "HumanProcessor.h"
#include "HumanPlayerController.h"
#include "ToolsIcon.h"
#include "Tool_PickAxe.h"
#include "PlanetGameplayAbility.h"
#include "HumanViewBackpackProcessor.h"
#include "BuildSharedData.h"
#include "BuildingBase.h"
#include "CollisionDataStruct.h"
#include "HumanViewAlloctionSkillsProcessor.h"
#include "Tool_Base.h"

namespace HumanProcessor
{
	FBuildingBaseProcessor::FBuildingBaseProcessor(FOwnerPawnType* CharacterPtr) :
		Super(CharacterPtr)
	{
	}

	void FBuildingBaseProcessor::EnterAction()
	{
		Super::EnterAction();

		UUIManagerSubSystem::GetInstance()->DisplayActionStateHUD(false);
		UUIManagerSubSystem::GetInstance()->DisplayBuildingStateHUD(true);
		UUIManagerSubSystem::GetInstance()->ViewBackpack(false);
		UUIManagerSubSystem::GetInstance()->ViewSkills(false);

		AddOrRemoveUseMenuItemEvent(true);
	}

	void FBuildingBaseProcessor::QuitAction()
	{
		AddOrRemoveUseMenuItemEvent(false);

		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
		if (OnwerActorPtr)
		{
			OnwerActorPtr->GetEquipmentItemsComponent()->RetractputTool();
		}

		UUIManagerSubSystem::GetInstance()->DisplayBuildingStateHUD(false);

		Super::QuitAction();
	}

	void FBuildingBaseProcessor::InputKey(const FInputKeyParams& Params)
	{
		if (Params.Event == EInputEvent::IE_Pressed)
		{
			auto SkillIter = HandleKeysMap.Find(Params.Key);
			if (SkillIter)
			{
				auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
				if (OnwerActorPtr)
				{
					OnwerActorPtr->GetEquipmentItemsComponent()->ActiveSkill(*SkillIter);
				}
			}
		}
		else
		{
			auto SkillIter = HandleKeysMap.Find(Params.Key);
			if (SkillIter)
			{
				auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
				OnwerActorPtr->GetEquipmentItemsComponent()->CancelSkill(*SkillIter);
			}
		}
	}

	void FBuildingBaseProcessor::QKeyPressed()
	{
		auto PCPtr = Cast<AHumanPlayerController>(GetOwnerActor()->GetController());

		if (PCPtr->PlayerInput->IsPressed(EKeys::LeftAlt))
		{
			UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<HumanProcessor::FHumanRegularProcessor>();
		}
	}

	void FBuildingBaseProcessor::VKeyPressed()
	{
		UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<FHumanViewAlloctionSkillsProcessor>();
	}

	void FBuildingBaseProcessor::BKeyPressed()
	{
		UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<FViewBackpackProcessor>();
	}

	void FBuildingBaseProcessor::MouseLeftPressed()
	{

	}

	void FBuildingBaseProcessor::MouseLeftReleased()
	{
	}

	void FBuildingBaseProcessor::MouseRightPressed()
	{

	}

	void FBuildingBaseProcessor::LAltKeyReleased()
	{
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
		if (!OnwerActorPtr)
		{
			return;
		}

		auto PlayerPCPtr = OnwerActorPtr->GetController<APlayerController>();
		if (PlayerPCPtr)
		{
			if (PlayerPCPtr->bShowMouseCursor > 0)
			{
				PlayerPCPtr->bShowMouseCursor = 0;
				UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerPCPtr);
			}
			else
			{
				PlayerPCPtr->bShowMouseCursor = 1;
				UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PlayerPCPtr);
			}
		}
	}

	void FBuildingBaseProcessor::AddOrRemoveUseMenuItemEvent(bool bIsAdd)
	{
		HandleKeysMap.Empty();
		if (bIsAdd)
		{
			auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
			if (OnwerActorPtr)
			{
				auto CanbeActiveInfos = OnwerActorPtr->GetEquipmentItemsComponent()->GetCanbeActivedTools();
				for (const auto& Iter : CanbeActiveInfos)
				{
					HandleKeysMap.Add(Iter->Key, Iter);
				}
			}
		}
	}
}