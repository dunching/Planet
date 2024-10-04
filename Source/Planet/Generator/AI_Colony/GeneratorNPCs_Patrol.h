// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GeneratorBase.h"

#include "GeneratorNPCs_Patrol.generated.h"

class UPlanetChildActorComponent;
class USplineComponent;

/**
 *
 */
UCLASS()
class PLANET_API AGeneratorNPCs_Patrol : public AGeneratorBase
{
	GENERATED_BODY()

public:

	AGeneratorNPCs_Patrol(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Spline")
	TObjectPtr<USplineComponent> SplineComponentPtr;
	
protected:

};
