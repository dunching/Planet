
#include "SceneUnitExtendInfo.h"

#include <Kismet/GameplayStatics.h>

#include "GameInstance/PlanetGameInstance.h"
#include "Planet.h"
#include "PlanetWorldSettings.h"
#include "StateTagExtendInfo.h"

USceneUnitExtendInfoMap::USceneUnitExtendInfoMap() :
	Super()
{
}

void USceneUnitExtendInfoMap::PostCDOContruct()
{
	Super::PostCDOContruct();

	InitialData();

	if (DataTable_Unit)
	{
		FTableRowUnit SceneUnitExtendInfoBase;
		DataTable_Unit.Get()->AddRow(TEXT("{"), SceneUnitExtendInfoBase);
	}
}

USceneUnitExtendInfoMap* USceneUnitExtendInfoMap::GetInstance()
{
	auto WorldSetting = Cast<APlanetWorldSettings>(GetWorldImp()->GetWorldSettings());
	return WorldSetting->GetSceneUnitExtendInfoMap();
}

FTableRowUnit* USceneUnitExtendInfoMap::GetTableRowUnit(FGameplayTag UnitType) const
{
	auto DataTablePtr = DataTable_Unit.LoadSynchronous();

	auto SceneUnitExtendInfoPtr = DataTablePtr->FindRow<FTableRowUnit>(*UnitType.ToString(), TEXT("GetUnit"));

	return SceneUnitExtendInfoPtr;
}

FTableRowUnit_TagExtendInfo* USceneUnitExtendInfoMap::GetTableRowUnit_TagExtendInfo(FGameplayTag UnitType) const
{
	auto DataTablePtr = DataTable_TagExtendInfo.LoadSynchronous();

	auto SceneUnitExtendInfoPtr = DataTablePtr->FindRow<FTableRowUnit_TagExtendInfo>(*UnitType.ToString(), TEXT("GetUnit"));

	return SceneUnitExtendInfoPtr;
}

void USceneUnitExtendInfoMap::InitialData()
{
}

