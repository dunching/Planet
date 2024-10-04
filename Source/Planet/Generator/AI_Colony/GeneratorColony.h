// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GeneratorBase.h"

#include "GeneratorColony.generated.h"

class UPlanetChildActorComponent;
class USplineComponent;

UCLASS()
class PLANET_API UFormationComponent : public USceneComponent
{
	GENERATED_BODY()

public:

	UFormationComponent(const FObjectInitializer& ObjectInitializer);

};

/**
 *
 */
UCLASS()
class PLANET_API AGeneratorColony : public AGeneratorBase
{
	GENERATED_BODY()

public:

	AGeneratorColony(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds)override;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TObjectPtr<USplineComponent> SplineComponentPtr = nullptr;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TObjectPtr<UFormationComponent> FormationComponentPtr = nullptr;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 ForwardSpeed = 250;

	int32 CurrentLength = 0;

};
