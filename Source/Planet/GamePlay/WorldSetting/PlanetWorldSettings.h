// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PlanetWorldSettings.generated.h"

class UAssetRefMap;
class UStateTagExtendInfoMap;
class USceneProxyExtendInfoMap;
class UGameOptions;
class AGuideActor;
class AGuideThread_Main;
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
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GuideThread")
	TArray<TSubclassOf<AGuideThread_Main>> MainGuideThreadChaptersAry;

	/**
	 * 提示[未完待续]的任务引导
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GuideThread")
	TSubclassOf<AGuideThread_Main> ToBeContinueGuideThread;

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
