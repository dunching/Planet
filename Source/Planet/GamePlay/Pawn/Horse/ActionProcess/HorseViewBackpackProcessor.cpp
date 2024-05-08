
#include "HorseViewBackpackProcessor.h"

#include <Kismet/GameplayStatics.h>

#include "GenerateType.h"
#include "PlanetPlayerState.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "HumanCharacter.h"
#include "HorseCharacter.h"
#include "UI/UIManagerSubSystem.h"
#include "InteractionToAIMenu.h"
#include "BackpackMenu.h"
#include "CreateMenu.h"
#include "InputProcessorSubSystem.h"
#include "HumanRegularProcessor.h"

namespace HorseProcessor
{
	FHorseViewBackpackProcessor::FHorseViewBackpackProcessor(AHorseCharacter* CharacterPtr) :
		Super(CharacterPtr)
	{
	}

	FHorseViewBackpackProcessor::~FHorseViewBackpackProcessor()
	{
	}

	void FHorseViewBackpackProcessor::EnterAction()
	{
		Super::EnterAction();

		auto HumanCharaterPtr = GetOwnerActor();
		auto PlayerPCPtr = HumanCharaterPtr->GetController<APlayerController>();
		if (PlayerPCPtr)
		{
			PlayerPCPtr->bShowMouseCursor = true;

			UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PlayerPCPtr);
		}

		UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetOwnerActor());
		if (!GameInstance)
		{
			return;
		}
		auto UIManagerPtr = UUIManagerSubSystem::GetInstance();
		if (!UIManagerPtr)
		{
			return;
		}
	}

	void FHorseViewBackpackProcessor::QuitAction()
	{
		auto HumanCharaterPtr = GetOwnerActor();
		auto PlayerPCPtr = HumanCharaterPtr->GetController<APlayerController>();
		if (PlayerPCPtr)
		{
			PlayerPCPtr->bShowMouseCursor = false;

			UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerPCPtr);
		}

		auto PlayerCharacterPtr = Cast<AHorseCharacter>(HumanCharaterPtr);

		UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetOwnerActor());
		if (!GameInstance)
		{
			return;
		}
		auto UIManagerPtr = GameInstance->GetSubsystem<UUIManagerSubSystem>();
		if (UIManagerPtr)
		{
		}

		Super::QuitAction();
	}

	void FHorseViewBackpackProcessor::BKeyPressed()
	{
		ESCKeyPressed();
	}

	void FHorseViewBackpackProcessor::EKeyPressed()
	{

	}

	void FHorseViewBackpackProcessor::ESCKeyPressed()
	{
	//	UInputProcessorSubSystem::GetInstance()->SwitchActionProcess<FRegularProcessor>();
	}

	void FHorseViewBackpackProcessor::CheckInteraction()
	{
	}
}