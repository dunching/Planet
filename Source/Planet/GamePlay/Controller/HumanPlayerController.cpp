
#include "HumanPlayerController.h"

#include "GameFramework/PlayerState.h"
#include "Interfaces/NetworkPredictionInterface.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PawnMovementComponent.h"
#include "EnhancedInputSubsystems.h"
#include <Engine/Engine.h>
#include <IXRTrackingSystem.h>
#include <IXRCamera.h>

#include "InputProcessorSubSystem.h"
#include "HorseCharacter.h"
#include "HumanCharacter.h"
#include "HumanRegularProcessor.h"
#include "HorseRegularProcessor.h"
#include "InputActions.h"
#include "UIManagerSubSystem.h"
#include "CharacterBase.h"
#include "HoldingItemsComponent.h"
#include "GroupMnaggerComponent.h"
#include "HumanAIController.h"
#include "SceneElement.h"
#include "HumanCharacter.h"
#include "HumanControllerInterface.h"
#include "NavgationSubSysetem.h"
#include "AssetRefMap.h"
#include "FocusIcon.h"
#include "TestCommand.h"
#include "GameplayTagsSubSystem.h"

AHumanPlayerController::AHumanPlayerController(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
}

void AHumanPlayerController::SetFocus(AActor* NewFocus, EAIFocusPriority::Type InPriority)
{
	ClearFocus(InPriority);

	if (NewFocus)
	{
		BindRemove(NewFocus);

		if (InPriority >= FocusInformation.Priorities.Num())
		{
			FocusInformation.Priorities.SetNum(InPriority + 1);
		}
		FocusInformation.Priorities[InPriority].Actor = NewFocus;

		auto AssetRefMapPtr = UAssetRefMap::GetInstance();
		FocusIconPtr = CreateWidget<UFocusIcon>(GetWorldImp(), AssetRefMapPtr->FocusIconClass);
		if (FocusIconPtr)
		{
			FocusIconPtr->FocusItem = FocusInformation.Priorities[InPriority];
			FocusIconPtr->AddToViewport();
		}
	}
}

AActor* AHumanPlayerController::GetFocusActor() const
{
	AActor* FocusActor = nullptr;
	for (int32 Index = FocusInformation.Priorities.Num() - 1; Index >= 0; --Index)
	{
		const FFocusKnowledge::FFocusItem& FocusItem = FocusInformation.Priorities[Index];
		FocusActor = FocusItem.Actor.Get();
		if (FocusActor)
		{
			break;
		}
		else if (FAISystem::IsValidLocation(FocusItem.Position))
		{
			break;
		}
	}

	return FocusActor;
}

void AHumanPlayerController::ClearFocus(EAIFocusPriority::Type InPriority)
{
	if (InPriority < FocusInformation.Priorities.Num())
	{
		if (FocusInformation.Priorities[InPriority].Actor.IsValid())
		{
			FocusInformation.Priorities[InPriority].Actor->OnEndPlay.RemoveDynamic(this, &ThisClass::OnFocusEndplay);
		}

		FocusInformation.Priorities[InPriority].Actor = nullptr;
		FocusInformation.Priorities[InPriority].Position = FAISystem::InvalidLocation;
	}

	if (FocusIconPtr)
	{
		FocusIconPtr->RemoveFromParent();
		FocusIconPtr = nullptr;
	}
}

FVector AHumanPlayerController::GetFocalPoint() const
{
	FVector Result = FAISystem::InvalidLocation;

	// find focus with highest priority
	for (int32 Index = FocusInformation.Priorities.Num() - 1; Index >= 0; --Index)
	{
		const FFocusKnowledge::FFocusItem& FocusItem = FocusInformation.Priorities[Index];
		AActor* FocusActor = FocusItem.Actor.Get();
		if (FocusActor)
		{
			Result = GetFocalPointOnActor(FocusActor);
			break;
		}
		else if (FAISystem::IsValidLocation(FocusItem.Position))
		{
			Result = FocusItem.Position;
			break;
		}
	}

	return Result;
}

FVector AHumanPlayerController::GetFocalPointOnActor(const AActor* Actor) const
{
	return Actor != nullptr ? Actor->GetActorLocation() : FAISystem::InvalidLocation;
}

void AHumanPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(
			UInputProcessorSubSystem::GetInstance()->InputActionsPtr->InputMappingContext,
			0
		);
	}

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);

	UNavgationSubSystem::GetInstance();
}

void AHumanPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
}

void AHumanPlayerController::UpdateRotation(float DeltaTime)
{
	APawn* const MyPawn = GetPawnOrSpectator();
	if (MyPawn)
	{
		FRotator ViewRotation = ControlRotationWithoutGravityTrans;

		const FVector FocalPoint = GetFocalPoint();
		if (FAISystem::IsValidLocation(FocalPoint))
		{
			const auto FocusRotation = (FocalPoint - MyPawn->GetPawnViewLocation()).ToOrientationQuat();
			FRotator CurrentControlRotation = GetControlRotation();

			const auto FoucusRotationInput = (MyPawn->GetGravityTransform().Inverse() * FocusRotation).Rotator();

			const auto DeltaRot = DeltaTime * 120.f;
			const float AngleTolerance = 1e-3f;

			// PITCH
			if (!FMath::IsNearlyEqual(ViewRotation.Pitch, FoucusRotationInput.Pitch, AngleTolerance))
			{
				ViewRotation.Pitch = FMath::FixedTurn(ViewRotation.Pitch, FoucusRotationInput.Pitch, DeltaRot);
			}

			// YAW
			if (!FMath::IsNearlyEqual(ViewRotation.Yaw, FoucusRotationInput.Yaw, AngleTolerance))
			{
				ViewRotation.Yaw = FMath::FixedTurn(ViewRotation.Yaw, FoucusRotationInput.Yaw, DeltaRot);
			}

			ViewRotation.Roll = 0.f;

			RotationInput = FRotator::ZeroRotator;
		}

		if (PlayerCameraManager)
		{
			// Calculate Delta to be applied on ViewRotation
			FRotator DeltaRot(RotationInput);
			PlayerCameraManager->ProcessViewRotation(DeltaTime, ViewRotation, DeltaRot);
		}

		AActor* ViewTarget = GetViewTarget();
		if (!PlayerCameraManager || !ViewTarget || !ViewTarget->HasActiveCameraComponent() || ViewTarget->HasActivePawnControlCameraComponent())
		{
			if (IsLocalPlayerController() && GEngine->XRSystem.IsValid() && GetWorld() != nullptr && GEngine->XRSystem->IsHeadTrackingAllowedForWorld(*GetWorld()))
			{
				auto XRCamera = GEngine->XRSystem->GetXRCamera();
				if (XRCamera.IsValid())
				{
					XRCamera->ApplyHMDRotation(this, ViewRotation);
				}
			}
		}

		ControlRotationWithoutGravityTrans = ViewRotation;

		ViewRotation = (MyPawn->GetGravityTransform() * ViewRotation.Quaternion()).Rotator();

		SetControlRotation(ViewRotation);

#if WITH_EDITOR
		RootComponent->SetWorldLocation(MyPawn->GetActorLocation());
#endif

		MyPawn->FaceRotation(ViewRotation, DeltaTime);
	}
}

void AHumanPlayerController::OnPossess(APawn* InPawn)
{
	bool bIsNewPawn = (InPawn && InPawn != GetPawn());

	Super::OnPossess(InPawn);

	if (bIsNewPawn)
	{
		if (InPawn && InPawn->IsA(AHumanCharacter::StaticClass()))
		{
#if TESTHOLDDATA
			TestCommand::AddPlayerCharacterTestDataImp(Cast<AHumanCharacter>(InPawn));
#endif

			auto GroupsHelperSPtr = GetGroupMnaggerComponent()->GetGroupHelper();
			if (GroupsHelperSPtr)
			{
				TeamMembersChangedDelegateHandle = GroupsHelperSPtr->MembersChanged.AddCallback(
					std::bind(&ThisClass::OnCharacterGroupMateChanged, this, std::placeholders::_1, std::placeholders::_2)
				);
			}

			UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<HumanProcessor::FHumanRegularProcessor>([this, InPawn](auto NewProcessor) {
				NewProcessor->SetPawn(Cast<AHumanCharacter>(InPawn));
				});
		}
	}
}

void AHumanPlayerController::OnUnPossess()
{
	APawn* CurrentPawn = GetPawn();

	auto CharacterPtr = Cast<FPawnType>(CurrentPawn);
	if (CharacterPtr)
	{
		if (TeamMembersChangedDelegateHandle)
		{
			TeamMembersChangedDelegateHandle->UnBindCallback();
		}
	}

	Super::OnUnPossess();
}

void AHumanPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
}

bool AHumanPlayerController::InputKey(const FInputKeyParams& Params)
{
	auto Result = Super::InputKey(Params);

	UInputProcessorSubSystem::GetInstance()->InputKey(Params);

	return Result;
}

UPlanetAbilitySystemComponent* AHumanPlayerController::GetAbilitySystemComponent() const
{
	return GetPawn<FPawnType>()->GetAbilitySystemComponent();
}

UGroupMnaggerComponent* AHumanPlayerController::GetGroupMnaggerComponent()const
{
	return GetPawn<FPawnType>()->GetGroupMnaggerComponent();
}

UGourpmateUnit* AHumanPlayerController::GetGourpMateUnit()
{
	return GetPawn<FPawnType>()->GetGourpMateUnit();
}

void AHumanPlayerController::InitialCharacter()
{
}

void AHumanPlayerController::OnCharacterGroupMateChanged(
	EGroupMateChangeType GroupMateChangeType,
	FPawnType* LeaderPCPtr
)
{
	switch (GroupMateChangeType)
	{
	case EGroupMateChangeType::kAdd:
	{
		if (LeaderPCPtr)
		{
			if (LeaderPCPtr->GetGroupMnaggerComponent()->GetGroupHelper()->OwnerPtr == GetPawn<FPawnType>())
			{
				auto AIPCPtr = LeaderPCPtr->GetController<AHumanAIController>();
				if (AIPCPtr)
				{
					AIPCPtr->SetCampType(ECharacterCampType::kTeamMate);
				}
			}
		}
	}
	break;
	}
}

void AHumanPlayerController::OnFocusEndplay(AActor* Actor, EEndPlayReason::Type EndPlayReason)
{
	ClearFocus();
}

void AHumanPlayerController::OnFocusDeathing(const FGameplayTag Tag, int32 Count)
{
	if (Count > 0)
	{
		ClearFocus();

		auto CharacterPtr = Cast<ACharacterBase>(GetFocusActor());
		if (!CharacterPtr)
		{
			return;
		}

		auto AIPCPtr = CharacterPtr->GetController<AHumanAIController>();
		if (!AIPCPtr)
		{
			return;
		}

		AIPCPtr->GetAbilitySystemComponent()->UnregisterGameplayTagEvent(
			OnOwnedDeathTagDelegateHandle,
			UGameplayTagsSubSystem::GetInstance()->DeathingTag,
			EGameplayTagEventType::NewOrRemoved
		);
	}
}

void AHumanPlayerController::BindRemove(AActor* Actor)
{
	if (!Actor)
	{
		return;
	}

	Actor->OnEndPlay.AddDynamic(this, &ThisClass::OnFocusEndplay);

	auto CharacterPtr = Cast<ACharacterBase>(Actor);
	if (!CharacterPtr)
	{
		return;
	}

	auto AIPCPtr = CharacterPtr->GetController<AHumanAIController>();
	if (!AIPCPtr)
	{
		return;
	}

	auto& DelegateRef = AIPCPtr->GetAbilitySystemComponent()->RegisterGameplayTagEvent(
		UGameplayTagsSubSystem::GetInstance()->DeathingTag,
		EGameplayTagEventType::NewOrRemoved
	);
	OnOwnedDeathTagDelegateHandle = DelegateRef.AddUObject(this, &ThisClass::OnFocusDeathing);
}
