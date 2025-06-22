#include "ModifyItemProxyStrategyInterface.h"

#include "InventoryComponentBase.h"

FModifyItemProxyStrategyIterface::FModifyItemProxyStrategyIterface()
{
}

FModifyItemProxyStrategyIterface::~FModifyItemProxyStrategyIterface()
{
}

TSharedPtr<FBasicProxy> FModifyItemProxyStrategyIterface::NetSerialize(
	const FGameplayTag ProxyType,
	FArchive& Ar,
	class UPackageMap* Map,
	bool& bOutSuccess
	)
{
	return nullptr;
}

FGameplayTag FModifyItemProxyStrategyIterface::GetCanOperationType() const
{
	return FGameplayTag::EmptyTag;
}

void FModifyItemProxyStrategyIterface::FindByID(
	const FGuid& ID,
	const TSharedPtr<FBasicProxy>& FindResultSPtr,
	const TObjectPtr<const UInventoryComponentBase>& InventoryComponentPtr
	)
{
}

TArray<TSharedPtr<FBasicProxy>> FModifyItemProxyStrategyIterface::FindByType(
	const FGameplayTag& ProxyType,
	const TObjectPtr<const UInventoryComponentBase>& InventoryComponentPtr
	) const
{
	return {};
}

TArray<TSharedPtr<FBasicProxy>> FModifyItemProxyStrategyIterface::Add(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	const FGameplayTag& InProxyType,
	int32 Num
	)
{
	return {};
}

void FModifyItemProxyStrategyIterface::Update(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	const FGuid& InProxyID
	)
{
	if (auto ProxySPtr = InventoryComponentPtr->FindProxy(InProxyID))
	{
		InventoryComponentPtr->UpdateInContainer(ProxySPtr);
	}
}

void FModifyItemProxyStrategyIterface::RemoveItemProxy(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	const FGuid& InProxyID
	)
{
	if (auto ProxySPtr = InventoryComponentPtr->FindProxy(InProxyID))
	{
		InventoryComponentPtr->RemoveFromContainer(ProxySPtr);
	}
}

TSharedPtr<FBasicProxy> FModifyItemProxyStrategyIterface::AddByRemote(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	const TSharedPtr<FBasicProxy>& RemoteProxySPtr
	)
{
	return nullptr;
}

TSharedPtr<FBasicProxy> FModifyItemProxyStrategyIterface::UpdateByRemote(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	const TSharedPtr<FBasicProxy>& LocalProxySPtr,
	const TSharedPtr<FBasicProxy>& RemoteProxySPtr
	)
{
	return nullptr;
}

void FModifyItemProxyStrategyIterface::RemoveByRemote(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	const TSharedPtr<FBasicProxy>& RemoteProxySPtr
	)
{
	RemoveItemProxy(InventoryComponentPtr, RemoteProxySPtr->GetID());
}
