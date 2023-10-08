
#include "HumanActionPigInteractionUI.h"

#include <Kismet/GameplayStatics.h>

#include "GenerateType.h"
#include "PlayerState/ShiYuPlayerState.h"
#include "CharacterBase.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "HumanCharacter.h"
#include "UI/UIManagerSubSystem.h"
#include "UI/Menu/InteractionToAIMenu.h"
#include "UI/Menu/CreateMenu/CreateMenu.h"
#include <UI/Menu/Backpack/BackpackMenu.h>
#include "InputProcessorSubSystem.h"
#include "HumanRegularProcessor.h"
#include "HorseProcessor.h"
#include "HumanProcessor.h"

namespace HumanProcessor
{
	FHumanActionPigInteractionUI::FHumanActionPigInteractionUI(AHumanCharacter* CharacterPtr) :
		Super(CharacterPtr)
	{
	}

	FHumanActionPigInteractionUI::~FHumanActionPigInteractionUI()
	{
	}

	void FHumanActionPigInteractionUI::EnterAction()
	{
		auto HumanCharaterPtr = GetOwnerActor<AHumanCharacter>();
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
		auto UIManagerPtr = GameInstance->GetSubsystem<UUIManagerSubSystem>();
		if (!UIManagerPtr)
		{
			return;
		}

		auto BackpackPtr = UIManagerPtr->DisplayBackpackLeftMenuUI([&](UBackpackMenu* BackpackPtr) {
			if (BackpackPtr)
			{
				BackpackPtr->SetHoldItemProperty(HumanCharaterPtr->GetHoldItemComponent()->GetHoldItemProperty());
			}
			});
		if (BackpackPtr)
		{
			BackpackPtr->SetHoldItemProperty(HumanCharaterPtr->GetHoldItemComponent()->GetHoldItemProperty());
			HumanCharaterPtr->GetHoldItemComponent()->GetHoldItemProperty()->SetItemChangeCB(
				EOnItemChangeNotityObjType::kLeftBackpackUI,
				std::bind(
					&UBackpackMenu::ResetFiled, BackpackPtr
				)
			);
		}

		UIManagerPtr->DisplayCreateQueueMenuUI([&](UCreateMenu* UIPtr) {
			if (UIPtr)
			{
				UIPtr->SetHoldItemProperty(HumanCharaterPtr->GetHoldItemComponent()->GetHoldItemProperty());
			}
			});
	}

	void FHumanActionPigInteractionUI::QuitAction()
	{
		auto HumanCharaterPtr = GetOwnerActor<AHumanCharacter>();
		auto PlayerPCPtr = HumanCharaterPtr->GetController<APlayerController>();
		if (PlayerPCPtr)
		{
			PlayerPCPtr->bShowMouseCursor = false;

			UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerPCPtr);
		}

		auto PlayerCharacterPtr = Cast<AHumanCharacter>(HumanCharaterPtr);

		UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetOwnerActor());
		if (!GameInstance)
		{
			return;
		}
		auto UIManagerPtr = GameInstance->GetSubsystem<UUIManagerSubSystem>();
		if (UIManagerPtr)
		{
			UIManagerPtr->RemoveBackpackLeftMenuUI();
			UIManagerPtr->RemoveBackpackRightMenuUI();
			UIManagerPtr->RemoveCreateQueueMenuUI();
		}

		if (HumanCharaterPtr->GetWorld())
		{
			HumanCharaterPtr->GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
			HumanCharaterPtr->GetHoldItemComponent()->GetHoldItemProperty()->RemoveItemChangeCB(
				EOnItemChangeNotityObjType::kLeftBackpackUI
			);
		}
	}

	void FHumanActionPigInteractionUI::BKeyPressed()
	{
		ESCKeyPressed();
	}

	void FHumanActionPigInteractionUI::EKeyPressed()
	{

	}

	void FHumanActionPigInteractionUI::ESCKeyPressed()
	{
		UInputProcessorSubSystem::GetInstance()->SwitchActionProcess<FHumanRegularProcessor>();
	}

	void FHumanActionPigInteractionUI::CheckInteraction()
	{
	}
}