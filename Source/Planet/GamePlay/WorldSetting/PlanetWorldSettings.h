// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PlanetWorldSettings.generated.h"

class UAssetRefMap;
class UStateTagExtendInfoMap;
class USceneUnitExtendInfoMap;

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

	UStateTagExtendInfoMap* GetStateTagExtendInfoInstance();
	
	USceneUnitExtendInfoMap* GetSceneUnitExtendInfoMap();

protected:

	UPROPERTY(Transient)
	UAssetRefMap* AssetRefMapPtr = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<UAssetRefMap>AssetRefMapClass;
	
	UPROPERTY(Transient)
	UStateTagExtendInfoMap* StateExtendInfoPtr = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<UStateTagExtendInfoMap>StateExtendInfoClass;
	
	UPROPERTY(Transient)
	USceneUnitExtendInfoMap* SceneUnitExtendInfoMapPtr = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<USceneUnitExtendInfoMap>SceneUnitExtendInfoMapPtrClass;

};
