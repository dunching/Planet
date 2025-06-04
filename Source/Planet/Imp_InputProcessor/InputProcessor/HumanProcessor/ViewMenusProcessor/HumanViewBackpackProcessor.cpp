
#include "HumanViewBackpackProcessor.h"

#include "GameFramework/PlayerController.h"

#include "CreateMenu.h"
#include "InputProcessorSubSystemBase.h"
#include "BuildingBaseProcessor.h"
#include "HumanViewAlloctionSkillsProcessor.h"
#include "InputProcessorSubSystem_Imp.h"
#include "HumanCharacter_Player.h"

namespace HumanProcessor
{
	FViewBackpackProcessor::FViewBackpackProcessor(FOwnerPawnType* CharacterPtr) :
		Super(CharacterPtr)
	{
	}

	void FViewBackpackProcessor::EnterAction()
	{
		Super::EnterAction();
	}

	void FViewBackpackProcessor::QuitAction()
	{
		Super::QuitAction();
	}

	void FViewBackpackProcessor::CheckInteraction()
	{
	}

	void FViewBackpackProcessor::QuitCurrentState()
	{
		UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<FBuildingBaseProcessor>();
	}

}
