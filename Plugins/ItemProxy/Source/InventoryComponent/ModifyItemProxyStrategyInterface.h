// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <set>

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InventoryComponentBase.h"

#include "UObject/ObjectMacros.h"
#include "UObject/Interface.h"

#include "TemplateHelper.h"

#include "ModifyItemProxyStrategyInterface.generated.h"

USTRUCT()
struct ITEMPROXY_API FModifyItemProxyStrategyInterface
{
	GENERATED_USTRUCT_BODY()

	using FItemProxyType = FBasicProxy;
	
	FModifyItemProxyStrategyInterface();
	
	virtual ~FModifyItemProxyStrategyInterface();

	virtual FGameplayTag GetCanOperationType() const;

	virtual void FindByID(
		const FGuid& ID,
		const TSharedPtr<FBasicProxy>&FindResultSPtr,
		const TObjectPtr<const UInventoryComponentBase>& InventoryComponentPtr
		);

	virtual void FindByType(
		const FGameplayTag& ProxyType,
		const TSharedPtr<FBasicProxy>&FindResultSPtr,
		const TObjectPtr<const UInventoryComponentBase>& InventoryComponentPtr
		);

	virtual void Add(
		const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
		int32 Num
		);

public:
};

template <typename ItemProxyType>
struct FModifyItemProxyStrategyBase : public FModifyItemProxyStrategyInterface
{
	using FItemProxyType = ItemProxyType;
	
	virtual void FindByID(
		const FGuid& ID,
		const TSharedPtr<FBasicProxy>&FindResultSPtr,
		const TObjectPtr<const UInventoryComponentBase>& InventoryComponentPtr
		) override;

	virtual void FindByType(
		const FGameplayTag& ProxyType,
		const TSharedPtr<FBasicProxy>&FindResultSPtr,
		const TObjectPtr<const UInventoryComponentBase>& InventoryComponentPtr
		) override;

	virtual void Add(
		const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
		int32 Num
		) override;
	
	TSharedPtr<FItemProxyType>ResultSPtr = nullptr;
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
void FModifyItemProxyStrategyBase<ItemProxyType>::FindByType(
	const FGameplayTag& ProxyType,
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
void FModifyItemProxyStrategyBase<ItemProxyType>::Add(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	int32 Num
	)
{
	const auto ProxyType = GetCanOperationType();
	auto SceneProxyExtendInfoPtr = GetTableRowProxy(ProxyType);

	auto NewResultSPtr = MakeShared<FItemProxyType>();

	NewResultSPtr->InitialProxy(ProxyType);

	InventoryComponentPtr->AddToContainer(NewResultSPtr);
}
