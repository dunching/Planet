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

    // RootMotion����AinMotionʱ�������赲
    FHitResult PerformBlockResult;

    const FLyraCharacterGroundInfo& GetGroundInfo();

    // ֹͣPwan��ת�����루����״̬���ƣ��磺ѣ�Σ���̣�
    bool bSkipPerformMovement = false;
    
protected:

    FLyraCharacterGroundInfo CachedGroundInfo;

    virtual void PhysicsRotation(float DeltaTime)override;

    virtual void CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration)override;

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

#if USECUSTOMEGRAVITY
    UGravityMovementComponent(const FObjectInitializer& ObjectInitializer);

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

    // ͨ�����������ȡ������Transform
    FVector PreviousGravityTransformForward = FVector::ForwardVector;
#endif

};