#include "GameplayEffectDataModifyInterface.h"

#include "Kismet/KismetMathLibrary.h"
#include "GameplayTagContainer.h"

float IGameplayEffectDataModifyInterface::GetBaseValueMaps(
	const FGameplayAttributeData* GameplayAttributeDataPtr
	)const
{
	float Result = 0.f;

	if (ValueMap.Contains(GameplayAttributeDataPtr))
	{
		const auto& GameplayAttributeDataMap = ValueMap[GameplayAttributeDataPtr];

		auto& ModifyStrategiesRef = GetValueModifysMap;

		int32 Value = 0;
		for (auto Iter = ModifyStrategiesRef.begin(); Iter != ModifyStrategiesRef.end(); Iter++)
		{
			Value = (*Iter)->GetValue(GameplayAttributeDataMap, Value);
		}
		
		Result = Value;
	}
	else
	{
		Result = GameplayAttributeDataPtr->GetCurrentValue();
	}

	return Result;
}

void IGameplayEffectDataModifyInterface::UpdateValueMap()
{
	for (auto Iter = ValueMap.CreateIterator(); Iter; ++Iter)
	{
		for (auto SecondIter = Iter->Value.DataMap.CreateIterator(); SecondIter; ++SecondIter)
		{
			if (SecondIter->Value <= 0)
			{
				SecondIter.RemoveCurrent();
			}
		}
		for (auto SecondIter = Iter->Value.MagnitudeMap.CreateIterator(); SecondIter; ++SecondIter)
		{
			if (SecondIter->Value <= 0)
			{
				SecondIter.RemoveCurrent();
			}
		}
		if (Iter->Value.DataMap.IsEmpty() && Iter->Value.MagnitudeMap.IsEmpty())
		{
			Iter.RemoveCurrent();
		}
	}
}

void IGameplayEffectDataModifyInterface::ApplyInputData(
	const FGameplayTagContainer& AllAssetTags,
	TSet<FGameplayTag>& NeedModifySet,
	const TMap<FGameplayTag, float>& CustomMagnitudes,
	const TSet<EAdditionalModify>& AdditionalModifyAry,
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput
	)
{
}

TMap<FGameplayTag, int32> IGameplayEffectDataModifyInterface::GetCost(
	const TMap<FGameplayTag, int32>& CostMap
	)
{
	TMap<FGameplayTag, int32>Result = CostMap;
	
	auto& ModifyStrategiesRef = CostModifysMap;

	int32 Value = 0;
	for (auto Iter = ModifyStrategiesRef.begin(); Iter != ModifyStrategiesRef.end(); Iter++)
	{
		Result = (*Iter)->GetCost(CostMap,Result);
	}

	return Result;
}

bool IGameplayEffectDataModifyInterface::CheckCost(
	const TMap<FGameplayTag, int32>& CostMap
	) const
{
	return false;
}

void IGameplayEffectDataModifyInterface::ModifyInputData(
	const FGameplayTagContainer& AllAssetTags,
	TSet<FGameplayTag>& NeedModifySet,
	TMap<FGameplayTag, float>& NewDatas,
	TSet<EAdditionalModify>& AdditionalModifyAry,
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput
	)
{
	// 根据自身的效果对【输入】进行一些修正
	TArray<decltype(InputDataModifysMap)::iterator> NeedRemoveIterAry;;

	for (auto Iter = InputDataModifysMap.begin(); Iter != InputDataModifysMap.end(); Iter++)
	{
		if ((*Iter)->Modify(
		                    ExecutionParams,
		                    OutExecutionOutput,
		                    NeedModifySet,
		                    NewDatas,
		                    AdditionalModifyAry
		                   ))
		{
			NeedRemoveIterAry.Add(Iter);
		}
	}

	for (auto Iter : NeedRemoveIterAry)
	{
		InputDataModifysMap.erase(Iter);
	}
}

void FDataComposition::Empty()
{
	DataMap.Empty();

	MagnitudeMap.Empty();
}

void IGameplayEffectDataModifyInterface::ModifyOutputData(
	const FGameplayTagContainer& AllAssetTags,
	TSet<FGameplayTag>& NeedModifySet,
	TMap<FGameplayTag, float>& NewDatas,
	TSet<EAdditionalModify>& AdditionalModifyAry,
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput
	)
{
	// 根据自身的效果对【输出】进行一些修正
	TArray<decltype(OutputDataModifysMap)::iterator> NeedRemoveIterAry;;

	for (auto Iter = OutputDataModifysMap.begin(); Iter != OutputDataModifysMap.end(); Iter++)
	{
		if ((*Iter)->Modify(
		                    ExecutionParams,
		                    OutExecutionOutput,
		                    NeedModifySet,
		                    NewDatas,
		                    AdditionalModifyAry
		                   ))
		{
			NeedRemoveIterAry.Add(Iter);
		}
	}

	for (auto Iter : NeedRemoveIterAry)
	{
		OutputDataModifysMap.erase(Iter);
	}
}

void IGameplayEffectDataModifyInterface::AddOutputModify(
	const TSharedPtr<IOutputDataModifyInterface>& GAEventModifySPtr
	)
{
	for (bool bIsContinue = true; bIsContinue;)
	{
		bIsContinue = false;
		GAEventModifySPtr->ID = FMath::RandRange(1, std::numeric_limits<int32>::max());
		for (const auto& Iter : OutputDataModifysMap)
		{
			if (Iter->ID == GAEventModifySPtr->ID)
			{
				bIsContinue = true;
				break;
			}
		}
	}
	OutputDataModifysMap.emplace(GAEventModifySPtr);
}

void IGameplayEffectDataModifyInterface::RemoveOutputModify(
	const TSharedPtr<IOutputDataModifyInterface>& GAEventModifySPtr
	)
{
	for (auto Iter = OutputDataModifysMap.begin(); Iter != OutputDataModifysMap.end(); Iter++)
	{
		if ((*Iter)->ID == GAEventModifySPtr->ID)
		{
			OutputDataModifysMap.erase(Iter);
			break;
		}
	}
}

void IGameplayEffectDataModifyInterface::AddInputModify(
	const TSharedPtr<IInputDataModifyInterface>& GAEventModifySPtr
	)
{
	for (bool bIsContinue = true; bIsContinue;)
	{
		bIsContinue = false;
		GAEventModifySPtr->ID = FMath::RandRange(1, std::numeric_limits<int32>::max());
		for (const auto& Iter : InputDataModifysMap)
		{
			if (Iter->ID == GAEventModifySPtr->ID)
			{
				bIsContinue = true;
				break;
			}
		}
	}
	InputDataModifysMap.emplace(GAEventModifySPtr);
}

void IGameplayEffectDataModifyInterface::RemoveInputModify(
	const TSharedPtr<IInputDataModifyInterface>& GAEventModifySPtr
	)
{
	for (auto Iter = InputDataModifysMap.begin(); Iter != InputDataModifysMap.end(); Iter++)
	{
		if ((*Iter)->ID == GAEventModifySPtr->ID)
		{
			InputDataModifysMap.erase(Iter);
			break;
		}
	}
}

void IGameplayEffectDataModifyInterface::AddGetValueModify(
	const TSharedPtr<IGetValueModifyInterface>& GAEventModifySPtr
	)
{
	for (bool bIsContinue = true; bIsContinue;)
	{
		bIsContinue = false;
		GAEventModifySPtr->ID = FMath::RandRange(1, std::numeric_limits<int32>::max());
		for (const auto& Iter : GetValueModifysMap)
		{
			if (Iter->ID == GAEventModifySPtr->ID)
			{
				bIsContinue = true;
				break;
			}
		}
	}
	GetValueModifysMap.emplace(GAEventModifySPtr);
}

void IGameplayEffectDataModifyInterface::RemoveGetValueModify(
	const TSharedPtr<IGetValueModifyInterface>& GAEventModifySPtr
	)
{
	for (auto Iter = GetValueModifysMap.begin(); Iter != GetValueModifysMap.end(); Iter++)
	{
		if ((*Iter)->ID == GAEventModifySPtr->ID)
		{
			GetValueModifysMap.erase(Iter);
			break;
		}
	}
}

void IGameplayEffectDataModifyInterface::AddGostModify(
	const TSharedPtr<IGostModifyInterface>& GAEventModifySPtr
	)
{
	for (bool bIsContinue = true; bIsContinue;)
	{
		bIsContinue = false;
		GAEventModifySPtr->ID = FMath::RandRange(1, std::numeric_limits<int32>::max());
		for (const auto& Iter : CostModifysMap)
		{
			if (Iter->ID == GAEventModifySPtr->ID)
			{
				bIsContinue = true;
				break;
			}
		}
	}
	CostModifysMap.emplace(GAEventModifySPtr);
}

void IGameplayEffectDataModifyInterface::RemoveGostModify(
	const TSharedPtr<IGostModifyInterface>& GAEventModifySPtr
	)
{
	for (auto Iter = CostModifysMap.begin(); Iter != CostModifysMap.end(); Iter++)
	{
		if ((*Iter)->ID == GAEventModifySPtr->ID)
		{
			CostModifysMap.erase(Iter);
			break;
		}
	}
}
