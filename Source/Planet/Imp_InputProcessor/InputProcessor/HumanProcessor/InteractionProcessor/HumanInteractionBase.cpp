#include "HumanInteractionBase.h"

#include "GameFramework/PlayerController.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "CreateMenu.h"
#include "InputProcessorSubSystemBase.h"
#include "HumanRegularProcessor.h"
#include "HorseProcessor.h"
#include "HumanCharacter_AI.h"
#include "HumanCharacter_Player.h"
#include "InputProcessorSubSystem_Imp.h"
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

	bool FHumanInteractionBaseProcessor::InputKey(
		const FInputKeyEventArgs& EventArgs
		)
	{
		switch (EventArgs.Event)
		{
		case IE_Pressed:
			{
				auto GameOptionsPtr = UGameOptions::GetInstance();

				if (EventArgs.Key == GameOptionsPtr->StopInteraction)
				{
					StopInteraciton();
					Switch2RegularProcessor();
					return true;
				}
			}
			break;
		case IE_Released:
			{
			}
			break;
		}

		return FInputProcessor::InputKey(EventArgs);
	}

	void FHumanInteractionBaseProcessor::StopInteraciton()
	{
		if (SceneActorInteractionInterfacePtr)
		{
			SceneActorInteractionInterfacePtr->GetSceneActorInteractionComponent()->StopInteractionItem();
		}
	}

	void FHumanInteractionBaseProcessor::Switch2RegularProcessor()
	{
		UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<FHumanRegularProcessor>();
	}
}
