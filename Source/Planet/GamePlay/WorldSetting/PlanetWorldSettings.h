// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PlanetWorldSettings.generated.h"

class UAssetRefMap;

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

	UAssetRefMap* GetAssetRefMapInstance();

protected:

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly)
		UAssetRefMap* AssetRefMapPtr = nullptr;
		
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TSubclassOf<UAssetRefMap>AssetRefMapClass;

};
