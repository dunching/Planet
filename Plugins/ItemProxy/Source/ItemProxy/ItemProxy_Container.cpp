#include "ItemProxy_Container.h"

#include "InventoryComponentBase.h"
#include "ItemProxy.h"
#include "ItemProxyCollection.h"

const FTableRowProxy* GetTableRowProxy(
	const FGameplayTag& ProxyType
	)
{
	auto SceneProxyExtendInfoMapPtr = UItemProxyCollection::GetInstance();
	return SceneProxyExtendInfoMapPtr->GetTableRowProxy(ProxyType);
}

void FProxy_FASI::PreReplicatedRemove(
	const struct FProxy_FASI_Container& InArraySerializer
	)
{
	// 在这里 我们对本地的数据进行绑定

	InArraySerializer.GetInventoryComponentBase()->RemoveProxy_SyncHelper(ProxySPtr);
}

void FProxy_FASI::PostReplicatedAdd(
	const struct FProxy_FASI_Container& InArraySerializer
	)
{
	// 在这里 我们对本地的数据进行绑定

	ProxySPtr = InArraySerializer.GetInventoryComponentBase()->AddProxy_SyncHelper(CacheProxySPtr);
}

void FProxy_FASI::PostReplicatedChange(
	const struct FProxy_FASI_Container& InArraySerializer
	)
{
	// 在这里 我们对本地的数据进行绑定

	InArraySerializer.GetInventoryComponentBase()->UpdateProxy_SyncHelper(ProxySPtr, CacheProxySPtr);
}

bool FProxy_FASI::NetSerialize(
	FArchive& Ar,
	class UPackageMap* Map,
	bool& bOutSuccess
	)
{
	return true;
}

bool FProxy_FASI::operator==(
	const FProxy_FASI& Right
	) const
{
	if (ProxySPtr && Right.ProxySPtr)
	{
		return ProxySPtr->GetProxyType() == Right.ProxySPtr->GetProxyType();
	}

	return true;
}

bool FProxy_FASI_Container::NetDeltaSerialize(
	FNetDeltaSerializeInfo& DeltaParms
	)
{
	const auto Result =
		FFastArraySerializer::FastArrayDeltaSerialize<FItemType, FContainerType>(Items, DeltaParms, *this);

	return Result;
}

void FProxy_FASI_Container::AddItem(
	const TSharedPtr<FBasicProxy>& ProxySPtr
	)
{
#if UE_EDITOR || UE_SERVER
	if (GetInventoryComponentBase()->GetNetMode() == NM_DedicatedServer)
	{
		if (ProxySPtr)
		{
			FItemType Item;

			Item.ProxySPtr = ProxySPtr;

			auto& Ref = Items.Add_GetRef(Item);

			MarkItemDirty(Ref);
		}
	}
#endif
}

void FProxy_FASI_Container::UpdateItem(
	const TSharedPtr<FBasicProxy>& ProxySPtr
	)
{
#if UE_EDITOR || UE_SERVER
	if (GetInventoryComponentBase()->GetNetMode() == NM_DedicatedServer)
	{
		if (ProxySPtr)
		{
			for (int32 Index = 0; Index < Items.Num(); Index++)
			{
				if (Items[Index].ProxySPtr == ProxySPtr)
				{
					// 注意：ProxySPtr 这个指针已经在外部进行了修改，在这部我们不必再对 Items[Index] 进行修改
					MarkItemDirty(Items[Index]);
					return;
				}
			}

			AddItem(ProxySPtr);
		}
	}
#endif
}

void FProxy_FASI_Container::UpdateItem(
	const FGuid& Proxy_ID
	)
{
#if UE_EDITOR || UE_SERVER
	if (GetInventoryComponentBase()->GetNetMode() == NM_DedicatedServer)
	{
		for (int32 Index = 0; Index < Items.Num(); Index++)
		{
			if (Items[Index].ProxySPtr->GetID() == Proxy_ID)
			{
				// 注意：ProxySPtr 这个指针已经在外部进行了修改，在这部我们不必再对 Items[Index] 进行修改
				MarkItemDirty(Items[Index]);
				return;
			}
		}
	}
#endif
}

void FProxy_FASI_Container::RemoveItem(
	const TSharedPtr<FBasicProxy>& ProxySPtr
	)
{
#if UE_EDITOR || UE_SERVER
	if (GetInventoryComponentBase()->GetNetMode() == NM_DedicatedServer)
	{
		for (int32 Index = 0; Index < Items.Num(); ++Index)
		{
			if (Items[Index].ProxySPtr == ProxySPtr)
			{
				Items.RemoveAt(Index);

				MarkArrayDirty();
				return;
			}
		}
	}
#endif
}

void FProxy_FASI_Container::SetInventoryComponentBase(
	const TObjectPtr<UInventoryComponentBase>& NewInventoryComponentPtr
	)
{
	InventoryComponentPtr = NewInventoryComponentPtr;
}

TObjectPtr<UInventoryComponentBase> FProxy_FASI_Container::GetInventoryComponentBase() const
{
	return InventoryComponentPtr;
}

TSharedPtr<FBasicProxy> FProxy_FASI_Container::GetProxyType(
	const FGameplayTag& ProxyType
	)
{
	return nullptr;
}
