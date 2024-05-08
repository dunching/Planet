
#include "AssetRefMap.h"

#include "GameInstance/PlanetGameInstance.h"
#include "AssetRefrencePath.h"
#include "Planet.h"

UAssetRefMap::UAssetRefMap():
	Super()
{

}

UAssetRefMap* UAssetRefMap::GetInstance()
{
	return GGameInstancePtr->GetAssetRefMapInstance();
}

FString GetVoxelWorldSlot()
{
	return TEXT("PlanetVoxelWorld");
}
