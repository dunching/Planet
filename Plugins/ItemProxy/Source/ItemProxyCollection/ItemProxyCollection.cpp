
#include "ItemProxyCollection.h"

#include <Kismet/GameplayStatics.h>

#include "GameplayTagsManager.h"
#include "Tools.h"

UItemProxyCollection* UItemProxyCollection::GetInstance()
{
	checkNoEntry();
	
	return nullptr;
}

const FTableRowProxy* UItemProxyCollection::GetTableRowProxy(FGameplayTag UnitType) const
{
	if (ProxysMap.Contains(UnitType))
	{
		return &ProxysMap[UnitType];
	}
	return nullptr;
}
