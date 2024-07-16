
#include "AssetRefMap.h"

#include <Kismet/GameplayStatics.h>

#include "GameInstance/PlanetGameInstance.h"
#include "StateTagExtendInfo.h"
#include "Planet.h"
#include "PlanetWorldSettings.h"

UAssetRefMap::UAssetRefMap():
	Super()
{

}

UAssetRefMap* UAssetRefMap::GetInstance()
{
	auto WorldSetting = Cast<APlanetWorldSettings>(GetWorldImp()->GetWorldSettings());
	return WorldSetting->GetAssetRefMapInstance();
}

FString GetVoxelWorldSlot()
{
	return TEXT("PlanetVoxelWorld");
}
