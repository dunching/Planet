// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SceneObj.h"
#include "ProjectileBase.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class UProjectileMovementComponent;

UCLASS()
class PLANET_API AProjectileBase : public ASceneObj
{
	GENERATED_BODY()

public:

    AProjectileBase(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Projectile")
		USceneComponent* SceneCompPtr = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Projectile")
		UStaticMeshComponent* StaticMeshCompPtr = nullptr;

#ifdef WITH_EDITOR
	virtual void Tick(float DeltaSeconds)override;
#endif

protected:

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement")
        UProjectileMovementComponent* ProjectileMovementCompPtr = nullptr;

};
