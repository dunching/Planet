// Zowee. All Rights Reserved.
#pragma once

#include <mutex>

#include "CoreMinimal.h"

#include <GenerateType.h>
#include <GameInstance/ShiYuGameInstance.h>
#include "AssetRefrencePath.h"
#include "AssetRefMap.h"
#include <ItemType.h>

#include "CacheAssetManager.generated.h"

class UWorld;

UCLASS()
class SHIYU_API UCacheAssetManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UCacheAssetManager();

	static UCacheAssetManager* GetInstance();

	virtual void Initialize(FSubsystemCollectionBase& Collection)override;

	virtual void Deinitialize()override;

	using FAsyncLoadTextureCB = std::function<void(UTexture2D*)>;

	void GetTextureByItemType(
		FItemNum ItemNum,
		const FAsyncLoadTextureCB& AsyncLoadTextureCB
	);


	TSet<EItemSocketType> GetSocketsByType(EEquipmentType NewItemType);

	UClass* GetClassByItemType(EEquipmentType NewItemType);

	UClass* GetClassByItemType(ERawMaterialType NewItemType);

	UClass* GetClassByItemType(EBuildingType NewItemType);

	UClass* GetClassByItemType(FItemType NewItemType);

	UClass* GetClassByItemType(FItemNum NewItemType);

private:

	void AsyncGetTextureFromThis(
		TSoftObjectPtr<UTexture2D>& ImgSoft,
		const FAsyncLoadTextureCB& AsyncLoadTextureCB
	);

	std::mutex LoadCachAssetMutex;

};