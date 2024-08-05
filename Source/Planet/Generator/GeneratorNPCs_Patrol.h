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
	
protected:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TObjectPtr<USplineComponent> SplineComponentPtr;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	int32 SampleNum = 3;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	int32 Offset = 100;

};
