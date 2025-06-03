
#include "HumanTransactionProcessor.h"

#include "DrawDebugHelpers.h"
#include "Async/Async.h"
#include "Engine/LocalPlayer.h"

#include "ToolsLibrary.h"
#include "PlacingBuildingAreaProcessor.h"
#include "ActionTrackVehiclePlace.h"
#include "UIManagerSubSystem.h"
#include <DestroyProgress.h>

#include "GameOptions.h"
#include "InputProcessorSubSystem.h"
#include "HumanCharacter_Player.h"
#include "HumanRegularProcessor.h"

namespace HumanProcessor
{
	FHumanTransactionProcessor::FHumanTransactionProcessor(FOwnerPawnType* CharacterPtr) :
	                                                                                     Super(CharacterPtr)
	{
	}

	void FHumanTransactionProcessor::EnterAction()
	{
		Super::EnterAction();

		SwitchShowCursor(true);

		UUIManagerSubSystem::GetInstance()->SwitchLayout(ELayoutCommon::kTransactionLayout);
	}

	void FHumanTransactionProcessor::QuitAction()
	{
		// UUIManagerSubSystem::GetInstance()->DisplayEndangeredLayout(false);

		Super::QuitAction();
	}

	bool FHumanTransactionProcessor::InputKey(
		const FInputKeyEventArgs& EventArgs
		)
	{
		switch (EventArgs.Event)
		{
		case IE_Pressed:
			{
				auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
				if (!OnwerActorPtr)
				{
					return false;
				}

				auto GameOptionsPtr = UGameOptions::GetInstance();
				
				if (EventArgs.Key == GameOptionsPtr->Return)
				{
					UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<FHumanRegularProcessor>();
					return true;
				}
				
			}
			break;
		case IE_Released:
			{
			}
			break;
		}

		return Super::InputKey(EventArgs);
	}
}
