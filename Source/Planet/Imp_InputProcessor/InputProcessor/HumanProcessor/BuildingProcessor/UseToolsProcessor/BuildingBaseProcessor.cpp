
#include "BuildingBaseProcessor.h"

#include "DrawDebugHelpers.h"
#include "Async/Async.h"
#include "EnhancedInputSubsystems.h"
#include <GameFramework/PlayerController.h>

#include "HumanCharacter_Player.h"

#include "InputProcessorSubSystemBase.h"
#include "HumanProcessor.h"

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