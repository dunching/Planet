#include "GetModifyItemProxyStrategiesInterface.h"

const TMap<FGameplayTag, TSharedPtr<FModifyItemProxyStrategyIterface>>& IGetModifyItemProxyStrategies::
GetModifyItemProxyStrategies() const
{
	return ModifyItemProxyStrategiesMap;
}
