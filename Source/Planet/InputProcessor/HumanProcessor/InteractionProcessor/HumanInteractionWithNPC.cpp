#include "HumanInteractionWithNPC.h"

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

namespace HumanProcessor
{
	FHumanInteractionWithNPCProcessor::FHumanInteractionWithNPCProcessor(FOwnerPawnType* CharacterPtr) :
		Super(CharacterPtr)
	{

	}

	void FHumanInteractionWithNPCProcessor::EnterAction()
	{
		FInputProcessor::EnterAction();

		auto HumanCharaterPtr = GetOwnerActor<FOwnerPawnType>();

		auto PlayerPCPtr = HumanCharaterPtr->GetController<APlayerController>();
		if (PlayerPCPtr)
		{
			PlayerPCPtr->bShowMouseCursor = true;

			UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PlayerPCPtr);
		}

		// Player对应的操作
		if (auto Character_NPCPtr = Cast<AHumanCharacter_AI>(HumanCharaterPtr->LookAtSceneActorPtr))
		{
			// 显示对应的UI
			HumanCharaterPtr
				->GetController<APlanetPlayerController>()
				->GetHUD<AMainHUD>()
				->GetMainHUDLayout()
				->GetInteractionList()
				->UpdateDisplay(Character_NPCPtr);
		}
	}

	void FHumanInteractionWithNPCProcessor::QuitAction()
	{
		auto HumanCharaterPtr = GetOwnerActor<FOwnerPawnType>();

		auto PlayerPCPtr = HumanCharaterPtr->GetController<APlayerController>();
		if (PlayerPCPtr)
		{
			PlayerPCPtr->bShowMouseCursor = false;

			UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerPCPtr);
		}
	
		// Player对应的操作
		if (auto Character_NPCPtr = Cast<AHumanCharacter_AI>(HumanCharaterPtr->LookAtSceneActorPtr))
		{
			// 显示对应的UI
			HumanCharaterPtr
				->GetController<APlanetPlayerController>()
				->GetHUD<AMainHUD>()
				->GetMainHUDLayout()
				->GetInteractionList()
				->CloseUI();
		}

		FInputProcessor::QuitAction();
	}

	void FHumanInteractionWithNPCProcessor::ESCKeyPressed()
	{
		Switch2RegularProcessor();
	}

	void FHumanInteractionWithNPCProcessor::Switch2RegularProcessor()
	{
		UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<FHumanRegularProcessor>();
	}
}
