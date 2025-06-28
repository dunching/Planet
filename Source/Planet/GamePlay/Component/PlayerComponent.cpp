#include "PlayerComponent.h"

#include "EnhancedInputComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Subsystems/SubsystemBlueprintLibrary.h"
#include "GameFramework/GameplayCameraComponent.h"

#include "CharacterBase.h"
#include "EnhancedInputSubsystems.h"
#include "GroupManagger.h"
#include "QuestSubSystem.h"
#include "HumanAIController.h"
#include "HumanCharacter_Player.h"
#include "HumanRegularProcessor.h"
#include "InputActions.h"
#include "InputProcessorSubSystem_Imp.h"
#include "MainHUD.h"
#include "PlanetPlayerController.h"
#include "TeamMatesHelperComponent.h"
#include "PlanetPlayerState.h"

FName UPlayerComponent::ComponentName = TEXT("PlayerComponent");

UPlayerComponent::UPlayerComponent(
	const FObjectInitializer& ObjectInitializer
	):
	 Super(ObjectInitializer)
{
}

void UPlayerComponent::BeginPlay()
{
	Super::BeginPlay();

#if UE_EDITOR || UE_CLIENT
	if (GetOwnerRole() == ROLE_AutonomousProxy)
	{
		auto OnwerActorPtr = GetOwner<FOwnerType>();
		if (!OnwerActorPtr)
		{
			return;
		}
	}
#endif
}

bool UPlayerComponent::TeleportTo(
	const FVector& DestLocation,
	const FRotator& DestRotation,
	bool bIsATest,
	bool bNoCheck
	)
{
	auto OnwerActorPtr = GetOwner<FOwnerType>();
	if (!OnwerActorPtr)
	{
		return false;
	}

	// TODO 使用EQS查询位置
	OnwerActorPtr->GetGroupManagger()->GetTeamMatesHelperComponent()->TeleportTo(
		 DestLocation,
		 DestRotation,
		 bIsATest,
		 bNoCheck
		);

	return true;
}

void UPlayerComponent::PossessedBy(
	APlayerController* NewController
	)
{
#if UE_EDITOR || UE_CLIENT
	if (GetOwnerRole() == ROLE_AutonomousProxy)
	{
		if (auto Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(NewController->GetLocalPlayer()))
		{
			InputActionsPtr = NewObject<UInputActions>();
			InputActionsPtr->InitialInputMapping();

			Subsystem->AddMappingContext(
			                             InputActionsPtr->InputMappingContext,
			                             0
			                            );
		}
	}
#endif
}

void UPlayerComponent::SetupPlayerInputComponent(
	UInputComponent* PlayerInputComponent
	)
{
#if UE_EDITOR || UE_CLIENT
	if (GetOwnerRole() == ROLE_AutonomousProxy)
	{
		auto OnwerActorPtr = GetOwner<FOwnerType>();
		if (!OnwerActorPtr)
		{
			return;
		}

		// 根据设置绑定
		if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<
			UEnhancedInputComponent>(PlayerInputComponent))
		{
			EnhancedInputComponent->BindAction(
			                                   InputActionsPtr->MoveForwardActionPtr,
			                                   ETriggerEvent::Triggered,
			                                   this,
			                                   &ThisClass::MoveForward
			                                  );
			EnhancedInputComponent->BindAction(
			                                   InputActionsPtr->MoveRightActionPtr,
			                                   ETriggerEvent::Triggered,
			                                   this,
			                                   &ThisClass::MoveRight
			                                  );

			EnhancedInputComponent->BindAction(
			                                   InputActionsPtr->AddPitchActionPtr,
			                                   ETriggerEvent::Triggered,
			                                   this,
			                                   &ThisClass::AddPitchInput
			                                  );
			EnhancedInputComponent->BindAction(
			                                   InputActionsPtr->AddYawActionPtr,
			                                   ETriggerEvent::Triggered,
			                                   this,
			                                   &ThisClass::AddYawInput
			                                  );
		}
	}
#endif
}

ECameraType UPlayerComponent::GetCameraType() const
{
	return CameraType;
}

void UPlayerComponent::SetCameraType(
	ECameraType NewCameraType
	)
{
	CameraType = NewCameraType;
}

void UPlayerComponent::OnPlayerDataIsOk()
{
}

void UPlayerComponent::OnLocalPlayerDataIsOk()
{
#if UE_EDITOR || UE_CLIENT
	if (GetOwnerRole() == ROLE_AutonomousProxy)
	{
		auto OnwerActorPtr = GetOwner<FOwnerType>();
		if (!OnwerActorPtr)
		{
			return;
		}

		if (!OnwerActorPtr->GetGroupManagger())
		{
			return;
		}

		auto PCPtr = OnwerActorPtr->GetController<APlanetPlayerController>();
		if (!PCPtr)
		{
			return;
		}

		auto PSPtr = PCPtr->GetPlayerState<APlanetPlayerState>();
		if (!PSPtr)
		{
			return;
		}

		// 显示
		Cast<AMainHUD>(PCPtr->MyHUD)->InitalHUD();

		// 在SetPawn之后调用
		UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<HumanProcessor::FHumanRegularProcessor>(
			 [this, OnwerActorPtr](
			 auto NewProcessor
			 )
			 {
				 NewProcessor->SetPawn(Cast<FOwnerType>(OnwerActorPtr));
			 }
			);

		// 
		UQuestSubSystem::GetInstance()->InitializeMainThread();
		UQuestSubSystem::GetInstance()->ActiveMainThread();
	}
#endif
}

void UPlayerComponent::AddYawInput(
	const FInputActionValue& InputActionValue
	)
{
	const auto Value = InputActionValue.Get<float>();

	auto OnwerActorPtr = GetOwner<FOwnerType>();

	OnwerActorPtr->AddControllerYawInput(Value);
}

void UPlayerComponent::AddPitchInput(
	const FInputActionValue& InputActionValue
	)
{
	const auto Value = InputActionValue.Get<float>();

	auto OnwerActorPtr = GetOwner<FOwnerType>();

	OnwerActorPtr->AddControllerPitchInput(Value);
}

void UPlayerComponent::MoveRight(
	const FInputActionValue& InputActionValue
	)
{
	const auto Value = InputActionValue.Get<float>();

	auto OnwerActorPtr = GetOwner<FOwnerType>();

	if (OnwerActorPtr->Controller != nullptr)
	{
		const FRotator Rotation = OnwerActorPtr->Controller->GetControlRotation();

		const FVector RightDirection = Rotation.Quaternion().GetRightVector();

		OnwerActorPtr->AddMovementInput(RightDirection, Value);
	}
}

void UPlayerComponent::MoveForward(
	const FInputActionValue& InputActionValue
	)
{
	const auto Value = InputActionValue.Get<float>();

	auto OnwerActorPtr = GetOwner<FOwnerType>();

	if (OnwerActorPtr->Controller != nullptr)
	{
		const FRotator Rotation = OnwerActorPtr->Controller->GetControlRotation();

		const FVector ForwardDirection =
			UKismetMathLibrary::MakeRotFromZX(
			                                  -OnwerActorPtr->GetGravityDirection(),
			                                  Rotation.Quaternion().GetForwardVector()
			                                 ).Vector();

		OnwerActorPtr->AddMovementInput(ForwardDirection, Value);
	}
}
