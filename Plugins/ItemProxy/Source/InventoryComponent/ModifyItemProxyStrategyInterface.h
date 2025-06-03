// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <set>

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "UObject/ObjectMacros.h"
#include "UObject/Interface.h"

#include "TemplateHelper.h"
#include "InventoryComponentBase.h"
#include "ItemProxy.h"
#include "PAD_ItemProxyCollection.h"

#include "ModifyItemProxyStrategyInterface.generated.h"

USTRUCT()
struct ITEMPROXY_API FModifyItemProxyStrategyIterface
{
	GENERATED_USTRUCT_BODY()

	using FItemProxyType = FBasicProxy;

	FModifyItemProxyStrategyIterface();

	virtual ~FModifyItemProxyStrategyIterface();

	virtual FGameplayTag GetCanOperationType() const;

	virtual void FindByID(
		const FGuid& ID,
		const TSharedPtr<FBasicProxy>& FindResultSPtr,
		const TObjectPtr<const UInventoryComponentBase>& InventoryComponentPtr
		);

	virtual TSharedPtr<FBasicProxy> FindByType(
		const FGameplayTag& ProxyType,
		const TObjectPtr<const UInventoryComponentBase>& InventoryComponentPtr
		) const;

	virtual TArray<TSharedPtr<FBasicProxy>> Add(
		const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
		const FGameplayTag& InProxyType,
		int32 Num
		);

	virtual void Update(
		const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
		const FGuid& InProxyID
		);

	virtual void RemoveItemProxy(
		const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
		const FGuid& InProxyID
		);

	virtual TSharedPtr<FBasicProxy> AddByRemote(
		const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
		const TSharedPtr<FBasicProxy>& RemoteProxySPtr
		);

	virtual TSharedPtr<FBasicProxy> UpdateByRemote(
		const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
		const TSharedPtr<FBasicProxy>& LocalProxySPtr,
		const TSharedPtr<FBasicProxy>& RemoteProxySPtr
		);

	virtual void RemoveByRemote(
		const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
		const TSharedPtr<FBasicProxy>& RemoteProxySPtr
		);

	virtual TSharedPtr<FBasicProxy> NetSerialize(
		FArchive& Ar,
		class UPackageMap* Map,
		bool& bOutSuccess
		);

public:
};

template <typename ItemProxyType>
struct FModifyItemProxyStrategyBase : public FModifyItemProxyStrategyIterface
{
	using FItemProxyType = ItemProxyType;

	virtual void FindByID(
		const FGuid& ID,
		const TSharedPtr<FBasicProxy>& FindResultSPtr,
		const TObjectPtr<const UInventoryComponentBase>& InventoryComponentPtr
		) override;

	virtual TArray<TSharedPtr<FBasicProxy>> Add(
		const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
		const FGameplayTag& InProxyType,
		int32 Num
		) override;

	virtual void Update(
		const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
		const FGuid& InProxyID
		) override;

	virtual void RemoveItemProxy(
		const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
		const FGuid& InProxyID
		) override;

	virtual TSharedPtr<FBasicProxy> AddByRemote(
		const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
		const TSharedPtr<FBasicProxy>& RemoteProxySPtr
		) override;

	virtual TSharedPtr<FBasicProxy> UpdateByRemote(
		const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
		const TSharedPtr<FBasicProxy>& LocalProxySPtr,
		const TSharedPtr<FBasicProxy>& RemoteProxySPtr
		) override;

	virtual void RemoveByRemote(
		const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
		const TSharedPtr<FBasicProxy>& RemoteProxySPtr
		) override;

	virtual TSharedPtr<FBasicProxy> NetSerialize(
		FArchive& Ar,
		class UPackageMap* Map,
		bool& bOutSuccess
		) override;

	TSharedPtr<FItemProxyType> ResultSPtr = nullptr;
private:
};

template <typename ItemProxyType>
void FModifyItemProxyStrategyBase<ItemProxyType>::FindByID(
	const FGuid& ID,
	const TSharedPtr<FBasicProxy>& FindResultSPtr,
	const TObjectPtr<const UInventoryComponentBase>& InventoryComponentPtr
	)
{
	ResultSPtr = nullptr;

	if (FindResultSPtr)
	{
		ResultSPtr = DynamicCastSharedPtr<FItemProxyType>(FindResultSPtr);
	}
}

template <typename ItemProxyType>
TArray<TSharedPtr<FBasicProxy>> FModifyItemProxyStrategyBase<ItemProxyType>::Add(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	const FGameplayTag& InProxyType,
	int32 Num
	)
{
	auto NewResultSPtr = MakeShared<FItemProxyType>();

	NewResultSPtr->InitialProxy(InProxyType);

	InventoryComponentPtr->AddToContainer(NewResultSPtr);

	return {NewResultSPtr};
}

template <typename ItemProxyType>
void FModifyItemProxyStrategyBase<ItemProxyType>::Update(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	const FGuid& InProxyID
	)
{
	if (auto ProxySPtr = InventoryComponentPtr->FindProxy(InProxyID))
	{
		InventoryComponentPtr->UpdateInContainer(ProxySPtr);
	}
}

template <typename ItemProxyType>
void FModifyItemProxyStrategyBase<ItemProxyType>::RemoveItemProxy(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
		const FGuid& InProxyID
	)
{
	if (auto ProxySPtr = InventoryComponentPtr->FindProxy(InProxyID))
	{
		InventoryComponentPtr->RemoveFromContainer(ProxySPtr);
	}
}

template <typename ItemProxyType>
TSharedPtr<FBasicProxy> FModifyItemProxyStrategyBase<ItemProxyType>::AddByRemote(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	const TSharedPtr<FBasicProxy>& InRemoteProxySPtr
	)
{
	auto NewResultSPtr = MakeShared<FItemProxyType>();

	NewResultSPtr->InitialProxy(InRemoteProxySPtr->GetProxyType());

	InventoryComponentPtr->AddToContainer(NewResultSPtr);

	NewResultSPtr->UpdateByRemote(DynamicCastSharedPtr<FItemProxyType>(InRemoteProxySPtr));

	return NewResultSPtr;
}

template <typename ItemProxyType>
TSharedPtr<FBasicProxy> FModifyItemProxyStrategyBase<ItemProxyType>::UpdateByRemote(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	const TSharedPtr<FBasicProxy>& LocalProxySPtr,
	const TSharedPtr<FBasicProxy>& RemoteProxySPtr
	)
{
	auto RightProxySPtr = DynamicCastSharedPtr<FItemProxyType>(RemoteProxySPtr);

	auto LeftProxySPtr = DynamicCastSharedPtr<FItemProxyType>(LocalProxySPtr);

	LeftProxySPtr->UpdateByRemote(RightProxySPtr);

	return LeftProxySPtr;
}

template <typename ItemProxyType>
void FModifyItemProxyStrategyBase<ItemProxyType>::RemoveByRemote(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	const TSharedPtr<FBasicProxy>& RemoteProxySPtr
	)
{
	InventoryComponentPtr->RemoveFromContainer(RemoteProxySPtr);
}

template <typename ItemProxyType>
TSharedPtr<FBasicProxy> FModifyItemProxyStrategyBase<ItemProxyType>::NetSerialize(
	FArchive& Ar,
	class UPackageMap* Map,
	bool& bOutSuccess
	)
{
	auto NewResultSPtr = MakeShared<FItemProxyType>();

	NewResultSPtr->NetSerialize(Ar, Map, bOutSuccess);

	return NewResultSPtr;
}
