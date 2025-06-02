#include "ModifyItemProxyStrategyInterface.h"

#include "InventoryComponentBase.h"

FModifyItemProxyStrategyInterface::FModifyItemProxyStrategyInterface()
{
}

FModifyItemProxyStrategyInterface::~FModifyItemProxyStrategyInterface()
{}

void FModifyItemProxyStrategyInterface::Add(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	int32 Num
	)
{
}

FGameplayTag FModifyItemProxyStrategyInterface::GetCanOperationType() const
{
	return FGameplayTag::EmptyTag;
}

void FModifyItemProxyStrategyInterface::FindByID(
	const FGuid& ID,
	const TSharedPtr<FBasicProxy>&FindResultSPtr,
	const TObjectPtr<const UInventoryComponentBase>& InventoryComponentPtr
	)
{
}

void FModifyItemProxyStrategyInterface::FindByType(
	const FGameplayTag& ProxyType,
	const TSharedPtr<FBasicProxy>& FindResultSPtr,
	const TObjectPtr<const UInventoryComponentBase>& InventoryComponentPtr
	)
{
}
