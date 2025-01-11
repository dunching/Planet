#include "PlanetPlayerController.h"

#include "GameFramework/Pawn.h"
#include "EnhancedInputSubsystems.h"
#include <Engine/Engine.h>
#include <IXRTrackingSystem.h>
#include <IXRCamera.h>
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/HUD.h"

#include "InputProcessorSubSystem.h"
#include "HorseCharacter.h"
#include "HumanCharacter.h"
#include "HumanRegularProcessor.h"
#include "HorseRegularProcessor.h"
#include "InputActions.h"
#include "UIManagerSubSystem.h"
#include "CharacterBase.h"
#include "TeamMatesHelperComponent.h"
#include "HumanAIController.h"
#include "ItemProxy_Minimal.h"
#include "PlanetControllerInterface.h"
#include "NavgationSubSysetem.h"
#include "FocusIcon.h"
#include "GameplayTagsLibrary.h"
#include "ItemProxy_Container.h"
#include "CharacterAbilitySystemComponent.h"
#include "HumanCharacter_Player.h"
#include "KismetGravityLibrary.h"
#include "CollisionDataStruct.h"
#include "GameMode_Main.h"
#include "LogWriter.h"
#include "GroupSharedInfo.h"
#include "HoldingItemsComponent.h"
#include "MainHUD.h"
#include "PlanetWorldSettings.h"
#include "WolrdProcess.h"

static TAutoConsoleVariable<int32> PlanetPlayerController_DrawControllerRotation(
	TEXT("PlanetPlayerController.DrawControllerRotation"),
	0,
	TEXT("")
	TEXT(" default: 0"));

APlanetPlayerController::APlanetPlayerController(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
}

void APlanetPlayerController::SetFocus(AActor* NewFocus, EAIFocusPriority::Type InPriority)
{
	if (NewFocus == GetFocusActor())
	{
		return;
	}

	if (auto TargetCharacterPtr = Cast<ACharacterBase>(NewFocus))
	{
		BindOnFocusRemove(TargetCharacterPtr);

		if (InPriority >= FocusInformation.Priorities.Num())
		{
			FocusInformation.Priorities.SetNum(InPriority + 1);
		}
		FocusInformation.Priorities[InPriority].Actor = TargetCharacterPtr;

		OnFocusCharacterDelegate(TargetCharacterPtr);
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

	OnFocusCharacterDelegate(nullptr);
}

FVector APlanetPlayerController::GetFocalPoint() const
{
	FVector Result = FAISystem::InvalidLocation;

	// find focus with the highest priority
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

void APlanetPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, GroupSharedInfoPtr, COND_None);
	// DOREPLIFETIME_CONDITION(ThisClass, WolrdProcessPtr, COND_AutonomousOnly);
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
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
				UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
			{
				Subsystem->AddMappingContext(
					UInputProcessorSubSystem::GetInstance()->InputActionsPtr->InputMappingContext,
					0
				);
			}

			FInputModeGameOnly InputMode;
			SetInputMode(InputMode);
		}
	}
#endif
}

void APlanetPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	InitialGroupSharedInfo();
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
			const auto Z = -UKismetGravityLibrary::GetGravity();
			const auto FocusRotation = UKismetMathLibrary::Quat_FindBetweenVectors(
				UKismetMathLibrary::MakeRotFromZX(Z, ViewRotation.Vector()).Vector(),
				UKismetMathLibrary::MakeRotFromZX(Z, FocalPoint - MyPawn->GetActorLocation()).Vector()
			).Rotator();

			const float AngleTolerance = 1e-3f;
			if (FMath::IsNearlyZero(FocusRotation.Yaw, AngleTolerance))
			{
				DeltaRot.Yaw = 0.f;
			}
			else
			{
				const float RotationRate_Yaw = 120.f * DeltaTime;
				DeltaRot.Yaw = FMath::FixedTurn(0.f, FocusRotation.Yaw, RotationRate_Yaw);
			}
		}
		DeltaRot.Roll = 0.f;

		if (PlayerCameraManager)
		{
			PlayerCameraManager->ProcessViewRotation(DeltaTime, ViewRotation, DeltaRot);
		}

		AActor* ViewTarget = GetViewTarget();
		if (!PlayerCameraManager || !ViewTarget || !ViewTarget->HasActiveCameraComponent() || ViewTarget->
			HasActivePawnControlCameraComponent())
		{
			if (IsLocalPlayerController() && GEngine->XRSystem.IsValid() && GetWorld() != nullptr && GEngine->XRSystem->
				IsHeadTrackingAllowedForWorld(*GetWorld()))
			{
				auto XRCamera = GEngine->XRSystem->GetXRCamera();
				if (XRCamera.IsValid())
				{
					XRCamera->ApplyHMDRotation(this, ViewRotation);
				}
			}
		}

		SetControlRotation(ViewRotation);

#ifdef WITH_EDITOR
		if (PlanetPlayerController_DrawControllerRotation.GetValueOnGameThread())
		{
			if (GetLocalRole() == ROLE_AutonomousProxy)
			{
				DrawDebugLine(GetWorld(), MyPawn->GetActorLocation(),
				              MyPawn->GetActorLocation() + (ViewRotation.Vector() * 500), FColor::Red, false, 3);
			}
		}
#endif

#if WITH_EDITOR
		RootComponent->SetWorldLocation(MyPawn->GetActorLocation());
#endif

		// 不要直接使用Controller上的旋转
		// MyPawn->FaceRotation(ViewRotation, DeltaTime);
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
			if (GetNetMode() == NM_DedicatedServer)
			{
				if (InPawn)
				{
					if (InPawn->IsA(AHumanCharacter::StaticClass()))
					{
						if (GetNetMode() == NM_DedicatedServer)
						{
							GetGroupSharedInfo()->GetTeamMatesHelperComponent()->SwitchTeammateOption(
								ETeammateOption::kFollow);
						}
					}
				}
			}
#endif
		}
		else if (InPawn->IsA(AHorseCharacter::StaticClass()))
		{
			auto PreviousPawnPtr = UInputProcessorSubSystem::GetInstance()->GetCurrentAction()->GetOwnerActor();

			UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<HorseProcessor::FHorseRegularProcessor>(
				[this, InPawn](auto NewProcessor)
				{
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

void APlanetPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	InitialWorldPrcess();
}

void APlanetPlayerController::OnGroupSharedInfoReady(AGroupSharedInfo* NewGroupSharedInfoPtr)
{
	ForEachComponent(false, [this](UActorComponent*ComponentPtr)
	{
		auto GroupSharedInterfacePtr = Cast<IGroupSharedInterface>(ComponentPtr);
		if (GroupSharedInterfacePtr)
		{
			GroupSharedInterfacePtr->OnGroupSharedInfoReady(GroupSharedInfoPtr);
		}
	});
}

void APlanetPlayerController::ResetGroupmateProxy(FCharacterProxy* NewGourpMateProxyPtr)
{
}

UPlanetAbilitySystemComponent* APlanetPlayerController::GetAbilitySystemComponent() const
{
	return GetPawn<FPawnType>()->GetCharacterAbilitySystemComponent();
}

AGroupSharedInfo* APlanetPlayerController::GetGroupSharedInfo() const
{
	return GroupSharedInfoPtr;
}

void APlanetPlayerController::SetGroupSharedInfo(AGroupSharedInfo* InGroupSharedInfoPtr)
{
}

UHoldingItemsComponent* APlanetPlayerController::GetHoldingItemsComponent() const
{
	return GroupSharedInfoPtr ? GroupSharedInfoPtr->GetHoldingItemsComponent() : nullptr;
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
	if (GetGroupSharedInfo() && GetGroupSharedInfo()->GetTeamMatesHelperComponent())
	{
		return GetGroupSharedInfo()->GetTeamMatesHelperComponent()->GetKnowCharacter();
	}

	return nullptr;
}

TSharedPtr<FCharacterProxy> APlanetPlayerController::GetCharacterProxy()
{
	return GetPawn<FPawnType>()->GetCharacterProxy();
}

ACharacterBase* APlanetPlayerController::GetRealCharacter() const
{
	return Cast<ACharacterBase>(GetPawn());
}

void APlanetPlayerController::OnHPChanged(int32 CurrentValue)
{
	if (CurrentValue <= 0)
	{
		GetAbilitySystemComponent()->TryActivateAbilitiesByTag(FGameplayTagContainer{
			UGameplayTagsLibrary::DeathingTag
		});
		GetAbilitySystemComponent()->OnAbilityEnded.AddLambda([this](const FAbilityEndedData& AbilityEndedData)
		{
			for (auto Iter : AbilityEndedData.AbilityThatEnded->AbilityTags)
			{
				if (Iter == UGameplayTagsLibrary::DeathingTag)
				{
					// TODO 
					//					Destroy();
				}
			}
		});
	}
}

void APlanetPlayerController::BindPCWithCharacter()
{
}

TSharedPtr<FCharacterProxy> APlanetPlayerController::InitialCharacterProxy(ACharacterBase* CharaterPtr)
{
	return CharaterPtr->GetCharacterProxy();
}

void APlanetPlayerController::InitialGroupSharedInfo()
{
#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		FActorSpawnParameters SpawnParameters;

		SpawnParameters.Owner = this;
		SpawnParameters.CustomPreSpawnInitalization = [](AActor* ActorPtr)
		{
			PRINTINVOKEINFO();
			auto GroupSharedInfoPtr = Cast<AGroupSharedInfo>(ActorPtr);
			if (GroupSharedInfoPtr)
			{
				GroupSharedInfoPtr->GroupID = FGuid::NewGuid();
			}
		};

		GroupSharedInfoPtr = GetWorld()->SpawnActor<AGroupSharedInfo>(
			AGroupSharedInfo::StaticClass(), SpawnParameters
		);
		OnGroupSharedInfoReady(GroupSharedInfoPtr);
	}
#endif
}

void APlanetPlayerController::InitialWorldPrcess()
{
	auto WorldSetting = Cast<APlanetWorldSettings>(GetWorldImp()->GetWorldSettings());
	
	FActorSpawnParameters SpawnParameters;

	SpawnParameters.Owner = this;

	WolrdProcessPtr = GetWorld()->SpawnActor<AWolrdProcess>(
		WorldSetting->WolrdProcessClass, SpawnParameters
	);
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
			UGameplayTagsLibrary::DeathingTag,
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

	// “消失”时
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

	// 目标进入“死亡”标签
	auto& DelegateRef = AIPCPtr->GetAbilitySystemComponent()->RegisterGameplayTagEvent(
		UGameplayTagsLibrary::DeathingTag,
		EGameplayTagEventType::NewOrRemoved
	);
	OnOwnedDeathTagDelegateHandle = DelegateRef.AddUObject(this, &ThisClass::OnFocusDeathing);
}

void APlanetPlayerController::MakeTrueDamege_Implementation(const TArray<FString>& Args)
{
	auto CharacterPtr = GetPawn<FPawnType>();
	if (!CharacterPtr && !Args.IsValidIndex(0))
	{
		return;
	}

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(Pawn_Object);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(CharacterPtr);

	FVector OutCamLoc = CharacterPtr->GetActorLocation();
	FRotator OutCamRot = CharacterPtr->GetActorRotation();

	FHitResult OutHit;
	if (GetWorld()->LineTraceSingleByObjectType(OutHit, OutCamLoc, OutCamLoc + (OutCamRot.Vector() * 1000),
	                                            ObjectQueryParams, Params))
	{
		auto TargetCharacterPtr = Cast<AHumanCharacter>(OutHit.GetActor());
		if (TargetCharacterPtr)
		{
			FGameplayAbilityTargetData_GASendEvent* GAEventDataPtr = new FGameplayAbilityTargetData_GASendEvent(
				CharacterPtr);

			GAEventDataPtr->TriggerCharacterPtr = CharacterPtr;

			FGAEventData GAEventData(TargetCharacterPtr, CharacterPtr);

			GAEventData.bIsWeaponAttack = true;
			GAEventData.bIsCantEvade = true;
			LexFromString(GAEventData.TrueDamage, *Args[0]);

			GAEventDataPtr->DataAry.Add(GAEventData);

			auto ICPtr = CharacterPtr->GetCharacterAbilitySystemComponent();
			ICPtr->SendEventImp(GAEventDataPtr);
		}
	}
}

void APlanetPlayerController::MakeTherapy_Implementation(const TArray<FString>& Args)
{
	auto CharacterPtr = GetPawn<FPawnType>();
	if (!CharacterPtr && !Args.IsValidIndex(0))
	{
		return;
	}

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(Pawn_Object);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(CharacterPtr);

	FVector OutCamLoc = CharacterPtr->GetActorLocation();
	FRotator OutCamRot = CharacterPtr->GetActorRotation();

	FHitResult OutHit;
	if (GetWorld()->LineTraceSingleByObjectType(OutHit, OutCamLoc, OutCamLoc + (OutCamRot.Vector() * 1000),
	                                            ObjectQueryParams, Params))
	{
		auto TargetCharacterPtr = Cast<AHumanCharacter>(OutHit.GetActor());
		if (TargetCharacterPtr)
		{
			FGameplayAbilityTargetData_GASendEvent* GAEventDataPtr = new FGameplayAbilityTargetData_GASendEvent(
				CharacterPtr);

			GAEventDataPtr->TriggerCharacterPtr = CharacterPtr;

			FGAEventData GAEventData(TargetCharacterPtr, CharacterPtr);

			int32 TreatmentVolume = 0;
			LexFromString(TreatmentVolume, *Args[0]);

			GAEventData.DataModify.Add(ECharacterPropertyType::HP, TreatmentVolume);

			GAEventDataPtr->DataAry.Add(GAEventData);

			auto ICPtr = CharacterPtr->GetCharacterAbilitySystemComponent();
			ICPtr->SendEventImp(GAEventDataPtr);
		}
	}
}

void APlanetPlayerController::MakeRespawn_Implementation(const TArray<FString>& Args)
{
	auto CharacterPtr = GetPawn<FPawnType>();
	if (!CharacterPtr && !Args.IsValidIndex(0))
	{
		return;
	}

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(Pawn_Object);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(CharacterPtr);

	FVector OutCamLoc = CharacterPtr->GetActorLocation();
	FRotator OutCamRot = CharacterPtr->GetActorRotation();

	FHitResult OutHit;
	if (GetWorld()->LineTraceSingleByObjectType(OutHit, OutCamLoc, OutCamLoc + (OutCamRot.Vector() * 1000),
	                                            ObjectQueryParams, Params))
	{
		auto TargetCharacterPtr = Cast<AHumanCharacter>(OutHit.GetActor());
		if (TargetCharacterPtr)
		{
			FGameplayAbilityTargetData_GASendEvent* GAEventDataPtr = new FGameplayAbilityTargetData_GASendEvent(
				CharacterPtr);

			GAEventDataPtr->TriggerCharacterPtr = CharacterPtr;

			FGAEventData GAEventData(TargetCharacterPtr, CharacterPtr);

			int32 TreatmentVolume = 0;
			LexFromString(TreatmentVolume, *Args[0]);

			GAEventData.DataModify.Add(ECharacterPropertyType::HP, TreatmentVolume);

			GAEventData.bIsRespawn = true;

			GAEventDataPtr->DataAry.Add(GAEventData);

			auto ICPtr = CharacterPtr->GetCharacterAbilitySystemComponent();
			ICPtr->SendEventImp(GAEventDataPtr);
		}
	}
}

void APlanetPlayerController::OnRep_GroupSharedInfoChanged()
{
	OnGroupSharedInfoReady(GroupSharedInfoPtr);

	// auto PlayerCharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	// if (PlayerCharacterPtr)
	// {
	// 	SetCampType(
	// 		IsTeammate(PlayerCharacterPtr) ? ECharacterCampType::kTeamMate : ECharacterCampType::kEnemy
	// 	);
	// }

	UUIManagerSubSystem::GetInstance()->InitialUI();
}

void APlanetPlayerController::OnRep_WolrdProcess()
{
}
