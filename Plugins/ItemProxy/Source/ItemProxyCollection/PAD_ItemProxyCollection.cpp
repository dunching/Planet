
#include "PAD_ItemProxyCollection.h"

#include <Kismet/GameplayStatics.h>

#include "GameplayTagsManager.h"
#include "Tools.h"

const FTableRowProxy* GetTableRowProxy(
	const FGameplayTag& ProxyType
	)
{
	auto SceneProxyExtendInfoMapPtr = UPAD_ItemProxyCollection::GetInstance();
	auto DataTable = SceneProxyExtendInfoMapPtr->DataTable_Proxy.LoadSynchronous();

	auto SceneProxyExtendInfoPtr = DataTable->FindRow<FTableRowProxy>(*ProxyType.ToString(), TEXT("GetProxy"));

	return SceneProxyExtendInfoPtr;
}

const UPAD_ItemProxyCollection* UPAD_ItemProxyCollection::GetInstance()
{
	auto WorldSetting = Cast<IGetItemProxyCollectionInterface>(GetWorldImp()->GetWorldSettings());
	return WorldSetting->GetItemProxyCollection();
}

const FTableRowProxy* UPAD_ItemProxyCollection::GetTableRowProxy(
	FGameplayTag UnitType
	) const
{
	auto DataTablePtr = DataTable_Proxy.LoadSynchronous();

	auto SceneUnitExtendInfoPtr = DataTablePtr->FindRow<FTableRowProxy>(*UnitType.ToString(), TEXT("GetUnit"));

	return SceneUnitExtendInfoPtr;
}
