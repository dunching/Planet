
#include "CacheAssetManager.h"

#include <string>

#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include <Kismet/GameplayStatics.h>
#include <Subsystems/SubsystemBlueprintLibrary.h>
#include "Engine/StreamableManager.h"

#include "GameInstance/PlanetGameInstance.h"
#include "AssetRefMap.h"
#include "Planet.h"

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

TSet<EItemSocketType> UCacheAssetManager::GetSocketsByType(EToolUnitType NewItemType)
{
	TSet<EItemSocketType> Result;

	switch (NewItemType)
	{
	case EToolUnitType::kNone:
	default:
	{
		Result.Add(EItemSocketType::kSkeletalRightHandSock);
	}
	break;
	}

	return Result;
}

UClass* UCacheAssetManager::GetClassByItemType(ERawMaterialsType NewItemType)
{
	UClass* ClassPtr = nullptr;
	return ClassPtr;
}

UClass* UCacheAssetManager::GetClassByItemType(EBuildingsType NewItemType)
{
	auto AssetRefMapPtr = UAssetRefMap::GetInstance();
// 	if (AssetRefMapPtr->BuildingsClass.Contains(NewItemType))
// 	{
// 		return AssetRefMapPtr->BuildingsClass[NewItemType];
// 	}

	return nullptr;
}

UClass* UCacheAssetManager::GetClassByItemType(EToolUnitType NewItemType)
{
	auto AssetRefMapPtr = UAssetRefMap::GetInstance();
// 	if (AssetRefMapPtr->EquipmentsTypeClass.Contains(NewItemType))
// 	{
// 		return AssetRefMapPtr->EquipmentsTypeClass[NewItemType];
// 	}

	return nullptr;
}

UClass* UCacheAssetManager::GetClassByItemType(EWeaponUnitType NewItemType)
{
	auto AssetRefMapPtr = UAssetRefMap::GetInstance();
// 	if (AssetRefMapPtr->WeaponsClass.Contains(NewItemType))
// 	{
// 		return AssetRefMapPtr->WeaponsClass[NewItemType];
// 	}

	return nullptr;
}