// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PlanetWorldSettings.generated.h"

class UAssetRefMap;
class UStateTagExtendInfoMap;
class USceneProxyExtendInfoMap;
class UGameOptions;
class AGuideActor;
class AGuideMainThread;

/**
 *
 */
UCLASS()
class PLANET_API APlanetWorldSettings : public AWorldSettings
{
	GENERATED_BODY()

public:

	UAssetRefMap* GetAssetRefMapInstance();

	USceneProxyExtendInfoMap* GetSceneProxyExtendInfoMap();
	
	UGameOptions* GetGameOptions();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GE")
	TSubclassOf<AGuideMainThread>MainLineGuideClass;

protected:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSoftObjectPtr<UAssetRefMap>AssetRefMapClass;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSoftObjectPtr<USceneProxyExtendInfoMap>SceneProxyExtendInfoMapPtrClass;
	
	UPROPERTY(Transient)
	UGameOptions* GameOptionsPtr = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<UGameOptions>GameOptionsClass;

};
