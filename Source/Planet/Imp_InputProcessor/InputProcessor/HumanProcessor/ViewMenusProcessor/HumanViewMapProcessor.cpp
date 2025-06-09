
#include "HumanViewMapProcessor.h"

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
	FHumanViewMapProcessor::FHumanViewMapProcessor(FOwnerPawnType* CharacterPtr) :
		Super(CharacterPtr)
	{
	}

	void FHumanViewMapProcessor::EnterAction()
	{
		Super::EnterAction();

		auto HumanCharaterPtr = GetOwnerActor<FOwnerPawnType>();

		UUIManagerSubSystem::GetInstance()->SwitchLayout(ELayoutCommon::kMenuLayout);
		UUIManagerSubSystem::GetInstance()->SwitchMenuLayout(EMenuType::kViewMap);
	}

	bool FHumanViewMapProcessor::InputKey(
		const FInputKeyEventArgs& EventArgs
		)
	{
		switch (EventArgs.Event)
		{
		case IE_Pressed:
			{
			}
			break;
		case IE_Released:
			{
				auto GameOptionsPtr = UGameOptions::GetInstance();

				if (EventArgs.Key == GameOptionsPtr->Return)
				{
					// 这个菜单下不要处理这个事件
					return true;
				}
			}
			break;
		}

		return Super::InputKey(EventArgs);
	}

	void FHumanViewMapProcessor::QuitAction()
	{
		Super::QuitAction();
	}
	
	void FHumanViewMapProcessor::CheckInteraction()
	{
	}

	void FHumanViewMapProcessor::QuitCurrentState()
	{
		UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<FHumanRegularProcessor>();
	}

}
