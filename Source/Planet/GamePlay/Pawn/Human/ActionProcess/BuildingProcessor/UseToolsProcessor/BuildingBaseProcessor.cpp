
#include "BuildingBaseProcessor.h"

#include "GameFramework/SpringArmComponent.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Async/Async.h"
#include "EnhancedInputSubsystems.h"

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
#include "PlanetPlayerController.h"
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

		UUIManagerSubSystem::GetInstance()->DisplayBuildingStateHUD(false);

		if (CurrentEquipmentPtr)
		{
			CurrentEquipmentPtr->Destroy();
		}

		Super::QuitAction();
	}

	void FBuildingBaseProcessor::InputKey(const FInputKeyParams& Params)
	{
		if (Params.Event == EInputEvent::IE_Pressed)
		{
			auto Iter = HandleKeysMap.Find(Params.Key);
			if (Iter)
			{
				const auto ToolSPtr = Iter->ToolUnitPtr;

				if (PreviousID == ToolSPtr->GetID())
				{
					if (CurrentEquipmentPtr)
					{
						CurrentEquipmentPtr->Destroy();
					}

					UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<HumanProcessor::FHumanRegularProcessor>();

					return;
				}

				auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
				if (OnwerActorPtr)
				{
					OnwerActorPtr->SwitchAnimLink(EAnimLinkClassType::kPickAxe);

					FActorSpawnParameters ActorSpawnParameters;
					ActorSpawnParameters.Owner = OnwerActorPtr;
					auto AxePtr = GetWorldImp()->SpawnActor<ATool_Base>(ToolSPtr->ToolActorClass, ActorSpawnParameters);
					if (AxePtr)
					{
						CurrentEquipmentPtr = AxePtr;
						PreviousID = ToolSPtr->GetID();
					}
				}
			}
		}
	}

	void FBuildingBaseProcessor::QKeyPressed()
	{
		auto PCPtr = Cast<APlanetPlayerController>(GetOwnerActor()->GetController());

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
		if (CurrentEquipmentPtr)
		{
			auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
			if (OnwerActorPtr)
			{
				CurrentEquipmentPtr->DoActionByCharacter(OnwerActorPtr, EEquipmentActionType::kStartAction);
			}
		}
	}

	void FBuildingBaseProcessor::MouseLeftReleased()
	{
		if (CurrentEquipmentPtr)
		{
			auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
			if (OnwerActorPtr)
			{
				CurrentEquipmentPtr->DoActionByCharacter(OnwerActorPtr, EEquipmentActionType::kStopAction);
			}
		}
	}

	void FBuildingBaseProcessor::MouseRightPressed()
	{

	}

	void FBuildingBaseProcessor::AddOrRemoveUseMenuItemEvent(bool bIsAdd)
	{
		HandleKeysMap.Empty();
		if (bIsAdd)
		{
			auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
			if (OnwerActorPtr)
			{
				auto ActiveSkillsAry = OnwerActorPtr->GetEquipmentItemsComponent()->GetTools();
				for (const auto& Iter : ActiveSkillsAry)
				{
					HandleKeysMap.Add(Iter.Value.Key, Iter.Value);
				}
			}
		}
	}

}