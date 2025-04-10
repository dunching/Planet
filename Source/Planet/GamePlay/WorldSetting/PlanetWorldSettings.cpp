
#include "PlanetWorldSettings.h"

#include "AssetRefMap.h"
#include "RewardsTD.h"
#include "SceneProxyExtendInfo.h"
#include "GameOptions.h"

UPAD_RewardsItems* APlanetWorldSettings::GetTableRow_RewardsTD() const
{
	return TableRow_RewardsTDRef.LoadSynchronous();
}

UAssetRefMap* APlanetWorldSettings::GetAssetRefMapInstance()const
{
	return AssetRefMapRef.LoadSynchronous();
}

USceneProxyExtendInfoMap* APlanetWorldSettings::GetSceneProxyExtendInfoMap()const
{
	return SceneProxyExtendInfoMapPtr.LoadSynchronous();
}
