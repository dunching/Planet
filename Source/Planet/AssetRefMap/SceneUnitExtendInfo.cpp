
#include "SceneUnitExtendInfo.h"

#include <Kismet/GameplayStatics.h>

#include "GameInstance/PlanetGameInstance.h"
#include "Planet.h"
#include "PlanetWorldSettings.h"
#include "StateTagExtendInfo.h"
#include "TalentInfo.h"

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

FTableRowUnit_AICharacter_Allocation* USceneUnitExtendInfoMap::GetTableRowUnit_AICharacter_Allocation(FGameplayTag UnitType) const
{
	auto DataTablePtr = DataTable_AICharacter_Allocation.LoadSynchronous();

	auto SceneUnitExtendInfoPtr = DataTablePtr->FindRow<FTableRowUnit_AICharacter_Allocation>(*UnitType.ToString(), TEXT("GetUnit"));

	return SceneUnitExtendInfoPtr;
}

const UPAD_Talent_Property* USceneUnitExtendInfoMap::GetTalent_Property(EPointPropertyType PointPropertyType) const
{
	if (PAD_Talent_PropertyMap.Contains(PointPropertyType))
	{
		return PAD_Talent_PropertyMap[PointPropertyType];
	}

	return nullptr;
}

void USceneUnitExtendInfoMap::InitialData()
{
}

