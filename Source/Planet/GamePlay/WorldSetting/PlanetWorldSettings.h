// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GetModifyItemProxyStrategiesInterface.h"
#include "GuideSubSystem.h"
#include "PAD_ItemProxyCollection.h"

#include "PlanetWorldSettings.generated.h"

class UAssetRefMap;
class UStateTagExtendInfoMap;
class USceneProxyExtendInfoMap;
class UGameOptions;
class AGuideActor;
class AGuideThread_Main;
class AGuideThread_MainBase;
class UPAD_RewardsItems;
class UPAD_ItemProxyCollection;

/**
 *
 */
UCLASS()
class PLANET_API APlanetWorldSettings :
	public AWorldSettings,
	public IGetItemProxyCollectionInterface,
	public IGetModifyItemProxyStrategies,
	public IGetGuideSubSystemInterface
{
	GENERATED_BODY()

public:

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	UPAD_RewardsItems* GetTableRow_RewardsTD()const;

	UAssetRefMap* GetAssetRefMapInstance()const;

	USceneProxyExtendInfoMap* GetSceneProxyExtendInfoMap()const;

	virtual UGuideSubSystem* GetGuideSubSystem()const override;
	
	virtual const UPAD_ItemProxyCollection*GetItemProxyCollection()const override;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GuideThread")
	TArray<TSubclassOf<AGuideThread_MainBase>> MainGuideThreadChaptersAry;

	/**
	 * 提示[未完待续]的任务引导
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GuideThread")
	TSubclassOf<AGuideThread_MainBase> ToBeContinueGuideThread;

protected:

	virtual void InitialModifyItemProxyStrategies() override;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSoftObjectPtr<UPAD_RewardsItems>TableRow_RewardsTDRef;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSoftObjectPtr<UAssetRefMap>AssetRefMapRef;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSoftObjectPtr<UPAD_ItemProxyCollection>PAD_ItemProxyCollectionRef;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSoftObjectPtr<USceneProxyExtendInfoMap>SceneProxyExtendInfoMapPtr;
	
};
