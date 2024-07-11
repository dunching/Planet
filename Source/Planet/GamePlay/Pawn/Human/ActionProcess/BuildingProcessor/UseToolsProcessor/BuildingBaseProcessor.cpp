
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
#include "InteractiveToolComponent.h"
#include "InteractiveConsumablesComponent.h"

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
			OnwerActorPtr->GetInteractiveToolComponent()->RetractputTool();
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
					switch ((*SkillIter)->Type)
					{
					case FCanbeActivedInfo::EType::kSwitchToTool:
					case FCanbeActivedInfo::EType::kActiveTool:
					{
						OnwerActorPtr->GetInteractiveToolComponent()->ActiveAction(*SkillIter);
					}
					break;
					case FCanbeActivedInfo::EType::kConsumables:
					{
						OnwerActorPtr->GetInteractiveConsumablesComponent()->ActiveAction(*SkillIter);
					}
					break;
					default:
						break;
					}
				}
			}
		}
		else
		{
			auto SkillIter = HandleKeysMap.Find(Params.Key);
			if (SkillIter)
			{
				auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
				if (OnwerActorPtr)
				{
					switch ((*SkillIter)->Type)
					{
					case FCanbeActivedInfo::EType::kSwitchToTool:
					case FCanbeActivedInfo::EType::kActiveTool:
					{
						OnwerActorPtr->GetInteractiveToolComponent()->CancelAction(*SkillIter);
					}
					break;
					case FCanbeActivedInfo::EType::kConsumables:
					{
						OnwerActorPtr->GetInteractiveConsumablesComponent()->CancelAction(*SkillIter);
					}
					break;
					default:
						break;
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

	}

	void FBuildingBaseProcessor::MouseLeftReleased()
	{
	}

	void FBuildingBaseProcessor::MouseRightPressed()
	{

	}

	void FBuildingBaseProcessor::LAltKeyPressed()
	{
		bIsPressdLeftAlt = true;
	}

	void FBuildingBaseProcessor::LAltKeyReleased()
	{
		if (bIsPressdLeftAlt)
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
		bIsPressdLeftAlt = false;
	}

	void FBuildingBaseProcessor::AddOrRemoveUseMenuItemEvent(bool bIsAdd)
	{
		HandleKeysMap.Empty();
		if (bIsAdd)
		{
			auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
			if (OnwerActorPtr)
			{
				auto CanbeActiveInfos = OnwerActorPtr->GetInteractiveToolComponent()->GetCanbeActiveAction();
				CanbeActiveInfos.Append(OnwerActorPtr->GetInteractiveConsumablesComponent()->GetCanbeActiveAction());
				for (const auto& Iter : CanbeActiveInfos)
				{
					switch (Iter->Type)
					{
					case FCanbeActivedInfo::EType::kNone:
					{}
					break;
					default:
					{
						HandleKeysMap.Add(Iter->Key, Iter);
					}
					break;
					}
				}
			}
		}
	}
}