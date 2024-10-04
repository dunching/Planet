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

    // RootMotion或者AinMotion时被物体阻挡
    FHitResult PerformBlockResult;

    UGravityMovementComponent(const FObjectInitializer& ObjectInitializer);

    const FLyraCharacterGroundInfo& GetGroundInfo();

    // 
    UPROPERTY(Replicated)
    bool bSkip_Rotation = false;
    
    // 
    UPROPERTY(Replicated)
    bool bSkip_RootMotion = false;
    
    // 
    UPROPERTY(Replicated)
    bool bSkip_PlayerInput = false;
    
    // 
    UPROPERTY(Replicated)
    bool bSkip_PathFollow = false;
    
    // 
    UPROPERTY(Replicated)
    bool bForceRotation = false;
    
	UFUNCTION(NetMulticast, Reliable)
	void SetIsOrientRotationToMovement_RPC(bool bIsOrientRotationToMovement);
	
protected:

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    virtual void PhysicsRotation(float DeltaTime)override;

    virtual FVector ConsumeInputVector()override;

    virtual void CalcVelocity(
        float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration
    )override;

    virtual void ApplyRootMotionToVelocity(float deltaTime)override;

    virtual bool ApplyRequestedMove(
        float DeltaTime, 
        float MaxAccel,
        float MaxSpeed,
        float Friction, 
        float BrakingDeceleration, 
        FVector& OutAcceleration,
        float& OutRequestedSpeed
    )override;

    virtual void PerformMovement(float DeltaTime)override;

    virtual void StartNewPhysics(float DeltaTime, int32 Iterations)override;

    virtual void HandleImpact(
        const FHitResult& Hit, float TimeSlice = 0.f, const FVector& MoveDelta = FVector::ZeroVector
	) override;

    virtual void TickComponent(
        float DeltaTime,
        enum ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction
    ) override;

    FRotator ComputeRootMotionToMovementRotation(const FRotator& CurrentRotation, float DeltaTime, FRotator& DeltaRotation) const;

    FLyraCharacterGroundInfo CachedGroundInfo;

#if USECUSTOMEGRAVITY
    virtual void BeginPlay() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason)override;

    virtual void ApplyRootMotionToVelocity(float deltaTime)override;

    virtual void SetGravityDirection(const FVector& GravityDir)override;

    virtual void PhysNavWalking(float deltaTime, int32 Iterations)override;

    virtual void PhysWalking(float deltaTime, int32 Iterations)override;

    virtual float SlideAlongSurface(
        const FVector& Delta, float Time, const FVector& Normal, FHitResult& Hit, bool bHandleImpact
    ) override;

protected:

    bool UpdateGravityTransform(float Delta);

	float Frequency = 1.f;

    FTSTicker::FDelegateHandle TickDelegateHandle;

    // 通过这个向量获取连续的Transform
    FVector PreviousGravityTransformForward = FVector::ForwardVector;
#endif

};