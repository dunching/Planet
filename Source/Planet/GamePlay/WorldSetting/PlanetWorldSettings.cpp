
#include "PlanetWorldSettings.h"

#include "AssetRefMap.h"
#include "StateTagExtendInfo.h"
#include "SceneProxyExtendInfo.h"
#include "GameOptions.h"

UAssetRefMap* APlanetWorldSettings::GetAssetRefMapInstance()
{
	return AssetRefMapClass.LoadSynchronous();
}

USceneProxyExtendInfoMap* APlanetWorldSettings::GetSceneProxyExtendInfoMap()
{
	return SceneProxyExtendInfoMapPtrClass.LoadSynchronous();
}

UGameOptions* APlanetWorldSettings::GetGameOptions()
{
	if (!GameOptionsPtr)
	{
		GameOptionsPtr = NewObject<UGameOptions>(GetWorld(), GameOptionsClass);
	}

	return GameOptionsPtr;
}
