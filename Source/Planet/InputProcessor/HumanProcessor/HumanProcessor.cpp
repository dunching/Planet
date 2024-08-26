
#include "HumanProcessor.h"

#include "DrawDebugHelpers.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Camera/CameraComponent.h"
#include "AIController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include <Kismet/GameplayStatics.h>
#include <Subsystems/SubsystemBlueprintLibrary.h>
#include "Async/Async.h"
#include "Components/CapsuleComponent.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "AbilitySystemBlueprintLibrary.h"

#include "LogHelper/LogWriter.h"
#include "ThreadPoolHelper/ThreadLibrary.h"
#include "ToolsLibrary.h"
#include "Animation/AnimInstanceBase.h"
#include "GenerateType.h"
#include "PlanetPlayerState.h"
#include "CharacterBase.h"
#include "ArticleBase.h"
#include "HumanCharacter.h"
#include "PlacingBuildingAreaProcessor.h"
#include "ActionTrackVehiclePlace.h"
#include "PlacingWallProcessor.h"
#include "PlacingGroundProcessor.h"
#include "InteractiveSkillComponent.h"
#include "ToolsMenu.h"
#include <BackpackMenu.h>
#include "UIManagerSubSystem.h"
#include <Character/GravityMovementComponent.h>
#include <DestroyProgress.h>
#include "InputProcessorSubSystem.h"
#include "HumanViewBackpackProcessor.h"
#include "HorseProcessor.h"
#include "HumanProcessor.h"
#include "Planet.h"
#include "PlanetGameInstance.h"
#include "HorseCharacter.h"
#include "PlanetGameplayAbility.h"
#include "BasicFutures_Dash.h"
#include "AssetRefMap.h"
#include "BuildingBaseProcessor.h"
#include "CharacterAttributesComponent.h"
#include "BuildSharedData.h"
#include "BuildingBase.h"
#include "CollisionDataStruct.h"
#include "CharacterAttibutes.h"
#include "GameplayTagsSubSystem.h"
#include "InteractiveBaseGAComponent.h"
#include "HumanRegularProcessor.h"

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

	void FHumanProcessor::MoveForward(const FInputActionValue& InputActionValue)
	{
		const auto Value = InputActionValue.Get<float>();

		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();

		if (OnwerActorPtr->Controller != nullptr)
		{
			const FRotator Rotation = OnwerActorPtr->Controller->GetControlRotation();

			const FVector ForwardDirection = 
				UKismetMathLibrary::MakeRotFromZX(-OnwerActorPtr->GetGravityDirection(), Rotation.Quaternion().GetForwardVector()).Vector();

#ifdef WITH_EDITOR
			if (DrawDebugHumanProcessor.GetValueOnGameThread())
			{
				DrawDebugLine(GetWorldImp(), OnwerActorPtr->GetActorLocation(), OnwerActorPtr->GetActorLocation() + (100 * ForwardDirection), FColor::Red, false, 3);
			}
#endif
			OnwerActorPtr->AddMovementInput(ForwardDirection, Value);
		}
	}

	void FHumanProcessor::MoveRight(const FInputActionValue& InputActionValue)
	{
		const auto Value = InputActionValue.Get<float>();

		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();

		if (OnwerActorPtr->Controller != nullptr)
		{
			const FRotator Rotation = OnwerActorPtr->Controller->GetControlRotation();

			const FVector RightDirection = Rotation.Quaternion().GetRightVector();

			OnwerActorPtr->AddMovementInput(RightDirection, Value);
		}
	}

	void FHumanProcessor::AddPitchInput(const FInputActionValue& InputActionValue)
	{
		const auto Value = InputActionValue.Get<float>();
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();

		OnwerActorPtr->AddControllerPitchInput(Value);
	}

	void FHumanProcessor::AddYawInput(const FInputActionValue& InputActionValue)
	{
		const auto Value = InputActionValue.Get<float>();
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
		OnwerActorPtr->AddControllerYawInput(Value);
	}

	void FHumanProcessor::SwitchWalkingOrRunning()
	{
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();

		if (OnwerActorPtr)
		{
			if (OnwerActorPtr->GetAbilitySystemComponent()->K2_HasMatchingGameplayTag(UGameplayTagsSubSystem::GetInstance()->Running))
			{
				OnwerActorPtr->GetInteractiveBaseGAComponent()->SwitchWalkState(false);
			}
			else
			{
				OnwerActorPtr->GetInteractiveBaseGAComponent()->SwitchWalkState(true);
			}
		}
	}

	void FHumanProcessor::LCtrlKeyPressed()
	{
		SwitchWalkingOrRunning();
	}

	void FHumanProcessor::LShiftKeyPressed()
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

			OnwerActorPtr->GetInteractiveBaseGAComponent()->Dash(DashDirection);
		}
	}

	void FHumanProcessor::LShiftKeyReleased()
	{
	}

	void FHumanProcessor::SpaceKeyPressed()
	{
		auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();

		if (OnwerActorPtr)
		{
			OnwerActorPtr->GetAbilitySystemComponent()->TryActivateAbilitiesByTag(
				FGameplayTagContainer{ UGameplayTagsSubSystem::GetInstance()->Jump }
			);
		}
	}

	void FHumanProcessor::SpaceKeyReleased()
	{
	}
}
