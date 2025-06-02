
#include "SceneProxyExtendInfo.h"

#include <Kismet/GameplayStatics.h>

#include "GameplayTagsManager.h"
#include "GameInstance/PlanetGameInstance.h"
#include "Tools.h"
#include "PlanetWorldSettings.h"
#include "StateTagExtendInfo.h"
#include "TalentInfo.h"
#include "Regions.h"

FTableRowProxy_CommonCooldownInfo* GetTableRowProxy_CommonCooldownInfo(
	const FGameplayTag& CommonCooldownTag
	)
{
	auto SceneProxyExtendInfoMapPtr = USceneProxyExtendInfoMap::GetInstance();
	auto DataTable = SceneProxyExtendInfoMapPtr->DataTable_CommonCooldownInfo.LoadSynchronous();

	auto SceneProxyExtendInfoPtr = DataTable->FindRow<FTableRowProxy_CommonCooldownInfo>(
		 *CommonCooldownTag.ToString(),
		 TEXT("GetProxy")
		);
	return SceneProxyExtendInfoPtr;
}

USceneProxyExtendInfoMap::USceneProxyExtendInfoMap() :
	Super()
{
}

void USceneProxyExtendInfoMap::PostCDOContruct()
{
	Super::PostCDOContruct();

	InitialData();
}

USceneProxyExtendInfoMap* USceneProxyExtendInfoMap::GetInstance()
{
	auto WorldSetting = Cast<APlanetWorldSettings>(GetWorldImp()->GetWorldSettings());
	return WorldSetting->GetSceneProxyExtendInfoMap();
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

FTableRow_Regions* USceneProxyExtendInfoMap::GetTableRow_Region(
	FGameplayTag UnitType
	) const
{
	auto DataTablePtr = DataTable_Regions.LoadSynchronous();

	auto RegionsPtr = DataTablePtr->FindRow<FTableRow_Regions>(*UnitType.ToString(), TEXT("GetUnit"));
	if (RegionsPtr)
	{
		return RegionsPtr;
	}

	// 默认的
	TArray<FTableRow_Regions*> OutRowArray;
	DataTablePtr->GetAllRows<FTableRow_Regions>(TEXT("GetUnit"),OutRowArray);
	if (OutRowArray.IsValidIndex(0))
	{
		return OutRowArray[0];
	}

	return nullptr;
}

TArray<FTableRow_Regions*> USceneProxyExtendInfoMap::GetTableRow_AllRegions() const
{
	auto DataTablePtr = DataTable_Regions.LoadSynchronous();

	TArray<FTableRow_Regions*> Result;
	DataTablePtr->GetAllRows<FTableRow_Regions>(TEXT("GetUnit"),Result);

	return Result;
}

TArray<FTableRowProxy_CharacterGrowthAttribute*> USceneProxyExtendInfoMap::GetTableRow_CharacterGrowthAttribute() const
{
	auto DataTablePtr = DataTable_Character_GrowthAttribute.LoadSynchronous();

	TArray<FTableRowProxy_CharacterGrowthAttribute*> Result;
	DataTablePtr->GetAllRows<FTableRowProxy_CharacterGrowthAttribute>(TEXT("GetUnit"),Result);

	return Result;
}

void USceneProxyExtendInfoMap::InitialData()
{
}

