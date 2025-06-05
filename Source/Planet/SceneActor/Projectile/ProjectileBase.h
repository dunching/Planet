// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "SceneActor.h"

#include "ProjectileBase.generated.h"

class USphereComponent;
class USceneComponent;
class UProjectileMovementComponent;
class ACharacterBase;

UCLASS()
class PLANET_API AProjectileBase : public ASceneActor
{
	GENERATED_BODY()

public:

	AProjectileBase(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

#ifdef WITH_EDITOR
#endif
	virtual void Tick(float DeltaSeconds)override;
	
	UFUNCTION(NetMulticast, Reliable)
	void SetHomingTarget(ACharacterBase* TargetPtr);

	UFUNCTION(NetMulticast, Reliable)
	void SetSpeed(float Speed);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Projectile")
	USphereComponent* CollisionComp = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Projectile")
	UStaticMeshComponent* StaticMeshCompPtr = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement")
	UProjectileMovementComponent* ProjectileMovementCompPtr = nullptr;
	
	UPROPERTY(Replicated)
	int32 MaxMoveRange = -1;

protected:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FVector StartPt = FVector::ZeroVector;

};
