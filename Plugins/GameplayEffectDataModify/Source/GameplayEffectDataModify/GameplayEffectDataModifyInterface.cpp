#include "GameplayEffectDataModifyInterface.h"

#include "Kismet/KismetMathLibrary.h"
#include "GameplayTagContainer.h"

float IGameplayEffectDataModifyInterface::GetBaseValueMaps(
	const FGameplayEffectSpec& Spec,
	const FGameplayAttributeData* GameplayAttributeDataPtr
	) const
{
	float Result = 0.f;

	if (!ValueMap.Contains(GameplayAttributeDataPtr))
	{
		return Result;
	}

	auto& GameplayAttributeDataMap = ValueMap[GameplayAttributeDataPtr];

	for (auto Iter : GameplayAttributeDataMap)
	{
		Result += Iter.Value;
	}

	// 去掉小数部分
	return UKismetMathLibrary::Round(Result);
}

void IGameplayEffectDataModifyInterface::UpdateValueMap()
{
	for (auto Iter = ValueMap.CreateIterator(); Iter; ++Iter)
	{
		for (auto SecondIter = Iter->Value.CreateIterator(); SecondIter; ++SecondIter)
		{
			if (SecondIter->Value <= 0)
			{
				SecondIter.RemoveCurrent();
			}
		}
		if (Iter->Value.IsEmpty())
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
