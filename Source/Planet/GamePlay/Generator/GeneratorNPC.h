// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GeneratorBase.h"

#include "GeneratorNPC.generated.h"

class UPlanetChildActorComponent;

/**
 *
 */
UCLASS()
class PLANET_API AGeneratorNPC : public AGeneratorBase
{
	GENERATED_BODY()

public:

	AGeneratorNPC(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

};
