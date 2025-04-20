#include "PlayerComponent.h"

#include "EnhancedInputComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Subsystems/SubsystemBlueprintLibrary.h"

#include "CharacterBase.h"
#include "EnhancedInputSubsystems.h"
#include "GroupManagger.h"
#include "HumanAIController.h"
#include "HumanCharacter_Player.h"
#include "InputActions.h"

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
