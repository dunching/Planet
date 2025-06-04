#include "HorseViewBackpackProcessor.h"

#include <Kismet/GameplayStatics.h>

#include "GenerateTypes.h"
#include "PlanetPlayerState.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "HumanCharacter.h"
#include "HorseCharacter.h"
#include "UI/UIManagerSubSystem.h"
#include "BackpackMenu.h"
#include "CreateMenu.h"
#include "InputProcessorSubSystemBase.h"
#include "HumanRegularProcessor.h"

namespace HorseProcessor
{
	FHorseViewBackpackProcessor::FHorseViewBackpackProcessor(
		AHorseCharacter* CharacterPtr
		) :
		  Super(CharacterPtr)
	{
	}

	FHorseViewBackpackProcessor::~FHorseViewBackpackProcessor()
	{
	}

	AHorseCharacter* FHorseViewBackpackProcessor::GetOwnerActor()
	{
		return Cast<AHorseCharacter>(OnwerPawnPtr);
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

	void FHorseViewBackpackProcessor::CheckInteraction()
	{
	}
}
