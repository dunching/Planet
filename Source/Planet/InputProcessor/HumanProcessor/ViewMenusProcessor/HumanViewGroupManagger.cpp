
#include "HumanViewGroupManagger.h"

#include <Kismet/GameplayStatics.h>

#include "GenerateType.h"
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
#include "HoldingItemsComponent.h"
#include "BuildingBaseProcessor.h"
#include "HumanViewAlloctionSkillsProcessor.h"
#include "HumanCharacter_Player.h"

namespace HumanProcessor
{
	FHumanViewGroupManagger::FHumanViewGroupManagger(FOwnerPawnType* CharacterPtr) :
		Super(CharacterPtr)
	{
	}

	void FHumanViewGroupManagger::EnterAction()
	{
		Super::EnterAction();

		auto HumanCharaterPtr = GetOwnerActor<FOwnerPawnType>();

		UUIManagerSubSystem::GetInstance()->ViewGroupMatesManagger(true);

		auto PlayerPCPtr = HumanCharaterPtr->GetController<APlayerController>();
		if (PlayerPCPtr)
		{
			PlayerPCPtr->bShowMouseCursor = true;

			UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PlayerPCPtr);
		}
	}

	void FHumanViewGroupManagger::QuitAction()
	{
		UUIManagerSubSystem::GetInstance()->ViewGroupMatesManagger(false);

		auto HumanCharaterPtr = GetOwnerActor<FOwnerPawnType>();

		auto PlayerPCPtr = HumanCharaterPtr->GetController<APlayerController>();
		if (PlayerPCPtr)
		{
			PlayerPCPtr->bShowMouseCursor = false;

			UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerPCPtr);
		}

		Super::QuitAction();
	}

	void FHumanViewGroupManagger::GKeyPressed()
	{
	}

	void FHumanViewGroupManagger::VKeyPressed()
	{
		UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<FHumanViewAlloctionSkillsProcessor>();
	}

	void FHumanViewGroupManagger::BKeyPressed()
	{
	}

	void FHumanViewGroupManagger::HKeyPressed()
	{
	}

	void FHumanViewGroupManagger::PressedNumKey(int32 NumKey)
	{

	}

	void FHumanViewGroupManagger::CheckInteraction()
	{
	}

	void FHumanViewGroupManagger::QuitCurrentState()
	{
		UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<FHumanRegularProcessor>();
	}

}