
#include "PlanetWorldSettings.h"

#include "AssetRefMap.h"
#include "StateTagExtendInfo.h"
#include "SceneProxyExtendInfo.h"
#include "GameOptions.h"

UAssetRefMap* APlanetWorldSettings::GetAssetRefMapInstance()
{
	if (!AssetRefMapPtr)
	{
		AssetRefMapPtr = NewObject<UAssetRefMap>(GetWorld(), AssetRefMapClass);
	}

	return AssetRefMapPtr;
}

USceneProxyExtendInfoMap* APlanetWorldSettings::GetSceneProxyExtendInfoMap()
{
	if (!SceneProxyExtendInfoMapPtr)
	{
		SceneProxyExtendInfoMapPtr = NewObject<USceneProxyExtendInfoMap>(GetWorld(), SceneProxyExtendInfoMapPtrClass);
	}

	return SceneProxyExtendInfoMapPtr;
}

UGameOptions* APlanetWorldSettings::GetGameOptions()
{
	if (!GameOptionsPtr)
	{
		GameOptionsPtr = NewObject<UGameOptions>(GetWorld(), GameOptionsClass);
	}

	return GameOptionsPtr;
}
