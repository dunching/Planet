
#include "DataTableCollection.h"

#include "GameplayTagsManager.h"
#include "Tools.h"
#include "PlanetWorldSettings.h"
#include "StateTagExtendInfo.h"
#include "TalentInfo.h"
#include "Regions.h"
#include "RewardsTD.h"

FTableRowProxy_CommonCooldownInfo* GetTableRowProxy_CommonCooldownInfo(
	const FGameplayTag& CommonCooldownTag
	)
{
	auto SceneProxyExtendInfoMapPtr = UDataTableCollection::GetInstance();
	auto DataTable = SceneProxyExtendInfoMapPtr->DataTable_CommonCooldownInfo.LoadSynchronous();

	auto SceneProxyExtendInfoPtr = DataTable->FindRow<FTableRowProxy_CommonCooldownInfo>(
		 *CommonCooldownTag.ToString(),
		 TEXT("GetProxy")
		);
	return SceneProxyExtendInfoPtr;
}

UDataTableCollection::UDataTableCollection() :
	Super()
{
}

void UDataTableCollection::PostCDOContruct()
{
	Super::PostCDOContruct();

	InitialData();
}

UDataTableCollection* UDataTableCollection::GetInstance()
{
	auto WorldSetting = Cast<APlanetWorldSettings>(GetWorldImp()->GetWorldSettings());
	return WorldSetting->GetSceneProxyExtendInfoMap();
}

const FTableRowProxy_TagExtendInfo* UDataTableCollection::GetTableRowProxy_TagExtendInfo(FGameplayTag UnitType) const
{
	auto DataTablePtr = DataTable_TagExtendInfo.LoadSynchronous();

	auto SceneUnitExtendInfoPtr = DataTablePtr->FindRow<FTableRowProxy_TagExtendInfo>(*UnitType.ToString(), TEXT("GetUnit"));

	return SceneUnitExtendInfoPtr;
}

const FTableRow_Regions* UDataTableCollection::GetTableRow_Region(
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

const FTableRow_TalenSocket* UDataTableCollection::GetTableRow_TalenSocket(
	FGameplayTag UnitType
	) const
{
	auto DataTablePtr = TableRow_TalenSocket.LoadSynchronous();

	auto RegionsPtr = DataTablePtr->FindRow<FTableRow_TalenSocket>(*UnitType.ToString(), TEXT("GetUnit"));
	if (RegionsPtr)
	{
		return RegionsPtr;
	}

	return nullptr;
}

const FTableRow_RewardsItems_DefeatEnemy* UDataTableCollection::GetTableRow_RewardsItems_DefeatEnemy(
	FGameplayTag UnitType
	) const
{
	auto DataTablePtr = TableRow_RewardsItems_DefeatEnemy.LoadSynchronous();

	auto RegionsPtr = DataTablePtr->FindRow<FTableRow_RewardsItems_DefeatEnemy>(*UnitType.ToString(), TEXT("GetUnit"));
	if (RegionsPtr)
	{
		return RegionsPtr;
	}

	return nullptr;
}

TArray<FTableRow_Regions*> UDataTableCollection::GetTableRow_AllRegions() const
{
	auto DataTablePtr = DataTable_Regions.LoadSynchronous();

	TArray<FTableRow_Regions*> Result;
	DataTablePtr->GetAllRows<FTableRow_Regions>(TEXT("GetUnit"),Result);

	return Result;
}

TArray<FTableRowProxy_CharacterGrowthAttribute*> UDataTableCollection::GetTableRow_CharacterGrowthAttribute() const
{
	auto DataTablePtr = DataTable_Character_GrowthAttribute.LoadSynchronous();

	TArray<FTableRowProxy_CharacterGrowthAttribute*> Result;
	DataTablePtr->GetAllRows<FTableRowProxy_CharacterGrowthAttribute>(TEXT("GetUnit"),Result);

	return Result;
}

void UDataTableCollection::InitialData()
{
}

