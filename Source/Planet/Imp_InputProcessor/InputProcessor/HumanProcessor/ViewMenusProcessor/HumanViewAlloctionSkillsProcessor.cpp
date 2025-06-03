
#include "HumanViewAlloctionSkillsProcessor.h"

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
#include "CharacterAttributesComponent.h"
#include "HumanCharacter_Player.h"
#include "MainHUD.h"
#include "MainHUDLayout.h"
#include "MainMenuLayout.h"
#include "PlanetPlayerController.h"

namespace HumanProcessor
{
	FHumanViewAlloctionSkillsProcessor::FHumanViewAlloctionSkillsProcessor(FOwnerPawnType* CharacterPtr) :
		Super(CharacterPtr)
	{
	}

	void FHumanViewAlloctionSkillsProcessor::EnterAction()
	{
		Super::EnterAction();
		
		auto HumanCharaterPtr = GetOwnerActor<FOwnerPawnType>();

		// UUIManagerSubSystem::GetInstance()->ViewBackpack(true);
		UUIManagerSubSystem::GetInstance()->SwitchLayout(ELayoutCommon::kMenuLayout);
		UUIManagerSubSystem::GetInstance()->SwitchMenuLayout(EMenuType::kAllocationSkill);
	}

	void FHumanViewAlloctionSkillsProcessor::QuitAction()
	{
		// UUIManagerSubSystem::GetInstance()->ViewBackpack(false);

		auto HumanCharaterPtr = GetOwnerActor<FOwnerPawnType>();

		//
		UUIManagerSubSystem::GetInstance()->SwitchLayout(ELayoutCommon::kEmptyLayout);
		
		Super::QuitAction();
	}

	void FHumanViewAlloctionSkillsProcessor::CheckInteraction()
	{
	}

	void FHumanViewAlloctionSkillsProcessor::QuitCurrentState()
	{
		UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<FHumanRegularProcessor>();
	}

}
