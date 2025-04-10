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
class UPAD_RewardsItems;

/**
 *
 */
UCLASS()
class PLANET_API APlanetWorldSettings : public AWorldSettings
{
	GENERATED_BODY()

public:

	UPAD_RewardsItems* GetTableRow_RewardsTD()const;

	UAssetRefMap* GetAssetRefMapInstance()const;

	USceneProxyExtendInfoMap* GetSceneProxyExtendInfoMap()const;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GuideLine")
	TSubclassOf<AGuideMainThread>MainLineGuideClass;

protected:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSoftObjectPtr<UPAD_RewardsItems>TableRow_RewardsTDRef;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSoftObjectPtr<UAssetRefMap>AssetRefMapRef;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSoftObjectPtr<USceneProxyExtendInfoMap>SceneProxyExtendInfoMapPtr;
	
	UPROPERTY(Transient)
	UGameOptions* GameOptionsPtr = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<UGameOptions>GameOptionsClass;

};
