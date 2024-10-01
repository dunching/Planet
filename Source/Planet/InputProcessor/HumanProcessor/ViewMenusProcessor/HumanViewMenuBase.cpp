
#include "HumanViewMenuBase.h"

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
#include "HoldingItemsComponent.h"
#include "BuildingBaseProcessor.h"
#include "HumanViewAlloctionSkillsProcessor.h"

namespace HumanProcessor
{
	FHumanViewMenuBase::FHumanViewMenuBase(FOwnerPawnType* CharacterPtr) :
		Super(CharacterPtr)
	{

	}

	void FHumanViewMenuBase::ESCKeyPressed()
	{
		Switch2RegularProcessor();
	}

	void FHumanViewMenuBase::Switch2RegularProcessor()
	{
		UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<FHumanRegularProcessor>();
	}

}