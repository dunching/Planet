
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
	if (HasRootMotionSources())
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
	SetGravityDirection(-UpdatedComponent->GetComponentLocation());

    return true;
}
