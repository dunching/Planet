#include "HumanInteractionWithChallengeEntry.h"

#include "GameFramework/PlayerController.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "CreateMenu.h"
#include "InputProcessorSubSystem.h"
#include "HumanRegularProcessor.h"
#include "HorseProcessor.h"
#include "HumanCharacter_AI.h"
#include "HumanCharacter_Player.h"
#include "InteractionList.h"
#include "MainHUD.h"
#include "MainHUDLayout.h"
#include "PlanetPlayerController.h"
#include "UIManagerSubSystem.h"

namespace HumanProcessor
{
	FHumanInteractionWithChallengeEntryProcessor::FHumanInteractionWithChallengeEntryProcessor(FOwnerPawnType* CharacterPtr) :
		Super(CharacterPtr)
	{

	}

	void FHumanInteractionWithChallengeEntryProcessor::EnterAction()
	{
		FInputProcessor::EnterAction();

		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
		if (OnwerActorPtr)
		{
			UUIManagerSubSystem::GetInstance()->SwitchLayout(ELayoutCommon::kOptionLayout);

			SwitchShowCursor(true);

			// Player对应的操作
			if (auto Character_NPCPtr = Cast<AHumanCharacter_AI>(OnwerActorPtr->LookAtSceneActorPtr))
			{
				// 显示对应的UI
				// OnwerActorPtr
				// 	->GetController<APlanetPlayerController>()
				// 	->GetHUD<AMainHUD>()
				// 	->GetMainHUDLayout()
				// 	->GetInteractionList()
				// 	->UpdateDisplay(Character_NPCPtr);
			}
		}
	}

	void FHumanInteractionWithChallengeEntryProcessor::QuitAction()
	{
		auto HumanCharaterPtr = GetOwnerActor<FOwnerPawnType>();

		// Player对应的操作
		if (auto Character_NPCPtr = Cast<AHumanCharacter_AI>(HumanCharaterPtr->LookAtSceneActorPtr))
		{
			// 显示对应的UI
			// HumanCharaterPtr
			// 	->GetController<APlanetPlayerController>()
			// 	->GetHUD<AMainHUD>()
			// 	->GetMainHUDLayout()
			// 	->GetInteractionList()
			// 	->CloseUI();
		}

		FInputProcessor::QuitAction();
	}

	void FHumanInteractionWithChallengeEntryProcessor::Switch2RegularProcessor()
	{
		UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<FHumanRegularProcessor>();
	}
}
