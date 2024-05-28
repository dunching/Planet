// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PlanetWorldSettings.generated.h"

/**
 *
 */
UCLASS()
class PLANET_API APlanetWorldSettings : public AWorldSettings
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Skill Settings")
	int32 ResetCooldownTime = 1;

};
