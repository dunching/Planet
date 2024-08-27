// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PlanetWorldSettings.generated.h"

class UAssetRefMap;
class UStateTagExtendInfoMap;
class USceneUnitExtendInfoMap;
class UGameOptions;

/**
 *
 */
UCLASS()
class PLANET_API APlanetWorldSettings : public AWorldSettings
{
	GENERATED_BODY()

public:

	UAssetRefMap* GetAssetRefMapInstance();

	USceneUnitExtendInfoMap* GetSceneUnitExtendInfoMap();
	
	UGameOptions* GetGameOptions();

protected:

	UPROPERTY(Transient)
	UAssetRefMap* AssetRefMapPtr = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<UAssetRefMap>AssetRefMapClass;
	
	UPROPERTY(Transient)
	USceneUnitExtendInfoMap* SceneUnitExtendInfoMapPtr = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<USceneUnitExtendInfoMap>SceneUnitExtendInfoMapPtrClass;
	
	UPROPERTY(Transient)
	UGameOptions* GameOptionsPtr = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<UGameOptions>GameOptionsClass;

};
