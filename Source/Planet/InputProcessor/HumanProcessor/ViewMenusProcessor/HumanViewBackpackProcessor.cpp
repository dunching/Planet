
#include "HumanViewBackpackProcessor.h"

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

namespace HumanProcessor
{
	FViewBackpackProcessor::FViewBackpackProcessor(FOwnerPawnType* CharacterPtr) :
		Super(CharacterPtr)
	{
	}

	void FViewBackpackProcessor::EnterAction()
	{
		Super::EnterAction();

		auto HumanCharaterPtr = GetOwnerActor<FOwnerPawnType>();

		UUIManagerSubSystem::GetInstance()->DisplayBuildingStateHUD(true);
		UUIManagerSubSystem::GetInstance()->ViewBackpack(
			true
		);

		auto PlayerPCPtr = HumanCharaterPtr->GetController<APlayerController>();
		if (PlayerPCPtr)
		{
			PlayerPCPtr->bShowMouseCursor = true;

			UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PlayerPCPtr);
		}
	}

	void FViewBackpackProcessor::QuitAction()
	{
		UUIManagerSubSystem::GetInstance()->DisplayBuildingStateHUD(false);
		UUIManagerSubSystem::GetInstance()->ViewBackpack(false);

		auto HumanCharaterPtr = GetOwnerActor<FOwnerPawnType>();

		auto PlayerPCPtr = HumanCharaterPtr->GetController<APlayerController>();
		if (PlayerPCPtr)
		{
			PlayerPCPtr->bShowMouseCursor = false;

			UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerPCPtr);
		}

		Super::QuitAction();
	}

	void FViewBackpackProcessor::VKeyPressed()
	{
		UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<FHumanViewAlloctionSkillsProcessor>();
	}

	void FViewBackpackProcessor::BKeyPressed()
	{
	}

	void FViewBackpackProcessor::ESCKeyPressed()
	{
		Super::ESCKeyPressed();
	}

	void FViewBackpackProcessor::PressedNumKey(int32 NumKey)
	{

	}

	void FViewBackpackProcessor::CheckInteraction()
	{
	}

	void FViewBackpackProcessor::QuitCurrentState()
	{
		UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<FBuildingBaseProcessor>();
	}

}