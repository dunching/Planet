
#include "HumanProcessor.h"

#include "DrawDebugHelpers.h"
#include "Async/Async.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputSubsystems.h"

#include "ToolsLibrary.h"
#include "PlanetPlayerState.h"
#include "CharacterBase.h"
#include "HumanCharacter_Player.h"
#include "PlacingBuildingAreaProcessor.h"
#include "ActionTrackVehiclePlace.h"
#include <DestroyProgress.h>
#include "HorseProcessor.h"
#include "GameplayTagsLibrary.h"
#include "CharacterAbilitySystemComponent.h"
#include "GameOptions.h"

#ifdef WITH_EDITOR
static TAutoConsoleVariable<int32> DrawDebugHumanProcessor(
	TEXT("Skill.DrawDebug.HumanProcessor"),
	0,
	TEXT("")
	TEXT(" default: 0"));
#endif

namespace HumanProcessor
{
	FHumanProcessor::FHumanProcessor(FOwnerPawnType* CharacterPtr) :
		Super(CharacterPtr)
	{
	}

	void FHumanProcessor::EnterAction()
	{
		Super::EnterAction();
	}

	bool FHumanProcessor::InputKey(
		const FInputKeyEventArgs& EventArgs
	)
	{
		switch (EventArgs.Event)
		{
		case IE_Pressed:
			{
				if (EventArgs.Key == EKeys::LeftAlt)
				{
					SwitchShowCursor(true);
				}
				
				auto GameOptionsPtr = UGameOptions::GetInstance();
				
				if (EventArgs.Key == GameOptionsPtr->DashKey)
				{
					Dash();
					return true;
				}
				else if (EventArgs.Key == GameOptionsPtr->RunKey)
				{
					SwitchWalkingOrRunning();
					return true;
				}
			}
			break;
		case IE_Released:
			{
				if (EventArgs.Key == EKeys::LeftAlt)
				{
					SwitchShowCursor(false);
				}
			}
			break;
		}

		return FInputProcessor::InputKey(EventArgs);
	}

	void FHumanProcessor::SwitchWalkingOrRunning()
	{
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();

		if (OnwerActorPtr)
		{
			if (OnwerActorPtr->GetCharacterAbilitySystemComponent()->K2_HasMatchingGameplayTag(UGameplayTagsLibrary::State_Running))
			{
				OnwerActorPtr->GetCharacterAbilitySystemComponent()->SwitchWalkState(false);
			}
			else
			{
				OnwerActorPtr->GetCharacterAbilitySystemComponent()->SwitchWalkState(true);
			}
		}
	}

	void FHumanProcessor::Dash()
	{
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();

		if (OnwerActorPtr)
		{
			auto EnhancedInputLocalPlayerSubsystemPtr = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
				Cast<APlayerController>(OnwerActorPtr->GetController())->GetLocalPlayer()
			);

			EDashDirection DashDirection = EDashDirection::kForward;

			if (EnhancedInputLocalPlayerSubsystemPtr->GetPlayerInput()->IsPressed(EKeys::W))
			{
				DashDirection = EDashDirection::kForward;
			}
			else if (EnhancedInputLocalPlayerSubsystemPtr->GetPlayerInput()->IsPressed(EKeys::S))
			{
				DashDirection = EDashDirection::kBackward;
			}
			else if (EnhancedInputLocalPlayerSubsystemPtr->GetPlayerInput()->IsPressed(EKeys::A))
			{
				DashDirection = EDashDirection::kLeft;
			}
			else if (EnhancedInputLocalPlayerSubsystemPtr->GetPlayerInput()->IsPressed(EKeys::D))
			{
				DashDirection = EDashDirection::kRight;
			}
			else
			{
				DashDirection = EDashDirection::kForward;
			}

			OnwerActorPtr->GetCharacterAbilitySystemComponent()->Dash(DashDirection);
		}
	}
}
