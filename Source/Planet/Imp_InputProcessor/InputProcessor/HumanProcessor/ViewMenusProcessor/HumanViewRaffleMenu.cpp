
#include "HumanViewRaffleMenu.h"

#include <Kismet/GameplayStatics.h>

#include "GenerateTypes.h"
#include "PlanetPlayerState.h"
#include "CharacterBase.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "HumanCharacter.h"
#include "UI/UIManagerSubSystem.h"
#include "BackpackMenu.h"
#include "CreateMenu.h"
#include "InputProcessorSubSystem.h"
#include "HumanRegularProcessor.h"
#include "HorseProcessor.h"
#include "HumanProcessor.h"
#include "InventoryComponent.h"
#include "BuildingBaseProcessor.h"
#include "HumanViewAlloctionSkillsProcessor.h"
#include "RaffleSubSystem.h"
#include "HumanCharacter_Player.h"

namespace HumanProcessor
{
	FHumanViewRaffleMenuProcessor::FHumanViewRaffleMenuProcessor(FOwnerPawnType* CharacterPtr) :
		Super(CharacterPtr)
	{
	}

	void FHumanViewRaffleMenuProcessor::EnterAction()
	{
		Super::EnterAction();
	}

	void FHumanViewRaffleMenuProcessor::QuitAction()
	{
		Super::QuitAction();
	}

	void FHumanViewRaffleMenuProcessor::CheckInteraction()
	{
	}

	void FHumanViewRaffleMenuProcessor::QuitCurrentState()
	{
		Switch2RegularProcessor();
	}

}