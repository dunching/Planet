
#include "PlanetWorldSettings.h"

#include "AssetRefMap.h"

UAssetRefMap* APlanetWorldSettings::GetAssetRefMapInstance()
{
	if (!AssetRefMapPtr)
	{
		AssetRefMapPtr = NewObject<UAssetRefMap>(GetWorld(), AssetRefMapClass);
	}

	return AssetRefMapPtr;
}
