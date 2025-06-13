
#include "ItemProxy_Coin.h"

#include "AllocationSkills.h"
#include "InventoryComponentBase.h"
#include "ItemProxy_Container.h"
#include "TeamMatesHelperComponent.h"

FCoinProxy::FCoinProxy()
{

}

bool FCoinProxy::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);
	NetSerialize_Allocationble(Ar, Map, bOutSuccess);
	NetSerialize_Unique(Ar, Map, bOutSuccess);

	return true;
}

void FCoinProxy::InitialProxy(
	const FGameplayTag& InProxyType
	)
{
	Super::InitialProxy(InProxyType);
	
	ProxyPtr = this;
}

void FCoinProxy::UpdateByRemote(
	const TSharedPtr<FCoinProxy>& RemoteSPtr
	)
{
	Super::UpdateByRemote(RemoteSPtr);
	
	ProxyPtr = this;
	UpdateByRemote_Allocationble(RemoteSPtr);
	
	UpdateByRemote_Unique(RemoteSPtr);
}

void FCoinProxy::ModifyNum(
	int32 Value
	)
{
	IProxy_Unique::ModifyNum(Value);

	UpdateData();
}

bool FCoinProxy::IsUnique() const
{
	return true;
}
