#include "DataModifyStrategies.h"

IDataModifyInterface::IDataModifyInterface(
	int32 InPriority
	) :
	  Priority(InPriority)
{
	ID = FMath::Rand32();
}

IDataModifyInterface::~IDataModifyInterface()
{
}

bool IDataModifyInterface::operator<(
	const IDataModifyInterface& RightValue
	) const
{
	return (Priority > RightValue.Priority) && (ID == RightValue.ID);
}

IOutputDataModifyInterface::IOutputDataModifyInterface(
	int32 InPriority /*= 1*/
	) :
	  IDataModifyInterface(InPriority)
{
}

bool IOutputDataModifyInterface::Modify(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput,
	TSet<FGameplayTag>& NeedModifySet,
	TMap<FGameplayTag, float>& NewDatas,
	TSet<EAdditionalModify>& AdditionalModifyAry
	)
{
	return true;
}

IInputDataModifyInterface::IInputDataModifyInterface(
	int32 InPriority /*= 1*/
	) :
	  IDataModifyInterface(InPriority)
{
}

bool IInputDataModifyInterface::Modify(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput,
	TSet<FGameplayTag>& NeedModifySet,
	TMap<FGameplayTag, float>& NewDatas,
	TSet<EAdditionalModify>& AdditionalModifyAry
	)
{
	return true;
}
