// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SceneObj.h"
#include "ProjectileBase.generated.h"

class USphereComponent;
class USceneComponent;
class UProjectileMovementComponent;

UCLASS()
class SCENEOBJECTS_API AProjectileBase : public ASceneObj
{
	GENERATED_BODY()

public:

	AProjectileBase(const FObjectInitializer& ObjectInitializer);

#ifdef WITH_EDITOR
	virtual void Tick(float DeltaSeconds)override;
#endif

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Projectile")
	USphereComponent* CollisionComp = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Projectile")
	UStaticMeshComponent* StaticMeshCompPtr = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement")
	UProjectileMovementComponent* ProjectileMovementCompPtr = nullptr;

protected:

};
