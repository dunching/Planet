
#include "PlanetPlayerController.h"

#include "GameFramework/PlayerState.h"
#include "Interfaces/NetworkPredictionInterface.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PawnMovementComponent.h"
#include "EnhancedInputSubsystems.h"
#include <Engine/Engine.h>
#include <IXRTrackingSystem.h>
#include <IXRCamera.h>
#include "Kismet/KismetMathLibrary.h"

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
#include "PlanetControllerInterface.h"
#include "NavgationSubSysetem.h"
#include "AssetRefMap.h"
#include "FocusIcon.h"
#include "TestCommand.h"
#include "GameplayTagsSubSystem.h"
#include "UICommon.h"
#include "CharacterAttributesComponent.h"
#include "TalentAllocationComponent.h"
#include "SceneUnitContainer.h"
#include "BaseFeatureComponent.h"
#include "EffectsList.h"
#include "PlanetPlayerState.h"
#include "GameMode_Main.h"
#include "KismetGravityLibrary.h"

APlanetPlayerController::APlanetPlayerController(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
}

void APlanetPlayerController::SetFocus(AActor* NewFocus, EAIFocusPriority::Type InPriority)
{
	ClearFocus(InPriority);

	if (NewFocus)
	{
		BindOnFocusRemove(NewFocus);

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
			FocusIconPtr->AddToViewport(EUIOrder::kFocus);
		}
	}
}

AActor* APlanetPlayerController::GetFocusActor() const
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

void APlanetPlayerController::ClearFocus(EAIFocusPriority::Type InPriority)
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

FVector APlanetPlayerController::GetFocalPoint() const
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

FVector APlanetPlayerController::GetFocalPointOnActor(const AActor* Actor) const
{
	return Actor != nullptr ? Actor->GetActorLocation() : FAISystem::InvalidLocation;
}

void APlanetPlayerController::BeginPlay()
{
	Super::BeginPlay();

#if UE_EDITOR || UE_CLIENT
	if (GetNetMode() == NM_Client)
	{
		// 因为Pawn是通过网络同步过来的，所以不在OnPoss里面去做
		auto CurrentPawn = GetPawn();
		if (CurrentPawn->IsA(AHumanCharacter::StaticClass()))
		{
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

			// ResetGroupmateUnit(HoldingItemsComponentPtr->GetSceneUnitContainer()->AddUnit_Groupmate(RowName));
			// 
			// 在SetPawn之后调用
			UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<HumanProcessor::FHumanRegularProcessor>([this, CurrentPawn](auto NewProcessor) {
				NewProcessor->SetPawn(Cast<AHumanCharacter>(CurrentPawn));
				});

			// 绑定效果状态栏
			auto EffectPtr = UUIManagerSubSystem::GetInstance()->ViewEffectsList(true);
			if (EffectPtr)
			{
				EffectPtr->BindCharacterState(GetRealCharacter());
			}
		}
	}
#endif

}

void APlanetPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void APlanetPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
}

void APlanetPlayerController::UpdateRotation(float DeltaTime)
{
	APawn* const MyPawn = GetPawnOrSpectator();
	if (MyPawn)
	{
		FRotator DeltaRot(RotationInput);
		FRotator ViewRotation = GetControlRotation();

		const FVector FocalPoint = GetFocalPoint();
		if (FAISystem::IsValidLocation(FocalPoint))
		{
			const auto FocusRotation = UKismetMathLibrary::Quat_FindBetweenVectors(
				UKismetMathLibrary::MakeRotFromZX(UKismetGravityLibrary::GetGravity(), ViewRotation.Vector()).Vector(),
				UKismetMathLibrary::MakeRotFromZX(UKismetGravityLibrary::GetGravity(), FocalPoint - MyPawn->GetPawnViewLocation()).Vector()
			).Rotator();

			const float AngleTolerance = 1e-3f;
			if (FMath::IsNearlyZero(FocusRotation.Yaw, AngleTolerance))
			{
				DeltaRot.Yaw = 0.f;
			}
			else
			{
				DeltaRot.Yaw = FocusRotation.Yaw;
			}
		}
		DeltaRot.Roll = 0.f;

		if (PlayerCameraManager)
		{
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

		SetControlRotation(ViewRotation);

#if WITH_EDITOR
		RootComponent->SetWorldLocation(MyPawn->GetActorLocation());
#endif

		MyPawn->FaceRotation(ViewRotation, DeltaTime);
	}
}

void APlanetPlayerController::OnPossess(APawn* InPawn)
{
	bool bIsNewPawn = (InPawn != GetPawn());

	if (bIsNewPawn)
	{
	}

	Super::OnPossess(InPawn);

	if (bIsNewPawn)
	{
	}

	if (InPawn)
	{
		// 注意：PC并非是在此处绑定，这段仅为测试
		BindPCWithCharacter();

		if (InPawn->IsA(AHumanCharacter::StaticClass()))
		{
#if UE_EDITOR || UE_SERVER
			if (InPawn)
			{
				if (InPawn->IsA(AHumanCharacter::StaticClass()))
				{
					if (GetNetMode() == NM_DedicatedServer)
					{
						GetGroupMnaggerComponent()->GetTeamHelper()->SwitchTeammateOption(ETeammateOption::kFollow);
					}
				}
			}
#endif
		}
		else if (InPawn->IsA(AHorseCharacter::StaticClass()))
		{
			auto PreviousPawnPtr = UInputProcessorSubSystem::GetInstance()->GetCurrentAction()->GetOwnerActor();

			UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<HorseProcessor::FHorseRegularProcessor>([this, InPawn](auto NewProcessor) {
				NewProcessor->SetPawn(Cast<AHorseCharacter>(InPawn));
				});
		}
	}
}

void APlanetPlayerController::OnUnPossess()
{
	APawn* CurrentPawn = GetPawn();

	auto CharacterPtr = Cast<FPawnType>(CurrentPawn);
	if (CharacterPtr)
	{
	}

	Super::OnUnPossess();
}

void APlanetPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
}

bool APlanetPlayerController::InputKey(const FInputKeyParams& Params)
{
	auto Result = Super::InputKey(Params);

	UInputProcessorSubSystem::GetInstance()->InputKey(Params);

	return Result;
}

void APlanetPlayerController::ResetGroupmateUnit(FCharacterProxy* NewGourpMateUnitPtr)
{
}

UPlanetAbilitySystemComponent* APlanetPlayerController::GetAbilitySystemComponent() const
{
	return GetPawn<FPawnType>()->GetAbilitySystemComponent();
}

UGroupMnaggerComponent* APlanetPlayerController::GetGroupMnaggerComponent()const
{
	return GetPawn<FPawnType>()->GetGroupMnaggerComponent();
}

UHoldingItemsComponent* APlanetPlayerController::GetHoldingItemsComponent() const
{
	return GetPawn<FPawnType>()->GetHoldingItemsComponent();
}

UCharacterAttributesComponent* APlanetPlayerController::GetCharacterAttributesComponent() const
{
	return GetPawn<FPawnType>()->GetCharacterAttributesComponent();
}

UTalentAllocationComponent* APlanetPlayerController::GetTalentAllocationComponent() const
{
	return GetPawn<FPawnType>()->GetTalentAllocationComponent();
}

TWeakObjectPtr<ACharacterBase> APlanetPlayerController::GetTeamFocusTarget() const
{
	if (GetGroupMnaggerComponent() && GetGroupMnaggerComponent()->GetTeamHelper())
	{
		return GetGroupMnaggerComponent()->GetTeamHelper()->GetKnowCharacter();
	}

	return nullptr;
}

TSharedPtr<FCharacterProxy> APlanetPlayerController::GetCharacterUnit()
{
	return GetPawn<FPawnType>()->GetCharacterUnit();
}

ACharacterBase* APlanetPlayerController::GetRealCharacter()const
{
	return Cast<ACharacterBase>(GetPawn());
}

void APlanetPlayerController::OnHPChanged(int32 CurrentValue)
{
	if (CurrentValue <= 0)
	{
		GetAbilitySystemComponent()->TryActivateAbilitiesByTag(FGameplayTagContainer{ UGameplayTagsSubSystem::GetInstance()->DeathingTag });
		GetAbilitySystemComponent()->OnAbilityEnded.AddLambda([this](const FAbilityEndedData& AbilityEndedData) {
			for (auto Iter : AbilityEndedData.AbilityThatEnded->AbilityTags)
			{
				if (Iter == UGameplayTagsSubSystem::GetInstance()->DeathingTag)
				{
//					Destroy();
				}
			}
			});
	}
}

void APlanetPlayerController::BindPCWithCharacter()
{
}

TSharedPtr<FCharacterProxy> APlanetPlayerController::InitialCharacterUnit(ACharacterBase* CharaterPtr)
{
	return CharaterPtr->GetCharacterUnit();
}

void APlanetPlayerController::OnFocusEndplay(AActor* Actor, EEndPlayReason::Type EndPlayReason)
{
	ClearFocus();
}

void APlanetPlayerController::OnFocusDeathing(const FGameplayTag Tag, int32 Count)
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

void APlanetPlayerController::BindOnFocusRemove(AActor* Actor)
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
