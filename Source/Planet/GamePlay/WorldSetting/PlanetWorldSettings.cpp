
#include "PlanetWorldSettings.h"

#include "AssetRefMap.h"
#include "StateTagExtendInfo.h"

UAssetRefMap* APlanetWorldSettings::GetAssetRefMapInstance()
{
	if (!AssetRefMapPtr)
	{
		AssetRefMapPtr = NewObject<UAssetRefMap>(GetWorld(), AssetRefMapClass);
	}

	return AssetRefMapPtr;
}

UStateTagExtendInfoMap* APlanetWorldSettings::GetStateTagExtendInfoInstance()
{
	if (!StateExtendInfoPtr)
	{
		StateExtendInfoPtr = NewObject<UStateTagExtendInfoMap>(GetWorld(), StateExtendInfoClass);
	}

	return StateExtendInfoPtr;
}
