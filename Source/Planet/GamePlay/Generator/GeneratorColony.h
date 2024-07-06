// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GeneratorBase.h"

#include "GeneratorColony.generated.h"

class UPlanetChildActorComponent;

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

};
