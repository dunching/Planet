
#include "CacheAssetManager.h"

#include <string>

#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include <Kismet/GameplayStatics.h>
#include <Subsystems/SubsystemBlueprintLibrary.h>

#include "GameInstance/ShiYuGameInstance.h"
#include "SceneObj/Equipment/EquipmentHand.h"
#include "SceneObj/Equipment/EquipmentKnife.h"
#include "ArticleStone.h"
#include "ArticleTree.h"
#include "SceneObj/Equipment/EquipmentGun.h"
#include "SceneObj/Equipment/EquipmentDogs.h"
#include "SceneObj/StageProperty/AutomaticFort.h"
#include "SceneObj/Track/TrackBase.h"
#include "SceneObj/Track/TrackVehicleBase.h"
#include "SceneObj/Building/Wall/WallBase.h"
#include "SceneObj/Building/Ground/GroundBase.h"
#include "AssetRefMap.h"
#include "ShiYu.h"

UCacheAssetManager::UCacheAssetManager() :
	Super()
{

}

UCacheAssetManager* UCacheAssetManager::GetInstance()
{
	return Cast<UCacheAssetManager>(USubsystemBlueprintLibrary::GetGameInstanceSubsystem(
		GetWorldImp(), UCacheAssetManager::StaticClass()
	));
}

void UCacheAssetManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UCacheAssetManager::Deinitialize()
{
	Super::Deinitialize();
}

void UCacheAssetManager::GetTextureByItemType(
	FItemNum ItemNum, const FAsyncLoadTextureCB& AsyncLoadTextureCB
)
{
	auto AssetRefMapPtr = UAssetRefMap::GetInstance();
	switch (ItemNum.ItemType.Enumtype)
	{
	case EEnumtype::kEquipment:
	{
		if (AssetRefMapPtr->EquipmentTypeImg.Contains(std::get<EEquipmentType>(ItemNum.ItemType.ItemType)))
		{
			AsyncGetTextureFromThis(AssetRefMapPtr->EquipmentTypeImg[std::get<EEquipmentType>(ItemNum.ItemType.ItemType)], AsyncLoadTextureCB);
		}
	}
	break;
	case EEnumtype::kBuilding:
	{
		if (AssetRefMapPtr->BuildingTypeImg.Contains(std::get<EBuildingType>(ItemNum.ItemType.ItemType)))
		{
			AsyncGetTextureFromThis(AssetRefMapPtr->BuildingTypeImg[std::get<EBuildingType>(ItemNum.ItemType.ItemType)], AsyncLoadTextureCB);
		}
	}
	break;
	case EEnumtype::kRawMaterialType:
	{
		if (AssetRefMapPtr->RawMaterialImg.Contains(std::get<ERawMaterialType>(ItemNum.ItemType.ItemType)))
		{
			AsyncGetTextureFromThis(AssetRefMapPtr->RawMaterialImg[std::get<ERawMaterialType>(ItemNum.ItemType.ItemType)], AsyncLoadTextureCB);
		}
	}
	break;
	default:
		break;
	}
}

TSet<EItemSocketType> UCacheAssetManager::GetSocketsByType(EEquipmentType NewItemType)
{
	TSet<EItemSocketType> Result;

	switch (NewItemType)
	{
	case EEquipmentType::kNone:
	default:
	{
		Result.Add(EItemSocketType::kSkeletalRightHandSock);
	}
	break;
	}

	return Result;
}

UClass* UCacheAssetManager::GetClassByItemType(ERawMaterialType NewItemType)
{
	UClass* ClassPtr = nullptr;
	return ClassPtr;
}

UClass* UCacheAssetManager::GetClassByItemType(EBuildingType NewItemType)
{
	auto AssetRefMapPtr = UAssetRefMap::GetInstance();
	if (AssetRefMapPtr->BuildingClass.Contains(NewItemType))
	{
		return AssetRefMapPtr->BuildingClass[NewItemType];
	}

	return nullptr;
}

UClass* UCacheAssetManager::GetClassByItemType(EEquipmentType NewItemType)
{
	auto AssetRefMapPtr = UAssetRefMap::GetInstance();
	if (AssetRefMapPtr->EquipmentTypeClass.Contains(NewItemType))
	{
		return AssetRefMapPtr->EquipmentTypeClass[NewItemType];
	}

	return nullptr;
}

UClass* UCacheAssetManager::GetClassByItemType(FItemType NewItemType)
{
	switch (NewItemType.Enumtype)
	{
	case EEnumtype::kEquipment:
	{
		return GetClassByItemType(std::get<EEquipmentType>(NewItemType.ItemType));
	}
	break;
	case EEnumtype::kBuilding:
	{
		return GetClassByItemType(std::get<EBuildingType>(NewItemType.ItemType));
	}
	break;
	case EEnumtype::kRawMaterialType:
	{
		return GetClassByItemType(std::get<ERawMaterialType>(NewItemType.ItemType));
	}
	break;
	default:
	{
		return nullptr;
	}
	break;
	}
	return nullptr;
}

UClass* UCacheAssetManager::GetClassByItemType(FItemNum NewItemType)
{
	return GetClassByItemType(NewItemType.ItemType);
}

void UCacheAssetManager::AsyncGetTextureFromThis(
	TSoftObjectPtr<UTexture2D>& ImgSoft,
	const FAsyncLoadTextureCB& AsyncLoadTextureCB
)
{
	FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();

	StreamableManager.RequestAsyncLoad(ImgSoft.ToSoftObjectPath(), [=]()
		{
			// 需要改进
			AsyncLoadTextureCB(ImgSoft.Get());
		});
}
