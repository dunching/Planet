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

IGetValueModifyInterface::IGetValueModifyInterface(
	int32 InPriority
	):
	 IDataModifyInterface(InPriority)
{
}

int32 IGetValueModifyInterface::GetValue(
	const FDataComposition& DataComposition,
	int32 PreviouValue
	) const
{
	return PreviouValue;
}

IGostModifyInterface::IGostModifyInterface(
	int32 InPriority
	):
	 IDataModifyInterface(InPriority)
{
}

TMap<FGameplayTag, int32> IGostModifyInterface::GetCost(
	const TMap<FGameplayTag, int32>& Original,
	const TMap<FGameplayTag, int32>& CurrentOriginal
	) const
{
	return {};
}

IRangeModifyInterface::IRangeModifyInterface(
	int32 InPriority
	):
	 IDataModifyInterface(InPriority)
{
}

float IRangeModifyInterface::GetRangeMagnitude() const
{
	return 1.f;
}

IDurationModifyInterface::IDurationModifyInterface(
	int32 InPriority
	):
	 IDataModifyInterface(InPriority)
{
}

float IDurationModifyInterface::GetDuration(
	const UAS_Character* AS_CharacterAttributePtr, 
	float Duration
	) const
{
	return Duration;
}

int32 ICooldownModifyInterface::GetCooldown(
	const UAS_Character* AS_CharacterAttributePtr, 
	int32 Cooldown
	) const
{
	return Cooldown;
}

inline ICooldownModifyInterface::ICooldownModifyInterface(
	int32 InPriority
	):
	 IDataModifyInterface(InPriority)
{
}

IExtraEffectModifyInterface::IExtraEffectModifyInterface(
	int32 InPriority
	):
	 IDataModifyInterface(InPriority)
{
}

int32 IExtraEffectModifyInterface::GetExtraEffectCount() const
{
	return 1;
}
