
#include "ViewMenuBaseProcessor.h"

#include <Kismet/GameplayStatics.h>
#include "CharacterBase.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "GenerateType.h"
#include "PlanetPlayerState.h"
#include "HumanCharacter.h"
#include "UIManagerSubSystem.h"
#include "BackpackMenu.h"
#include "CreateMenu.h"
#include "InputProcessorSubSystem.h"
#include "HumanRegularProcessor.h"
#include "HorseProcessor.h"
#include "HumanProcessor.h"
#include "InventoryComponent.h"
#include "BuildingBaseProcessor.h"
#include "HumanViewAlloctionSkillsProcessor.h"
#include "HumanCharacter_Player.h"

namespace HumanProcessor
{
	FViewMenuBaseProcessor::FViewMenuBaseProcessor(FOwnerPawnType* CharacterPtr) :
		Super(CharacterPtr)
	{

	}

	void FViewMenuBaseProcessor::EnterAction()
	{
		FInputProcessor::EnterAction();
		
		SwitchShowCursor(true);
	}

	void FViewMenuBaseProcessor::Switch2RegularProcessor()
	{
		UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<FHumanRegularProcessor>();
	}

}