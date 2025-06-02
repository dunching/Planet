// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <set>

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Interface.h"

#include "TemplateHelper.h"
#include "InventoryComponentBase.h"
#include "ModifyItemProxyStrategyInterface.h"


#include "ModifyItemProxyStrategyGeneric.generated.h"

template <typename ItemProxyType>
struct ITEMPROXY_API FModifyItemProxyStrategyGeneric : public FModifyItemProxyStrategyInterface
{
	using FItemProxyType = ItemProxyType;
	
	virtual FGameplayTag GetCanOperationType() const override;

	void SetOperationType(
		FGameplayTag InOperationType
		);

	virtual void Add(
		const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
		int32 Num
		) override;

private:
	FGameplayTag OperationType;
};

template <typename ItemProxy>
void FModifyItemProxyStrategyGeneric<ItemProxy>::Add(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	int32 Num
	)
{
	const auto ProxyType = GetCanOperationType();
	auto SceneProxyExtendInfoPtr = GetTableRowProxy(ProxyType);

	auto ResultPtr = MakeShared<FItemProxyType>();

	ResultPtr->InitialProxy(ProxyType);

	GetInventoryComponentBase()->AddToContainer(ResultPtr);
}

template <typename ItemProxy>
FGameplayTag FModifyItemProxyStrategyGeneric<ItemProxy>::GetCanOperationType() const
{
	return OperationType;
}

template <typename ItemProxy>
void FModifyItemProxyStrategyGeneric<ItemProxy>::SetOperationType(
	FGameplayTag InOperationType
	)
{
	OperationType = InOperationType;
}
