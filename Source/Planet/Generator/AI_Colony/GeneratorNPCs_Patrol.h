// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GeneratorBase.h"

#include "GeneratorNPCs_Patrol.generated.h"

class UPlanetChildActorComponent;
class USplineComponent;
class ACharacterBase;

/**
 * NPC会沿着这条样条线巡逻
 */
UCLASS()
class PLANET_API AGeneratorNPCs_Patrol : public AGeneratorBase
{
	GENERATED_BODY()

public:

	AGeneratorNPCs_Patrol(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	bool CheckIsFarawayOriginal(ACharacterBase*TargetCharacterPtr) const;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Spline")
	TObjectPtr<USplineComponent> SplineComponentPtr;
	
	UPROPERTY(EditAnywhere, Category = Parameter)
	int32 MaxDistance = 800;

protected:

};
