
#include "HumanViewRaffleMenu.h"

#include <Kismet/GameplayStatics.h>

#include "GenerateType.h"
#include "PlanetPlayerState.h"
#include "CharacterBase.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "HumanCharacter.h"
#include "UI/UIManagerSubSystem.h"
#include "InteractionToAIMenu.h"
#include "BackpackMenu.h"
#include "CreateMenu.h"
#include "InputProcessorSubSystem.h"
#include "HumanRegularProcessor.h"
#include "HorseProcessor.h"
#include "HumanProcessor.h"
#include "HoldingItemsComponent.h"
#include "BuildingBaseProcessor.h"
#include "HumanViewAlloctionSkillsProcessor.h"
#include "RaffleSubSystem.h"

namespace HumanProcessor
{
	FHumanViewRaffleMenuProcessor::FHumanViewRaffleMenuProcessor(FOwnerPawnType* CharacterPtr) :
		Super(CharacterPtr)
	{
	}

	void FHumanViewRaffleMenuProcessor::EnterAction()
	{
		Super::EnterAction();

		auto HumanCharaterPtr = GetOwnerActor<FOwnerPawnType>();

		UUIManagerSubSystem::GetInstance()->ViewRaffleMenu(true);

		auto PlayerPCPtr = HumanCharaterPtr->GetController<APlayerController>();
		if (PlayerPCPtr)
		{
			PlayerPCPtr->bShowMouseCursor = true;

			UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PlayerPCPtr);
		}
	}

	void FHumanViewRaffleMenuProcessor::QuitAction()
	{
		UUIManagerSubSystem::GetInstance()->ViewRaffleMenu(false);
		
		URaffleSubSystem::GetInstance()->SyncUnits2Player();

		auto HumanCharaterPtr = GetOwnerActor<FOwnerPawnType>();

		auto PlayerPCPtr = HumanCharaterPtr->GetController<APlayerController>();
		if (PlayerPCPtr)
		{
			PlayerPCPtr->bShowMouseCursor = false;

			UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerPCPtr);
		}

		Super::QuitAction();
	}

	void FHumanViewRaffleMenuProcessor::F10KeyPressed()
	{
		QuitCurrentState();
	}

	void FHumanViewRaffleMenuProcessor::VKeyPressed()
	{
		UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<FHumanViewAlloctionSkillsProcessor>();
	}

	void FHumanViewRaffleMenuProcessor::BKeyPressed()
	{
	}

	void FHumanViewRaffleMenuProcessor::ESCKeyPressed()
	{
		QuitCurrentState();
	}

	void FHumanViewRaffleMenuProcessor::PressedNumKey(int32 NumKey)
	{

	}

	void FHumanViewRaffleMenuProcessor::CheckInteraction()
	{
	}

	void FHumanViewRaffleMenuProcessor::QuitCurrentState()
	{
		Switch2RegularProcessor();
	}

}