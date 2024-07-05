
#include "GravityMovementComponent.h"

#include <Engine/Engine.h>
#include <GameFramework/Character.h>
#include <IXRTrackingSystem.h>
#include <IXRCamera.h>
#include <Kismet/KismetMathLibrary.h>
#include "Components/CapsuleComponent.h"
#include "AI/Navigation/NavigationDataInterface.h"

DEFINE_LOG_CATEGORY_STATIC(LogGravityCharacterMovement, Log, All);
DEFINE_LOG_CATEGORY_STATIC(LogNavMeshMovement, Log, All);
DEFINE_LOG_CATEGORY_STATIC(LogCharacterNetSmoothing, Log, All);

DECLARE_CYCLE_STAT(TEXT("Char Tick"), STAT_CharacterMovementTick, STATGROUP_Character);
DECLARE_CYCLE_STAT(TEXT("Char NonSimulated Time"), STAT_CharacterMovementNonSimulated, STATGROUP_Character);
DECLARE_CYCLE_STAT(TEXT("Char Simulated Time"), STAT_CharacterMovementSimulated, STATGROUP_Character);
DECLARE_CYCLE_STAT(TEXT("Char PerformMovement"), STAT_CharacterMovementPerformMovement, STATGROUP_Character);
DECLARE_CYCLE_STAT(TEXT("Char ReplicateMoveToServer"), STAT_CharacterMovementReplicateMoveToServer, STATGROUP_Character);
DECLARE_CYCLE_STAT(TEXT("Char CallServerMove"), STAT_CharacterMovementCallServerMove, STATGROUP_Character);
DECLARE_CYCLE_STAT(TEXT("Char ServerMove"), STAT_CharacterMovementServerMove, STATGROUP_Character);
DECLARE_CYCLE_STAT(TEXT("Char ServerForcePositionUpdate"), STAT_CharacterMovementForcePositionUpdate, STATGROUP_Character);
DECLARE_CYCLE_STAT(TEXT("Char RootMotionSource Calculate"), STAT_CharacterMovementRootMotionSourceCalculate, STATGROUP_Character);
DECLARE_CYCLE_STAT(TEXT("Char RootMotionSource Apply"), STAT_CharacterMovementRootMotionSourceApply, STATGROUP_Character);
DECLARE_CYCLE_STAT(TEXT("Char ClientUpdatePositionAfterServerUpdate"), STAT_CharacterMovementClientUpdatePositionAfterServerUpdate, STATGROUP_Character);
DECLARE_CYCLE_STAT(TEXT("Char CombineNetMove"), STAT_CharacterMovementCombineNetMove, STATGROUP_Character);
DECLARE_CYCLE_STAT(TEXT("Char NetSmoothCorrection"), STAT_CharacterMovementSmoothCorrection, STATGROUP_Character);
DECLARE_CYCLE_STAT(TEXT("Char SmoothClientPosition"), STAT_CharacterMovementSmoothClientPosition, STATGROUP_Character);
DECLARE_CYCLE_STAT(TEXT("Char SmoothClientPosition_Interp"), STAT_CharacterMovementSmoothClientPosition_Interp, STATGROUP_Character);
DECLARE_CYCLE_STAT(TEXT("Char SmoothClientPosition_Visual"), STAT_CharacterMovementSmoothClientPosition_Visual, STATGROUP_Character);
DECLARE_CYCLE_STAT(TEXT("Char Physics Interation"), STAT_CharPhysicsInteraction, STATGROUP_Character);
DECLARE_CYCLE_STAT(TEXT("Char StepUp"), STAT_CharStepUp, STATGROUP_Character);
DECLARE_CYCLE_STAT(TEXT("Char FindFloor"), STAT_CharFindFloor, STATGROUP_Character);
DECLARE_CYCLE_STAT(TEXT("Char AdjustFloorHeight"), STAT_CharAdjustFloorHeight, STATGROUP_Character);
DECLARE_CYCLE_STAT(TEXT("Char Update Acceleration"), STAT_CharUpdateAcceleration, STATGROUP_Character);
DECLARE_CYCLE_STAT(TEXT("Char MoveUpdateDelegate"), STAT_CharMoveUpdateDelegate, STATGROUP_Character);
DECLARE_CYCLE_STAT(TEXT("Char PhysWalking"), STAT_CharPhysWalking, STATGROUP_Character);
DECLARE_CYCLE_STAT(TEXT("Char PhysFalling"), STAT_CharPhysFalling, STATGROUP_Character);
DECLARE_CYCLE_STAT(TEXT("Char PhysNavWalking"), STAT_CharPhysNavWalking, STATGROUP_Character);
DECLARE_CYCLE_STAT(TEXT("Char NavProjectPoint"), STAT_CharNavProjectPoint, STATGROUP_Character);
DECLARE_CYCLE_STAT(TEXT("Char NavProjectLocation"), STAT_CharNavProjectLocation, STATGROUP_Character);
DECLARE_CYCLE_STAT(TEXT("Char ProcessLanded"), STAT_CharProcessLanded, STATGROUP_Character);

// Defines for build configs
#if DO_CHECK && !UE_BUILD_SHIPPING // Disable even if checks in shipping are enabled.
#define devCode( Code )		checkCode( Code )
#else
#define devCode(...)
#endif

namespace LyraCharacter
{
    static float GroundTraceDistance = 100000.0f;
    FAutoConsoleVariableRef CVar_GroundTraceDistance(TEXT("LyraCharacter.GroundTraceDistance"), GroundTraceDistance, TEXT("Distance to trace down when generating ground information."), ECVF_Cheat);
};

UGravityMovementComponent::UGravityMovementComponent(const FObjectInitializer& ObjectInitializer):
    Super(ObjectInitializer)
{
}

const FLyraCharacterGroundInfo& UGravityMovementComponent::GetGroundInfo()
{
    if (!CharacterOwner || (GFrameCounter == CachedGroundInfo.LastUpdateFrame))
    {
        return CachedGroundInfo;
    }

    if (MovementMode == MOVE_Walking)
    {
        CachedGroundInfo.GroundHitResult = CurrentFloor.HitResult;
        CachedGroundInfo.GroundDistance = 0.0f;
    }
    else
    {
        const UCapsuleComponent* CapsuleComp = CharacterOwner->GetCapsuleComponent();
        check(CapsuleComp);

        const float CapsuleHalfHeight = CapsuleComp->GetUnscaledCapsuleHalfHeight();
        const ECollisionChannel CollisionChannel = (UpdatedComponent ? UpdatedComponent->GetCollisionObjectType() : ECC_Pawn);
        const FVector TraceStart(GetActorLocation());
        const FVector TraceEnd(TraceStart.X, TraceStart.Y, (TraceStart.Z - LyraCharacter::GroundTraceDistance - CapsuleHalfHeight));

        FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(LyraCharacterMovementComponent_GetGroundInfo), false, CharacterOwner);
        FCollisionResponseParams ResponseParam;
        InitCollisionParams(QueryParams, ResponseParam);

        FHitResult HitResult;
        GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, CollisionChannel, QueryParams, ResponseParam);

        CachedGroundInfo.GroundHitResult = HitResult;
        CachedGroundInfo.GroundDistance = LyraCharacter::GroundTraceDistance;

        if (MovementMode == MOVE_NavWalking)
        {
            CachedGroundInfo.GroundDistance = 0.0f;
        }
        else if (HitResult.bBlockingHit)
        {
            CachedGroundInfo.GroundDistance = FMath::Max((HitResult.Distance - CapsuleHalfHeight), 0.0f);
        }
    }

    CachedGroundInfo.LastUpdateFrame = GFrameCounter;

    return CachedGroundInfo;
}

void UGravityMovementComponent::BeginPlay()
{
    Super::BeginPlay();

    UpdateGravityTransform(0.f);
	TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &ThisClass::UpdateGravityTransform), Frequency);
}

void UGravityMovementComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    FTSTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);

	Super::EndPlay(EndPlayReason);
}

void UGravityMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UGravityMovementComponent::PerformMovement(float DeltaSeconds)
{
    SCOPE_CYCLE_COUNTER(STAT_CharacterMovementPerformMovement);

    const UWorld* MyWorld = GetWorld();
    if (!HasValidData() || MyWorld == nullptr)
    {
        return;
    }

    bTeleportedSinceLastUpdate = UpdatedComponent->GetComponentLocation() != LastUpdateLocation;

    // no movement if we can't move, or if currently doing physical simulation on UpdatedComponent
    if (MovementMode == MOVE_None || UpdatedComponent->Mobility != EComponentMobility::Movable || UpdatedComponent->IsSimulatingPhysics())
    {
        if (!CharacterOwner->bClientUpdating && !CharacterOwner->bServerMoveIgnoreRootMotion)
        {
            // Consume root motion
            if (CharacterOwner->IsPlayingRootMotion() && CharacterOwner->GetMesh())
            {
                TickCharacterPose(DeltaSeconds);
                RootMotionParams.Clear();
            }
            if (CurrentRootMotion.HasActiveRootMotionSources())
            {
                CurrentRootMotion.Clear();
            }
        }
        // Clear pending physics forces
        ClearAccumulatedForces();
        return;
    }

    // Force floor update if we've moved outside of CharacterMovement since last update.
    bForceNextFloorCheck |= (IsMovingOnGround() && bTeleportedSinceLastUpdate);

    // Update saved LastPreAdditiveVelocity with any external changes to character Velocity that happened since last update.
    if (CurrentRootMotion.HasAdditiveVelocity())
    {
        const FVector Adjustment = (Velocity - LastUpdateVelocity);
        CurrentRootMotion.LastPreAdditiveVelocity += Adjustment;

#if ROOT_MOTION_DEBUG
        if (RootMotionSourceDebug::CVarDebugRootMotionSources.GetValueOnGameThread() == 1)
        {
            if (!Adjustment.IsNearlyZero())
            {
                FString AdjustedDebugString = FString::Printf(TEXT("PerformMovement HasAdditiveVelocity LastUpdateVelocityAdjustment LastPreAdditiveVelocity(%s) Adjustment(%s)"),
                    *CurrentRootMotion.LastPreAdditiveVelocity.ToCompactString(), *Adjustment.ToCompactString());
                RootMotionSourceDebug::PrintOnScreen(*CharacterOwner, AdjustedDebugString);
            }
        }
#endif
    }

    FVector OldVelocity;
    FVector OldLocation;

    // Scoped updates can improve performance of multiple MoveComponent calls.
    {
        struct FScopedCapsuleMovementUpdate : public FScopedMovementUpdate
        {
            typedef FScopedMovementUpdate Super;

            FScopedCapsuleMovementUpdate(USceneComponent* UpdatedComponent, bool bEnabled)
                : Super(bEnabled ? UpdatedComponent : nullptr, EScopedUpdate::DeferredUpdates)
            {
            }
        };

        FScopedCapsuleMovementUpdate ScopedMovementUpdate(UpdatedComponent, bEnableScopedMovementUpdates);

        MaybeUpdateBasedMovement(DeltaSeconds);

        // Clean up invalid RootMotion Sources.
        // This includes RootMotion sources that ended naturally.
        // They might want to perform a clamp on velocity or an override, 
        // so we want this to happen before ApplyAccumulatedForces and HandlePendingLaunch as to not clobber these.
        const bool bHasRootMotionSources = HasRootMotionSources();
        if (bHasRootMotionSources && !CharacterOwner->bClientUpdating && !CharacterOwner->bServerMoveIgnoreRootMotion)
        {
            SCOPE_CYCLE_COUNTER(STAT_CharacterMovementRootMotionSourceCalculate);

            const FVector VelocityBeforeCleanup = Velocity;
            CurrentRootMotion.CleanUpInvalidRootMotion(DeltaSeconds, *CharacterOwner, *this);

#if ROOT_MOTION_DEBUG
            if (RootMotionSourceDebug::CVarDebugRootMotionSources.GetValueOnGameThread() == 1)
            {
                if (Velocity != VelocityBeforeCleanup)
                {
                    const FVector Adjustment = Velocity - VelocityBeforeCleanup;
                    FString AdjustedDebugString = FString::Printf(TEXT("PerformMovement CleanUpInvalidRootMotion Velocity(%s) VelocityBeforeCleanup(%s) Adjustment(%s)"),
                        *Velocity.ToCompactString(), *VelocityBeforeCleanup.ToCompactString(), *Adjustment.ToCompactString());
                    RootMotionSourceDebug::PrintOnScreen(*CharacterOwner, AdjustedDebugString);
                }
            }
#endif
        }

        OldVelocity = Velocity;
        OldLocation = UpdatedComponent->GetComponentLocation();

        ApplyAccumulatedForces(DeltaSeconds);

        // Update the character state before we do our movement
        UpdateCharacterStateBeforeMovement(DeltaSeconds);

        if (MovementMode == MOVE_NavWalking && bWantsToLeaveNavWalking)
        {
            TryToLeaveNavWalking();
        }

        // Character::LaunchCharacter() has been deferred until now.
        HandlePendingLaunch();
        ClearAccumulatedForces();

#if ROOT_MOTION_DEBUG
        if (RootMotionSourceDebug::CVarDebugRootMotionSources.GetValueOnGameThread() == 1)
        {
            if (OldVelocity != Velocity)
            {
                const FVector Adjustment = Velocity - OldVelocity;
                FString AdjustedDebugString = FString::Printf(TEXT("PerformMovement ApplyAccumulatedForces+HandlePendingLaunch Velocity(%s) OldVelocity(%s) Adjustment(%s)"),
                    *Velocity.ToCompactString(), *OldVelocity.ToCompactString(), *Adjustment.ToCompactString());
                RootMotionSourceDebug::PrintOnScreen(*CharacterOwner, AdjustedDebugString);
            }
        }
#endif

        // Update saved LastPreAdditiveVelocity with any external changes to character Velocity that happened due to ApplyAccumulatedForces/HandlePendingLaunch
        if (CurrentRootMotion.HasAdditiveVelocity())
        {
            const FVector Adjustment = (Velocity - OldVelocity);
            CurrentRootMotion.LastPreAdditiveVelocity += Adjustment;

#if ROOT_MOTION_DEBUG
            if (RootMotionSourceDebug::CVarDebugRootMotionSources.GetValueOnGameThread() == 1)
            {
                if (!Adjustment.IsNearlyZero())
                {
                    FString AdjustedDebugString = FString::Printf(TEXT("PerformMovement HasAdditiveVelocity AccumulatedForces LastPreAdditiveVelocity(%s) Adjustment(%s)"),
                        *CurrentRootMotion.LastPreAdditiveVelocity.ToCompactString(), *Adjustment.ToCompactString());
                    RootMotionSourceDebug::PrintOnScreen(*CharacterOwner, AdjustedDebugString);
                }
            }
#endif
        }

        // Prepare Root Motion (generate/accumulate from root motion sources to be used later)
        if (bHasRootMotionSources && !CharacterOwner->bClientUpdating && !CharacterOwner->bServerMoveIgnoreRootMotion)
        {
            // Animation root motion - If using animation RootMotion, tick animations before running physics.
            if (CharacterOwner->IsPlayingRootMotion() && CharacterOwner->GetMesh())
            {
                TickCharacterPose(DeltaSeconds);

                // Make sure animation didn't trigger an event that destroyed us
                if (!HasValidData())
                {
                    return;
                }

                // For local human clients, save off root motion data so it can be used by movement networking code.
                if (CharacterOwner->IsLocallyControlled() && (CharacterOwner->GetLocalRole() == ROLE_AutonomousProxy) && CharacterOwner->IsPlayingNetworkedRootMotionMontage())
                {
                    CharacterOwner->ClientRootMotionParams = RootMotionParams;
                }
            }

            // Generates root motion to be used this frame from sources other than animation
            {
                SCOPE_CYCLE_COUNTER(STAT_CharacterMovementRootMotionSourceCalculate);
                CurrentRootMotion.PrepareRootMotion(DeltaSeconds, *CharacterOwner, *this, true);
            }

            // For local human clients, save off root motion data so it can be used by movement networking code.
            if (CharacterOwner->IsLocallyControlled() && (CharacterOwner->GetLocalRole() == ROLE_AutonomousProxy))
            {
                CharacterOwner->SavedRootMotion = CurrentRootMotion;
            }
        }

        // Apply Root Motion to Velocity
        if (CurrentRootMotion.HasOverrideVelocity() || HasAnimRootMotion())
        {
            // Animation root motion overrides Velocity and currently doesn't allow any other root motion sources
            if (HasAnimRootMotion())
            {
                // Convert to world space (animation root motion is always local)
                USkeletalMeshComponent* SkelMeshComp = CharacterOwner->GetMesh();
                if (SkelMeshComp)
                {
                    // Convert Local Space Root Motion to world space. Do it right before used by physics to make sure we use up to date transforms, as translation is relative to rotation.
                    RootMotionParams.Set(ConvertLocalRootMotionToWorld(RootMotionParams.GetRootMotionTransform(), DeltaSeconds));
                }

                // Then turn root motion to velocity to be used by various physics modes.
                if (DeltaSeconds > 0.f)
                {
                    AnimRootMotionVelocity = CalcAnimRootMotionVelocity(RootMotionParams.GetRootMotionTransform().GetTranslation(), DeltaSeconds, Velocity);
                    Velocity = ConstrainAnimRootMotionVelocity(AnimRootMotionVelocity, Velocity);
                    if (IsFalling())
                    {
                        Velocity += FVector(DecayingFormerBaseVelocity.X, DecayingFormerBaseVelocity.Y, 0.f);
                    }
                }

                UE_LOG(LogRootMotion, Log, TEXT("PerformMovement WorldSpaceRootMotion Translation: %s, Rotation: %s, Actor Facing: %s, Velocity: %s")
                    , *RootMotionParams.GetRootMotionTransform().GetTranslation().ToCompactString()
                    , *RootMotionParams.GetRootMotionTransform().GetRotation().Rotator().ToCompactString()
                    , *CharacterOwner->GetActorForwardVector().ToCompactString()
                    , *Velocity.ToCompactString()
                );
            }
            else
            {
                // We don't have animation root motion so we apply other sources
                if (DeltaSeconds > 0.f)
                {
                    SCOPE_CYCLE_COUNTER(STAT_CharacterMovementRootMotionSourceApply);

                    const FVector VelocityBeforeOverride = Velocity;
                    FVector NewVelocity = Velocity;
                    CurrentRootMotion.AccumulateOverrideRootMotionVelocity(DeltaSeconds, *CharacterOwner, *this, NewVelocity);
                    if (IsFalling())
                    {
                        NewVelocity += CurrentRootMotion.HasOverrideVelocityWithIgnoreZAccumulate() ? FVector(DecayingFormerBaseVelocity.X, DecayingFormerBaseVelocity.Y, 0.f) : DecayingFormerBaseVelocity;
                    }
                    Velocity = NewVelocity;

#if ROOT_MOTION_DEBUG
                    if (RootMotionSourceDebug::CVarDebugRootMotionSources.GetValueOnGameThread() == 1)
                    {
                        if (VelocityBeforeOverride != Velocity)
                        {
                            FString AdjustedDebugString = FString::Printf(TEXT("PerformMovement AccumulateOverrideRootMotionVelocity Velocity(%s) VelocityBeforeOverride(%s)"),
                                *Velocity.ToCompactString(), *VelocityBeforeOverride.ToCompactString());
                            RootMotionSourceDebug::PrintOnScreen(*CharacterOwner, AdjustedDebugString);
                        }
                    }
#endif
                }
            }
        }

#if ROOT_MOTION_DEBUG
        if (RootMotionSourceDebug::CVarDebugRootMotionSources.GetValueOnGameThread() == 1)
        {
            FString AdjustedDebugString = FString::Printf(TEXT("PerformMovement Velocity(%s) OldVelocity(%s)"),
                *Velocity.ToCompactString(), *OldVelocity.ToCompactString());
            RootMotionSourceDebug::PrintOnScreen(*CharacterOwner, AdjustedDebugString);
        }
#endif

        // NaN tracking
        devCode(ensureMsgf(!Velocity.ContainsNaN(), TEXT("UCharacterMovementComponent::PerformMovement: Velocity contains NaN (%s)\n%s"), *GetPathNameSafe(this), *Velocity.ToString()));

        // Clear jump input now, to allow movement events to trigger it for next update.
        CharacterOwner->ClearJumpInput(DeltaSeconds);
        NumJumpApexAttempts = 0;

        // change position
        StartNewPhysics(DeltaSeconds, 0);

        if (!HasValidData())
        {
            return;
        }

        // Update character state based on change from movement
        UpdateCharacterStateAfterMovement(DeltaSeconds);

        if (bAllowPhysicsRotationDuringAnimRootMotion || !HasAnimRootMotion())
        {
            PhysicsRotation(DeltaSeconds);
        }

        // Apply Root Motion rotation after movement is complete.
        if (HasAnimRootMotion())
        {
            const FQuat OldActorRotationQuat = UpdatedComponent->GetComponentQuat();
            const FQuat RootMotionRotationQuat = RootMotionParams.GetRootMotionTransform().GetRotation();
            if (!RootMotionRotationQuat.IsIdentity())
            {
                const FQuat NewActorRotationQuat = RootMotionRotationQuat * OldActorRotationQuat;
                MoveUpdatedComponent(FVector::ZeroVector, NewActorRotationQuat, true);
            }

#if !(UE_BUILD_SHIPPING)
            // debug
            if (false)
            {
                const FRotator OldActorRotation = OldActorRotationQuat.Rotator();
                const FVector ResultingLocation = UpdatedComponent->GetComponentLocation();
                const FRotator ResultingRotation = UpdatedComponent->GetComponentRotation();

                // Show current position
                DrawDebugCoordinateSystem(MyWorld, CharacterOwner->GetMesh()->GetComponentLocation() + FVector(0, 0, 1), ResultingRotation, 50.f, false);

                // Show resulting delta move.
                DrawDebugLine(MyWorld, OldLocation, ResultingLocation, FColor::Red, false, 10.f);

                // Log details.
                UE_LOG(LogRootMotion, Warning, TEXT("PerformMovement Resulting DeltaMove Translation: %s, Rotation: %s, MovementBase: %s"), //-V595
                    *(ResultingLocation - OldLocation).ToCompactString(), *(ResultingRotation - OldActorRotation).GetNormalized().ToCompactString(), *GetNameSafe(CharacterOwner->GetMovementBase()));

                const FVector RMTranslation = RootMotionParams.GetRootMotionTransform().GetTranslation();
                const FRotator RMRotation = RootMotionParams.GetRootMotionTransform().GetRotation().Rotator();
                UE_LOG(LogRootMotion, Warning, TEXT("PerformMovement Resulting DeltaError Translation: %s, Rotation: %s"),
                    *(ResultingLocation - OldLocation - RMTranslation).ToCompactString(), *(ResultingRotation - OldActorRotation - RMRotation).GetNormalized().ToCompactString());
            }
#endif // !(UE_BUILD_SHIPPING)

            // Root Motion has been used, clear
            RootMotionParams.Clear();
        }
        else if (CurrentRootMotion.HasActiveRootMotionSources())
        {
            FQuat RootMotionRotationQuat;
            if (CharacterOwner && UpdatedComponent && CurrentRootMotion.GetOverrideRootMotionRotation(DeltaSeconds, *CharacterOwner, *this, RootMotionRotationQuat))
            {
                const FQuat OldActorRotationQuat = UpdatedComponent->GetComponentQuat();
                const FQuat NewActorRotationQuat = RootMotionRotationQuat * OldActorRotationQuat;
                MoveUpdatedComponent(FVector::ZeroVector, NewActorRotationQuat, true);
            }
        }

        // consume path following requested velocity
        LastUpdateRequestedVelocity = bHasRequestedVelocity ? RequestedVelocity : FVector::ZeroVector;
        bHasRequestedVelocity = false;

        OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
    } // End scoped movement update

    // Call external post-movement events. These happen after the scoped movement completes in case the events want to use the current state of overlaps etc.
    CallMovementUpdateDelegate(DeltaSeconds, OldLocation, OldVelocity);

    if (0 == 0)
    {
        SaveBaseLocation(); // behaviour before implementing this fix
    }
    else
    {
        MaybeSaveBaseLocation();
    }
    UpdateComponentVelocity();

    const bool bHasAuthority = CharacterOwner && CharacterOwner->HasAuthority();

    // If we move we want to avoid a long delay before replication catches up to notice this change, especially if it's throttling our rate.
    if (bHasAuthority && UNetDriver::IsAdaptiveNetUpdateFrequencyEnabled() && UpdatedComponent)
    {
        UNetDriver* NetDriver = MyWorld->GetNetDriver();
        if (NetDriver && NetDriver->IsServer())
        {
            if (!NetDriver->IsPendingNetUpdate(CharacterOwner) && NetDriver->IsNetworkActorUpdateFrequencyThrottled(CharacterOwner))
            {
                if (ShouldCancelAdaptiveReplication())
                {
                    NetDriver->CancelAdaptiveReplication(CharacterOwner);
                }
            }
        }
    }

    const FVector NewLocation = UpdatedComponent ? UpdatedComponent->GetComponentLocation() : FVector::ZeroVector;
    const FQuat NewRotation = UpdatedComponent ? UpdatedComponent->GetComponentQuat() : FQuat::Identity;

    if (bHasAuthority && UpdatedComponent && !IsNetMode(NM_Client))
    {
        const bool bLocationChanged = (NewLocation != LastUpdateLocation);
        const bool bRotationChanged = (NewRotation != LastUpdateRotation);
        if (bLocationChanged || bRotationChanged)
        {
            // Update ServerLastTransformUpdateTimeStamp. This is used by Linear smoothing on clients to interpolate positions with the correct delta time,
            // so the timestamp should be based on the client's move delta (ServerAccumulatedClientTimeStamp), not the server time when receiving the RPC.
            const bool bIsRemotePlayer = (CharacterOwner->GetRemoteRole() == ROLE_AutonomousProxy);
            const FNetworkPredictionData_Server_Character* ServerData = bIsRemotePlayer ? GetPredictionData_Server_Character() : nullptr;
            if (bIsRemotePlayer && ServerData && 0)
            {
                ServerLastTransformUpdateTimeStamp = float(ServerData->ServerAccumulatedClientTimeStamp);
            }
            else
            {
                ServerLastTransformUpdateTimeStamp = MyWorld->GetTimeSeconds();
            }
        }
    }

    LastUpdateLocation = NewLocation;
    LastUpdateRotation = NewRotation;
    LastUpdateVelocity = Velocity;
}

void UGravityMovementComponent::ApplyRootMotionToVelocity(float deltaTime)
{
    SCOPE_CYCLE_COUNTER(STAT_CharacterMovementRootMotionSourceApply);

    // Animation root motion is distinct from root motion sources right now and takes precedence
    if (HasAnimRootMotion() && deltaTime > 0.f)
    {
        Velocity = ConstrainAnimRootMotionVelocity(AnimRootMotionVelocity, Velocity);
        if (IsFalling())
        {
            Velocity += FVector(DecayingFormerBaseVelocity.X, DecayingFormerBaseVelocity.Y, 0.f);
        }
        return;
    }

    const FVector OldVelocity = Velocity;

    bool bAppliedRootMotion = false;

    // Apply override velocity
    if (CurrentRootMotion.HasOverrideVelocity())
    {
        CurrentRootMotion.AccumulateOverrideRootMotionVelocity(deltaTime, *CharacterOwner, *this, Velocity);
        if (IsFalling())
        {
            Velocity += CurrentRootMotion.HasOverrideVelocityWithIgnoreZAccumulate() ? FVector(DecayingFormerBaseVelocity.X, DecayingFormerBaseVelocity.Y, 0.f) : DecayingFormerBaseVelocity;
        }
        bAppliedRootMotion = true;

#if ROOT_MOTION_DEBUG
        if (RootMotionSourceDebug::CVarDebugRootMotionSources.GetValueOnGameThread() == 1)
        {
            FString AdjustedDebugString = FString::Printf(TEXT("ApplyRootMotionToVelocity HasOverrideVelocity Velocity(%s)"),
                *Velocity.ToCompactString());
            RootMotionSourceDebug::PrintOnScreen(*CharacterOwner, AdjustedDebugString);
        }
#endif
    }

    // Next apply additive root motion
    if (CurrentRootMotion.HasAdditiveVelocity())
    {
        CurrentRootMotion.LastPreAdditiveVelocity = Velocity; // Save off pre-additive Velocity for restoration next tick
        CurrentRootMotion.AccumulateAdditiveRootMotionVelocity(deltaTime, *CharacterOwner, *this, Velocity);
        CurrentRootMotion.bIsAdditiveVelocityApplied = true; // Remember that we have it applied
        bAppliedRootMotion = true;

#if ROOT_MOTION_DEBUG
        if (RootMotionSourceDebug::CVarDebugRootMotionSources.GetValueOnGameThread() == 1)
        {
            FString AdjustedDebugString = FString::Printf(TEXT("ApplyRootMotionToVelocity HasAdditiveVelocity Velocity(%s) LastPreAdditiveVelocity(%s)"),
                *Velocity.ToCompactString(), *CurrentRootMotion.LastPreAdditiveVelocity.ToCompactString());
            RootMotionSourceDebug::PrintOnScreen(*CharacterOwner, AdjustedDebugString);
        }
#endif
    }

    // Switch to Falling if we have vertical velocity from root motion so we can lift off the ground
    const FVector AppliedVelocityDelta = RotateWorldToGravity(Velocity - OldVelocity);
    if (bAppliedRootMotion && AppliedVelocityDelta.Z != 0.f && IsMovingOnGround())
    {
        float LiftoffBound;
        if (CurrentRootMotion.LastAccumulatedSettings.HasFlag(ERootMotionSourceSettingsFlags::UseSensitiveLiftoffCheck))
        {
            // Sensitive bounds - "any positive force"
            LiftoffBound = UE_SMALL_NUMBER;
        }
        else
        {
            // Default bounds - the amount of force gravity is applying this tick
            LiftoffBound = FMath::Max(-GetGravityZ() * deltaTime, UE_SMALL_NUMBER);
        }

        if (AppliedVelocityDelta.Z > LiftoffBound)
        {
            SetMovementMode(MOVE_Falling);
        }
    }
}

void UGravityMovementComponent::SetGravityDirection(const FVector& InNewGravityDir)
{
    FVector NewGravityDir = InNewGravityDir.GetSafeNormal();
    if (ensure(!NewGravityDir.IsNearlyZero()))
    {
        if (!GravityDirection.Equals(NewGravityDir))
        {
            GravityDirection = NewGravityDir;
            WorldToGravityTransform = UKismetMathLibrary::MakeRotFromZX(
                -GravityDirection,
                PreviousGravityTransformForward
            ).Quaternion();
            GravityToWorldTransform = WorldToGravityTransform.Inverse();
			PreviousGravityTransformForward = WorldToGravityTransform.GetForwardVector();

			bHasCustomGravity = !GravityDirection.Equals(DefaultGravityDirection);
        }
    }
}

void UGravityMovementComponent::PhysicsRotation(float DeltaTime)
{
	if (HasRootMotionSources() || HasAnimRootMotion())
	{
		return;
	}

    Super::PhysicsRotation(DeltaTime);
}

void UGravityMovementComponent::PhysWalking(float deltaTime, int32 Iterations)
{
    if (deltaTime < MIN_TICK_TIME)
    {
        return;
    }

    if (!CharacterOwner || (!CharacterOwner->Controller && !bRunPhysicsWithNoController && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() && (CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)))
    {
        Acceleration = FVector::ZeroVector;
        Velocity = FVector::ZeroVector;
        return;
    }

    if (!UpdatedComponent->IsQueryCollisionEnabled())
    {
        SetMovementMode(MOVE_Walking);
        return;
    }

    bJustTeleported = false;
    bool bCheckedFall = false;
    bool bTriedLedgeMove = false;
    float remainingTime = deltaTime;

    // Perform the move
    while ((remainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations) && CharacterOwner && (CharacterOwner->Controller || bRunPhysicsWithNoController || HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity() || (CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)))
    {
        Iterations++;
        bJustTeleported = false;
        const float timeTick = GetSimulationTimeStep(remainingTime, Iterations);
        remainingTime -= timeTick;

        // Save current values
        UPrimitiveComponent* const OldBase = GetMovementBase();
        const FVector PreviousBaseLocation = (OldBase != NULL) ? OldBase->GetComponentLocation() : FVector::ZeroVector;
        const FVector OldLocation = UpdatedComponent->GetComponentLocation();
        const FFindFloorResult OldFloor = CurrentFloor;

        RestorePreAdditiveRootMotionVelocity();

        // Ensure velocity is horizontal.
        MaintainHorizontalGroundVelocity();
        const FVector OldVelocity = Velocity;

        // Apply acceleration
        if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
        {
            CalcVelocity(timeTick, GroundFriction, false, GetMaxBrakingDeceleration());
        }

        ApplyRootMotionToVelocity(timeTick);

        if (IsFalling())
        {
            // Root motion could have put us into Falling.
            // No movement has taken place this movement tick so we pass on full time/past iteration count
            StartNewPhysics(remainingTime + timeTick, Iterations - 1);
            return;
        }

        // Compute move parameters
        const FVector MoveVelocity = Velocity;
        const FVector Delta = timeTick * MoveVelocity;
        const bool bZeroDelta = Delta.IsNearlyZero();
        FStepDownResult StepDownResult;

        if (bZeroDelta)
        {
            remainingTime = 0.f;
        }
        else
        {
            // try to move forward
            MoveAlongFloor(MoveVelocity, timeTick, &StepDownResult);

            if (IsFalling())
            {
                // pawn decided to jump up
                const float DesiredDist = Delta.Size();
                if (DesiredDist > UE_KINDA_SMALL_NUMBER)
                {
                    const float ActualDist = (UpdatedComponent->GetComponentLocation() - OldLocation).Size2D();
                    remainingTime += timeTick * (1.f - FMath::Min(1.f, ActualDist / DesiredDist));
                }
                StartNewPhysics(remainingTime, Iterations);
                return;
            }
            else if (IsSwimming()) //just entered water
            {
                StartSwimming(OldLocation, OldVelocity, timeTick, remainingTime, Iterations);
                return;
            }
        }

        // Update floor.
        // StepUp might have already done it for us.
        if (StepDownResult.bComputedFloor)
        {
            CurrentFloor = StepDownResult.FloorResult;
        }
        else
        {
            FindFloor(UpdatedComponent->GetComponentLocation(), CurrentFloor, bZeroDelta, NULL);
        }

        // check for ledges here
        const bool bCheckLedges = !CanWalkOffLedges();
        if (bCheckLedges && !CurrentFloor.IsWalkableFloor())
        {
            // calculate possible alternate movement
            const FVector GravDir = GetGravityDirection();
            const FVector NewDelta = bTriedLedgeMove ? FVector::ZeroVector : GetLedgeMove(OldLocation, Delta, GravDir);
            if (!NewDelta.IsZero())
            {
                // first revert this move
                RevertMove(OldLocation, OldBase, PreviousBaseLocation, OldFloor, false);

                // avoid repeated ledge moves if the first one fails
                bTriedLedgeMove = true;

                // Try new movement direction
                Velocity = NewDelta / timeTick;
                remainingTime += timeTick;
                continue;
            }
            else
            {
                // see if it is OK to jump
                // @todo collision : only thing that can be problem is that oldbase has world collision on
                bool bMustJump = bZeroDelta || (OldBase == NULL || (!OldBase->IsQueryCollisionEnabled() && MovementBaseUtility::IsDynamicBase(OldBase)));
                if ((bMustJump || !bCheckedFall) && CheckFall(OldFloor, CurrentFloor.HitResult, Delta, OldLocation, remainingTime, timeTick, Iterations, bMustJump))
                {
                    return;
                }
                bCheckedFall = true;

                // revert this move
                RevertMove(OldLocation, OldBase, PreviousBaseLocation, OldFloor, true);
                remainingTime = 0.f;
                break;
            }
        }
        else
        {
            // Validate the floor check
            if (CurrentFloor.IsWalkableFloor())
            {
                if (ShouldCatchAir(OldFloor, CurrentFloor))
                {
                    HandleWalkingOffLedge(OldFloor.HitResult.ImpactNormal, OldFloor.HitResult.Normal, OldLocation, timeTick);
                    if (IsMovingOnGround())
                    {
                        // If still walking, then fall. If not, assume the user set a different mode they want to keep.
                        StartFalling(Iterations, remainingTime, timeTick, Delta, OldLocation);
                    }
                    return;
                }

                AdjustFloorHeight();
                SetBase(CurrentFloor.HitResult.Component.Get(), CurrentFloor.HitResult.BoneName);
            }
            else if (CurrentFloor.HitResult.bStartPenetrating && remainingTime <= 0.f)
            {
                // The floor check failed because it started in penetration
                // We do not want to try to move downward because the downward sweep failed, rather we'd like to try to pop out of the floor.
                FHitResult Hit(CurrentFloor.HitResult);
                Hit.TraceEnd = Hit.TraceStart + RotateGravityToWorld(FVector(0.f, 0.f, MAX_FLOOR_DIST));
                const FVector RequestedAdjustment = GetPenetrationAdjustment(Hit);
                ResolvePenetration(RequestedAdjustment, Hit, UpdatedComponent->GetComponentQuat());
                bForceNextFloorCheck = true;
            }

            // check if just entered water
            if (IsSwimming())
            {
                StartSwimming(OldLocation, Velocity, timeTick, remainingTime, Iterations);
                return;
            }

            // See if we need to start falling.
            if (!CurrentFloor.IsWalkableFloor() && !CurrentFloor.HitResult.bStartPenetrating)
            {
                const bool bMustJump = bJustTeleported || bZeroDelta || (OldBase == NULL || (!OldBase->IsQueryCollisionEnabled() && MovementBaseUtility::IsDynamicBase(OldBase)));
                if ((bMustJump || !bCheckedFall) && CheckFall(OldFloor, CurrentFloor.HitResult, Delta, OldLocation, remainingTime, timeTick, Iterations, bMustJump))
                {
                    return;
                }
                bCheckedFall = true;
            }
        }


        // Allow overlap events and such to change physics state and velocity
        if (IsMovingOnGround())
        {
            // Make velocity reflect actual move
            if (!bJustTeleported && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() && timeTick >= MIN_TICK_TIME)
            {
                // TODO-RootMotionSource: Allow this to happen during partial override Velocity, but only set allowed axes?
                Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / timeTick;
                MaintainHorizontalGroundVelocity();
            }
        }

        // If we didn't move at all this iteration then abort (since future iterations will also be stuck).
        if (UpdatedComponent->GetComponentLocation() == OldLocation)
        {
            remainingTime = 0.f;
            break;
        }
    }

    if (IsMovingOnGround())
    {
        MaintainHorizontalGroundVelocity();
    }
}

void UGravityMovementComponent::PhysNavWalking(float deltaTime, int32 Iterations)
{
    SCOPE_CYCLE_COUNTER(STAT_CharPhysNavWalking);

    if (deltaTime < MIN_TICK_TIME)
    {
        return;
    }

    if ((!CharacterOwner || !CharacterOwner->Controller) && !bRunPhysicsWithNoController && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
    {
        Acceleration = FVector::ZeroVector;
        Velocity = FVector::ZeroVector;
        return;
    }

    RestorePreAdditiveRootMotionVelocity();

    // Ensure velocity is horizontal.
    MaintainHorizontalGroundVelocity();
    devCode(ensureMsgf(!Velocity.ContainsNaN(), TEXT("PhysNavWalking: Velocity contains NaN before CalcVelocity (%s)\n%s"), *GetPathNameSafe(this), *Velocity.ToString()));

    //bound acceleration
    Acceleration.Z = 0.f;
    if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
    {
        CalcVelocity(deltaTime, GroundFriction, false, GetMaxBrakingDeceleration());
        devCode(ensureMsgf(!Velocity.ContainsNaN(), TEXT("PhysNavWalking: Velocity contains NaN after CalcVelocity (%s)\n%s"), *GetPathNameSafe(this), *Velocity.ToString()));
    }

    ApplyRootMotionToVelocity(deltaTime);

    if (IsFalling())
    {
        // Root motion could have put us into Falling
        StartNewPhysics(deltaTime, Iterations);
        return;
    }

    Iterations++;

    FVector DesiredMove = Velocity;
    DesiredMove.Z = 0.f;

    const FVector OldLocation = GetActorFeetLocation();
    const FVector DeltaMove = DesiredMove * deltaTime;
    const bool bDeltaMoveNearlyZero = DeltaMove.IsNearlyZero();

    FVector AdjustedDest = OldLocation + DeltaMove;
    FNavLocation DestNavLocation;

    bool bSameNavLocation = false;
    if (CachedNavLocation.NodeRef != INVALID_NAVNODEREF)
    {
        if (bProjectNavMeshWalking)
        {
            const float DistSq2D = (OldLocation - CachedNavLocation.Location).SizeSquared2D();
            const float DistZ = FMath::Abs(OldLocation.Z - CachedNavLocation.Location.Z);

            const float TotalCapsuleHeight = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2.0f;
            const float ProjectionScale = (OldLocation.Z > CachedNavLocation.Location.Z) ? NavMeshProjectionHeightScaleUp : NavMeshProjectionHeightScaleDown;
            const float DistZThr = TotalCapsuleHeight * FMath::Max(0.f, ProjectionScale);

            bSameNavLocation = (DistSq2D <= UE_KINDA_SMALL_NUMBER) && (DistZ < DistZThr);
        }
        else
        {
            bSameNavLocation = CachedNavLocation.Location.Equals(OldLocation);
        }

        if (bDeltaMoveNearlyZero && bSameNavLocation)
        {
            if (const INavigationDataInterface* NavData = GetNavData())
            {
                if (!NavData->IsNodeRefValid(CachedNavLocation.NodeRef))
                {
                    CachedNavLocation.NodeRef = INVALID_NAVNODEREF;
                    bSameNavLocation = false;
                }
            }
        }
    }

    if (bDeltaMoveNearlyZero && bSameNavLocation)
    {
        DestNavLocation = CachedNavLocation;
        UE_LOG(LogNavMeshMovement, VeryVerbose, TEXT("%s using cached navmesh location! (bProjectNavMeshWalking = %d)"), *GetNameSafe(CharacterOwner), bProjectNavMeshWalking);
    }
    else
    {
        SCOPE_CYCLE_COUNTER(STAT_CharNavProjectPoint);

        // Start the trace from the Z location of the last valid trace.
        // Otherwise if we are projecting our location to the underlying geometry and it's far above or below the navmesh,
        // we'll follow that geometry's plane out of range of valid navigation.
        if (bSameNavLocation && bProjectNavMeshWalking)
        {
            AdjustedDest.Z = CachedNavLocation.Location.Z;
        }

        // Find the point on the NavMesh
        const bool bHasNavigationData = FindNavFloor(AdjustedDest, DestNavLocation);
        if (!bHasNavigationData)
        {
            SetMovementMode(MOVE_Walking);
            return;
        }

        CachedNavLocation = DestNavLocation;
    }

    if (DestNavLocation.NodeRef != INVALID_NAVNODEREF)
    {
        FVector NewLocation(AdjustedDest.X, AdjustedDest.Y, DestNavLocation.Location.Z);
        if (bProjectNavMeshWalking)
        {
            SCOPE_CYCLE_COUNTER(STAT_CharNavProjectLocation);
            const float TotalCapsuleHeight = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2.0f;
            const float UpOffset = TotalCapsuleHeight * FMath::Max(0.f, NavMeshProjectionHeightScaleUp);
            const float DownOffset = TotalCapsuleHeight * FMath::Max(0.f, NavMeshProjectionHeightScaleDown);
            NewLocation = ProjectLocationFromNavMesh(deltaTime, OldLocation, NewLocation, UpOffset, DownOffset);
        }

        FVector AdjustedDelta = NewLocation - OldLocation;

        if (!AdjustedDelta.IsNearlyZero())
        {
            FHitResult HitResult;
            SafeMoveUpdatedComponent(AdjustedDelta, UpdatedComponent->GetComponentQuat(), bSweepWhileNavWalking, HitResult);
        }

        // Update velocity to reflect actual move
        if (!bJustTeleported && !HasAnimRootMotion() && !CurrentRootMotion.HasVelocity())
        {
            Velocity = (GetActorFeetLocation() - OldLocation) / deltaTime;
            MaintainHorizontalGroundVelocity();
        }

        bJustTeleported = false;
    }
    else
    {
        StartFalling(Iterations, deltaTime, deltaTime, DeltaMove, OldLocation);
    }
}

bool UGravityMovementComponent::UpdateGravityTransform(float Delta)
{
//	SetGravityDirection(-UpdatedComponent->GetComponentLocation());
	SetGravityDirection(FVector::DownVector);

    return true;
}
