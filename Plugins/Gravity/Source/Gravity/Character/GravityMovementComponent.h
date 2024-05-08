// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include <GameFramework/CharacterMovementComponent.h>

#include "GravityMovementComponent.generated.h"

USTRUCT(BlueprintType)
struct GRAVITY_API FLyraCharacterGroundInfo
{
    GENERATED_BODY()

    FLyraCharacterGroundInfo()
        : LastUpdateFrame(0)
        , GroundDistance(0.0f)
    {}

    uint64 LastUpdateFrame;

    UPROPERTY(BlueprintReadOnly)
    FHitResult GroundHitResult;

    UPROPERTY(BlueprintReadOnly)
    float GroundDistance;
};

UCLASS(config = Game)
class GRAVITY_API UGravityMovementComponent : public UCharacterMovementComponent
{
    GENERATED_BODY()

public:

    UGravityMovementComponent(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable, Category = "Lyra|CharacterMovement")
    const FLyraCharacterGroundInfo& GetGroundInfo();

    FLyraCharacterGroundInfo CachedGroundInfo;

    virtual void BeginPlay() override;

    virtual void TickComponent(
        float DeltaTime,
        enum ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction
    ) override;

    virtual void SetGravityDirection(const FVector& GravityDir)override;

    virtual void CalcVelocity(
        float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration
    )override;

    virtual void MaintainHorizontalGroundVelocity()override;

    virtual FVector ConstrainInputAcceleration(const FVector& InputAcceleration) const override;

    virtual FRotator ComputeOrientToMovementRotation(
        const FRotator& CurrentRotation, float DeltaTime, FRotator& DeltaRotation
    ) const override;

    virtual bool ShouldComputePerchResult(
        const FHitResult& InHit, bool bCheckRadius = true
    ) const override;

    virtual bool ComputePerchResult(
        const float TestRadius,
        const FHitResult& InHit,
        const float InMaxFloorDist,
        FFindFloorResult& OutPerchFloorResult
    ) const override;

    virtual bool FloorSweepTest(
        struct FHitResult& OutHit,
        const FVector& Start,
        const FVector& End,
        ECollisionChannel TraceChannel,
        const struct FCollisionShape& CollisionShape,
        const struct FCollisionQueryParams& Params,
        const struct FCollisionResponseParams& ResponseParam
    ) const;

    virtual void PhysicsRotation(float DeltaTime)override;

    virtual void PhysNavWalking(float deltaTime, int32 Iterations)override;

    virtual void PhysWalking(float deltaTime, int32 Iterations)override;

    virtual void ComputeFloorDist(
        const FVector& CapsuleLocation, float LineDistance, float SweepDistance,
        FFindFloorResult& OutFloorResult, float SweepRadius, const FHitResult* DownwardSweepResult = NULL
    ) const override;

private:

    float VelocityScale = 1.f;

    // 通过这个向量获取连续的Transform
    FVector DesiredDirection = FVector::ForwardVector;

};