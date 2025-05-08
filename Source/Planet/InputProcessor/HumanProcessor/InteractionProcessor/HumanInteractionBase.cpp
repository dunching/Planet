#include "HumanInteractionBase.h"

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
	FHumanInteractionBaseProcessor::FHumanInteractionBaseProcessor(
		FOwnerPawnType* CharacterPtr
		):
		 Super(CharacterPtr)
	{
	}
}
