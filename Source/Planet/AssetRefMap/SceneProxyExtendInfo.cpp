
#include "SceneProxyExtendInfo.h"

#include <Kismet/GameplayStatics.h>

#include "GameInstance/PlanetGameInstance.h"
#include "Planet.h"
#include "PlanetWorldSettings.h"
#include "StateTagExtendInfo.h"
#include "TalentInfo.h"
#include "CharactersInfo.h"

USceneProxyExtendInfoMap::USceneProxyExtendInfoMap() :
	Super()
{
}

void USceneProxyExtendInfoMap::PostCDOContruct()
{
	Super::PostCDOContruct();

	InitialData();

	if (DataTable_Proxy)
	{
		FTableRowProxy SceneUnitExtendInfoBase;
		DataTable_Proxy.Get()->AddRow(TEXT("{"), SceneUnitExtendInfoBase);
	}
}

USceneProxyExtendInfoMap* USceneProxyExtendInfoMap::GetInstance()
{
	auto WorldSetting = Cast<APlanetWorldSettings>(GetWorldImp()->GetWorldSettings());
	return WorldSetting->GetSceneProxyExtendInfoMap();
}

FTableRowProxy* USceneProxyExtendInfoMap::GetTableRowProxy(FGameplayTag UnitType) const
{
	auto DataTablePtr = DataTable_Proxy.LoadSynchronous();

	auto SceneUnitExtendInfoPtr = DataTablePtr->FindRow<FTableRowProxy>(*UnitType.ToString(), TEXT("GetUnit"));

	return SceneUnitExtendInfoPtr;
}

FTableRowProxy_TagExtendInfo* USceneProxyExtendInfoMap::GetTableRowProxy_TagExtendInfo(FGameplayTag UnitType) const
{
	auto DataTablePtr = DataTable_TagExtendInfo.LoadSynchronous();

	auto SceneUnitExtendInfoPtr = DataTablePtr->FindRow<FTableRowProxy_TagExtendInfo>(*UnitType.ToString(), TEXT("GetUnit"));

	return SceneUnitExtendInfoPtr;
}

const UPAD_Talent_Property* USceneProxyExtendInfoMap::GetTalent_Property(EPointPropertyType PointPropertyType) const
{
	if (PAD_Talent_PropertyMap.Contains(PointPropertyType))
	{
		return PAD_Talent_PropertyMap[PointPropertyType];
	}

	return nullptr;
}

void USceneProxyExtendInfoMap::InitialData()
{
}

