
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

#include "HumanCharacter_Player.h"

#include "GameplayAbilitySpec.h"
#include "AbilitySystemComponent.h"

#include "Animation/AnimInstanceBase.h"


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

		// UUIManagerSubSystem::GetInstance()->DisplayBuildingLayout(true);

		AddOrRemoveUseMenuItemEvent(true);
	}

	void FBuildingBaseProcessor::QuitAction()
	{
		AddOrRemoveUseMenuItemEvent(false);

		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
		if (OnwerActorPtr)
		{
//			OnwerActorPtr->GetInteractiveToolComponent()->RetractputTool();
		}

		// UUIManagerSubSystem::GetInstance()->DisplayBuildingLayout(false);

		Super::QuitAction();
	}

	void FBuildingBaseProcessor::AddOrRemoveUseMenuItemEvent(bool bIsAdd)
	{
// 		HandleKeysMap.Empty();
// 		if (bIsAdd)
// 		{
// 			auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
// 			if (OnwerActorPtr)
// 			{
// // 				auto CanbeActiveInfos = OnwerActorPtr->GetInteractiveToolComponent()->GetCanbeActiveAction();
// // 				CanbeActiveInfos.Append(OnwerActorPtr->GetInteractiveConsumablesComponent()->GetCanbeActiveAction());
// // 				for (const auto& Iter : CanbeActiveInfos)
// // 				{
// // 					switch (Iter->Type)
// // 					{
// // 					case FCanbeInteractionInfo::EType::kNone:
// // 					{}
// // 					break;
// // 					default:
// // 					{
// // 						HandleKeysMap.Add(Iter->Key, Iter);
// // 					}
// // 					break;
// // 					}
// // 				}
// 			}
// 		}
	}
}