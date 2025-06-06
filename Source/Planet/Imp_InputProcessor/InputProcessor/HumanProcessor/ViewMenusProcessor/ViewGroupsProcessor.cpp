
#include "ViewGroupsProcessor.h"

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
#include "InputProcessorSubSystemBase.h"
#include "HumanRegularProcessor.h"
#include "HorseProcessor.h"
#include "HumanProcessor.h"
#include "InventoryComponent.h"
#include "BuildingBaseProcessor.h"
#include "HumanViewAlloctionSkillsProcessor.h"
#include "HumanCharacter_Player.h"
#include "InputProcessorSubSystem_Imp.h"

namespace HumanProcessor
{
	FViewGroupsProcessor::FViewGroupsProcessor(FOwnerPawnType* CharacterPtr) :
		Super(CharacterPtr)
	{
	}

	void FViewGroupsProcessor::EnterAction()
	{
		Super::EnterAction();

		auto HumanCharaterPtr = GetOwnerActor<FOwnerPawnType>();

		// UUIManagerSubSystem::GetInstance()->ViewTalentAllocation(true);
		UUIManagerSubSystem::GetInstance()->SwitchLayout(ELayoutCommon::kMenuLayout);
		UUIManagerSubSystem::GetInstance()->SwitchMenuLayout(EMenuType::kGroupManagger);
	}

	void FViewGroupsProcessor::QuitAction()
	{
		// UUIManagerSubSystem::GetInstance()->SwitchLayout(ELayoutCommon::kEmptyLayout);

		Super::QuitAction();
	}

	void FViewGroupsProcessor::CheckInteraction()
	{
	}

	void FViewGroupsProcessor::QuitCurrentState()
	{
		UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<FHumanRegularProcessor>();
	}

}
