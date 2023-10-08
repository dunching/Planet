
#include "AssetRefMap.h"

#include "GameInstance/ShiYuGameInstance.h"
#include "AssetRefrencePath.h"
#include "ShiYu.h"

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
	return TEXT("ShiYuVoxelWorld");
}
