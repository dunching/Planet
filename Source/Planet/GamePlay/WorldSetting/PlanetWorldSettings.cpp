
#include "PlanetWorldSettings.h"

#include "AssetRefMap.h"
#include "StateTagExtendInfo.h"
#include "SceneUnitExtendInfo.h"
#include "GameOptions.h"

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

USceneUnitExtendInfoMap* APlanetWorldSettings::GetSceneUnitExtendInfoMap()
{
	if (!SceneUnitExtendInfoMapPtr)
	{
		SceneUnitExtendInfoMapPtr = NewObject<USceneUnitExtendInfoMap>(GetWorld(), SceneUnitExtendInfoMapPtrClass);
	}

	return SceneUnitExtendInfoMapPtr;
}

UGameOptions* APlanetWorldSettings::GetGameOptions()
{
	if (!GameOptionsPtr)
	{
		GameOptionsPtr = NewObject<UGameOptions>(GetWorld(), GameOptionsClass);
	}

	return GameOptionsPtr;
}
