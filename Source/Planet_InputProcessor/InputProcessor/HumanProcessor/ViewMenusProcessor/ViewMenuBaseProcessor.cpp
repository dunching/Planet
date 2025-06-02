
#include "ViewMenuBaseProcessor.h"

#include <Kismet/GameplayStatics.h>
#include "CharacterBase.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "GenerateTypes.h"
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

	bool FViewMenuBaseProcessor::InputKey(
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
				
				if (EventArgs.Key == GameOptionsPtr->EntryActionProcessor)
				{
					Switch2RegularProcessor();
					return true;
				}
				
			}
			break;
		case IE_Released:
			{
				auto GameOptionsPtr = UGameOptions::GetInstance();

				// 这个事件只有Rlease？
				if (EventArgs.Key == GameOptionsPtr->Return)
				{
					Switch2RegularProcessor();
					return true;
				}
				
			}
			break;
		}

		return Super::InputKey(EventArgs);
	}

	void FViewMenuBaseProcessor::Switch2RegularProcessor()
	{
		UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<FHumanRegularProcessor>();
	}

}