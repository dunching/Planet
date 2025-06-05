// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "SplineMesh.generated.h"

class USplineComponent;
class USplineMeshComponent;

/**
 *
 */
UCLASS()
class PLANET_API ASplineMesh : public AActor
{
	GENERATED_BODY()

public:
	ASplineMesh(
		const FObjectInitializer& ObjectInitializer
		);

	virtual void OnConstruction(
		const FTransform& Transform
		) override;

	virtual void Tick(float DeltaSeconds) override;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TObjectPtr<USplineComponent> SplineComponentPtr;
	
protected:
	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category=Collision
	)
	FName CollisionProfileName;

	UPROPERTY(
		VisibleAnywhere,
		BlueprintReadOnly
	)
	TArray<USplineMeshComponent*> SplineMeshComponentsAry;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category=Collision
	)
	int32 IntervalLength = 1000;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category=Collision
	)
	int32 ScaleZ = 100;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category=Collision
	)
	TSoftObjectPtr<UStaticMesh> StaticMeshRef;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category=Collision
	)
	TSoftObjectPtr<UMaterial> MaterialRef;
};

/**
 * 区域空气墙
 */
UCLASS()
class PLANET_API ARegionAirWall : public AActor
{
	GENERATED_BODY()

public:

	ARegionAirWall(
		const FObjectInitializer& ObjectInitializer
		);

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TObjectPtr<USplineComponent> SplineComponentPtr = nullptr;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FGameplayTag RegionTag;
	
private:

	UFUNCTION()
	void OnEntryRegion(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
		);

	UFUNCTION()
	void OnLeaveRegion(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
		);

	UPROPERTY(Transient)
	TObjectPtr<UAudioComponent>AudioComponentPtr = nullptr;
};
