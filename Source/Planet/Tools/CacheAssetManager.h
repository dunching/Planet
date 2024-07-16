// Zowee. All Rights Reserved.
#pragma once

#include <mutex>

#include "CoreMinimal.h"

#include <GenerateType.h>
#include <GameInstance/PlanetGameInstance.h>
#include "StateTagExtendInfo.h"
#include "AssetRefMap.h"
#include <SceneElement.h>
#include "HoldingItemsComponent.h"

#include "CacheAssetManager.generated.h"

class UWorld;

struct FStreamableHandle;

UCLASS()
class PLANET_API UCacheAssetManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UCacheAssetManager();

	static UCacheAssetManager* GetInstance();

	virtual void Initialize(FSubsystemCollectionBase& Collection)override;

	virtual void Deinitialize()override;

	TSet<EItemSocketType> GetSocketsByType(EToolUnitType NewItemType);

	UClass* GetClassByItemType(EWeaponUnitType NewItemType);

	UClass* GetClassByItemType(EToolUnitType NewItemType);

	UClass* GetClassByItemType(ERawMaterialsType NewItemType);

	UClass* GetClassByItemType(EBuildingsType NewItemType);

	//UClass* GetClassByItemType(FSceneTool NewItemType);

private:

};