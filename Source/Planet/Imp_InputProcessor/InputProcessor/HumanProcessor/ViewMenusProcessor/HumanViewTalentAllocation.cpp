
#include "HumanViewTalentAllocation.h"

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
	FHumanViewTalentAllocation::FHumanViewTalentAllocation(FOwnerPawnType* CharacterPtr) :
		Super(CharacterPtr)
	{
	}

	void FHumanViewTalentAllocation::EnterAction()
	{
		Super::EnterAction();

		auto HumanCharaterPtr = GetOwnerActor<FOwnerPawnType>();

		UUIManagerSubSystem::GetInstance()->SwitchLayout(ELayoutCommon::kMenuLayout);
		UUIManagerSubSystem::GetInstance()->SwitchMenuLayout(EMenuType::kAllocationTalent);
	}

	bool FHumanViewTalentAllocation::InputKey(
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
					// 这个菜单下不要处理这个事件，因为我们“可能”需要【右键】去减少【天赋点】分配
					return true;
				}
			}
			break;
		}

		return Super::InputKey(EventArgs);
	}

	void FHumanViewTalentAllocation::QuitAction()
	{
		Super::QuitAction();
	}
	
	void FHumanViewTalentAllocation::CheckInteraction()
	{
	}

	void FHumanViewTalentAllocation::QuitCurrentState()
	{
		UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<FHumanRegularProcessor>();
	}

}
