#include "GameplayEffectDataModifyInterface.h"

#include "Kismet/KismetMathLibrary.h"
#include "GameplayTagContainer.h"

float IGameplayEffectDataModifyInterface::GetBaseValueMaps(
	const FGameplayAttributeData* GameplayAttributeDataPtr
	) const
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

void IGameplayEffectDataModifyInterface::UpdateValueMap(
	const FGameplayAttributeData* GameplayAttributeDataPtr
	)
{
	if (ValueMap.Contains(GameplayAttributeDataPtr))
	{
		auto & Ref = ValueMap[GameplayAttributeDataPtr];
		for (auto SecondIter = Ref.DataMap.CreateIterator(); SecondIter; ++SecondIter)
		{
			if (SecondIter->Key == FGameplayTag::EmptyTag)
			{
			}
			else
			{
				if (SecondIter->Value <= 0)
				{
					SecondIter.RemoveCurrent();
				}
			}
		}
		
		for (auto SecondIter = Ref.MagnitudeMap.CreateIterator(); SecondIter; ++SecondIter)
		{
			if (SecondIter->Key == FGameplayTag::EmptyTag)
			{
			}
			else
			{
				if (SecondIter->Value <= 0)
				{
					SecondIter.RemoveCurrent();
				}
			}
		}
		
		if (Ref.DataMap.IsEmpty() && Ref.MagnitudeMap.IsEmpty())
		{
			ValueMap.Remove(GameplayAttributeDataPtr);
		}
	}
	
	for (auto Iter = ValueMap.CreateIterator(); Iter; ++Iter)
	{
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
	TMap<FGameplayTag, int32> Result = CostMap;

	auto& ModifyStrategiesRef = CostModifysMap;

	int32 Value = 0;
	for (auto Iter = ModifyStrategiesRef.begin(); Iter != ModifyStrategiesRef.end(); Iter++)
	{
		Result = (*Iter)->GetCost(CostMap, Result);
	}

	return Result;
}

bool IGameplayEffectDataModifyInterface::CheckCost(
	const TMap<FGameplayTag, int32>& CostMap
	) const
{
	return false;
}

float IGameplayEffectDataModifyInterface::GetDuration(
	const UAS_Character* AS_CharacterAttributePtr,
	float Duration
	) const
{
	auto& ModifyStrategiesRef = DurationModifysMap;

	float Result = Duration;
	for (auto Iter = ModifyStrategiesRef.begin(); Iter != ModifyStrategiesRef.end(); Iter++)
	{
		Result = (*Iter)->GetDuration(AS_CharacterAttributePtr, Result);
	}

	return Result;
}

int32 IGameplayEffectDataModifyInterface::GetCooldown(
	const UAS_Character* AS_CharacterAttributePtr,
	int32 Cooldown
	) const
{
	auto& ModifyStrategiesRef = CooldownModifysMap;

	int32 Result = Cooldown;
	for (auto Iter = ModifyStrategiesRef.begin(); Iter != ModifyStrategiesRef.end(); Iter++)
	{
		Result = (*Iter)->GetCooldown(AS_CharacterAttributePtr, Result);
	}

	return Result;
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

void IGameplayEffectDataModifyInterface::AddDurationModify(
	const TSharedPtr<IDurationModifyInterface>& GAEventModifySPtr
	)
{
	for (bool bIsContinue = true; bIsContinue;)
	{
		bIsContinue = false;
		GAEventModifySPtr->ID = FMath::RandRange(1, std::numeric_limits<int32>::max());
		for (const auto& Iter : DurationModifysMap)
		{
			if (Iter->ID == GAEventModifySPtr->ID)
			{
				bIsContinue = true;
				break;
			}
		}
	}
	DurationModifysMap.emplace(GAEventModifySPtr);
}

inline void IGameplayEffectDataModifyInterface::AddCooldownModify(
	const TSharedPtr<ICooldownModifyInterface>& GAEventModifySPtr
	)
{
	for (bool bIsContinue = true; bIsContinue;)
	{
		bIsContinue = false;
		GAEventModifySPtr->ID = FMath::RandRange(1, std::numeric_limits<int32>::max());
		for (const auto& Iter : CooldownModifysMap)
		{
			if (Iter->ID == GAEventModifySPtr->ID)
			{
				bIsContinue = true;
				break;
			}
		}
	}
	CooldownModifysMap.emplace(GAEventModifySPtr);
}

void IGameplayEffectDataModifyInterface::RemoveCooldownModify(
	const TSharedPtr<ICooldownModifyInterface>& GAEventModifySPtr
	)
{
	for (auto Iter = CooldownModifysMap.begin(); Iter != CooldownModifysMap.end(); Iter++)
	{
		if ((*Iter)->ID == GAEventModifySPtr->ID)
		{
			CooldownModifysMap.erase(Iter);
			break;
		}
	}
}

void IGameplayEffectDataModifyInterface::UpdatePermanentValue(
	float Value,
	int32 MinValue,
	int32 MaxValue,
	EUpdateValueType UpdateValueType,
	const FGameplayEffectSpec& Spec,
	const FGameplayAttributeData* GameplayAttributeDataPtr
	)
{
	if (ValueMap.Contains(GameplayAttributeDataPtr))
	{
	}
	else
	{
		FDataComposition DataComposition;

		DataComposition.DataMap = {
			{
				FGameplayTag::EmptyTag,
				FMath::Clamp(GameplayAttributeDataPtr->GetCurrentValue(), MinValue, MaxValue)
			}
		};

		ValueMap.Add(
		             GameplayAttributeDataPtr,
		             DataComposition
		            );
	}

	switch (UpdateValueType)
	{
	case EUpdateValueType::kPermanent_Addtive:
		{
			auto& GameplayAttributeDataMap = ValueMap[GameplayAttributeDataPtr];

			if (GameplayAttributeDataMap.DataMap.Contains(FGameplayTag::EmptyTag))
			{
				GameplayAttributeDataMap.DataMap[FGameplayTag::EmptyTag] += Value;
			}
			else
			{
				GameplayAttributeDataMap.DataMap.Add(FGameplayTag::EmptyTag, Value);
			}

			GameplayAttributeDataMap.DataMap[FGameplayTag::EmptyTag] =
				FMath::Clamp(
				             GameplayAttributeDataMap.DataMap[FGameplayTag::EmptyTag],
				             MinValue,
				             MaxValue
				            );
		}
		break;
	case EUpdateValueType::kPermanent_Override:
		{
			auto& GameplayAttributeDataMap = ValueMap[GameplayAttributeDataPtr];

			if (GameplayAttributeDataMap.DataMap.Contains(FGameplayTag::EmptyTag))
			{
				GameplayAttributeDataMap.DataMap[FGameplayTag::EmptyTag] = Value;
			}
			else
			{
				GameplayAttributeDataMap.DataMap.Add(FGameplayTag::EmptyTag, Value);
			}

			GameplayAttributeDataMap.DataMap[FGameplayTag::EmptyTag] =
				FMath::Clamp(
				             GameplayAttributeDataMap.DataMap[FGameplayTag::EmptyTag],
				             MinValue,
				             MaxValue
				            );
		}
		break;
	}

	UpdateValueMap(GameplayAttributeDataPtr);
}

void IGameplayEffectDataModifyInterface::UpdateTemporaryValue(
	const FGameplayTag& Tag,
	float Value,
	EUpdateValueType UpdateValueType,
	const FGameplayEffectSpec& Spec,
	const FGameplayAttributeData* GameplayAttributeDataPtr
	)
{
	FGameplayTagContainer AllAssetTags;
	Spec.GetAllAssetTags(AllAssetTags);

	switch (UpdateValueType)
	{
	case EUpdateValueType::kTemporary_Data_Addtive:
		{
			if (ValueMap.Contains(GameplayAttributeDataPtr))
			{
				auto& GameplayAttributeDataMap = ValueMap[GameplayAttributeDataPtr];
				if (GameplayAttributeDataMap.DataMap.Contains(Tag))
				{
					GameplayAttributeDataMap.DataMap[Tag] += Value;
				}
				else
				{
					GameplayAttributeDataMap.DataMap.Add(Tag, Value);
				}
			}
			else
			{
				FDataComposition DataComposition;

				DataComposition.DataMap = {
					{
						FGameplayTag::EmptyTag,
						GameplayAttributeDataPtr->GetCurrentValue()
					},
					{
						Tag, Value
					}
				};

				ValueMap.Add(
				             GameplayAttributeDataPtr,
				             DataComposition
				            );
			}
			return;
		}
		break;
	case EUpdateValueType::kTemporary_Percent_Addtive:
		{
			if (ValueMap.Contains(GameplayAttributeDataPtr))
			{
				auto& GameplayAttributeDataMap = ValueMap[GameplayAttributeDataPtr];
				if (GameplayAttributeDataMap.MagnitudeMap.Contains(Tag))
				{
					GameplayAttributeDataMap.MagnitudeMap[Tag] += Value;
				}
				else
				{
					GameplayAttributeDataMap.MagnitudeMap.Add(Tag, Value);
				}
			}
			else
			{
				FDataComposition DataComposition;

				DataComposition.DataMap = {
					{
						FGameplayTag::EmptyTag,
						GameplayAttributeDataPtr->GetCurrentValue()
					}
				};

				DataComposition.MagnitudeMap = {
					{
						Tag, Value
					}
				};

				ValueMap.Add(
				             GameplayAttributeDataPtr,
				             DataComposition
				            );
			}
			return;
		}
		break;
	case EUpdateValueType::kTemporary_Data_Override:
		{
			if (ValueMap.Contains(GameplayAttributeDataPtr))
			{
				auto& GameplayAttributeDataMap = ValueMap[GameplayAttributeDataPtr];
				GameplayAttributeDataMap.DataMap.Add(Tag, Value);
			}
			else
			{
				FDataComposition DataComposition;

				DataComposition.DataMap = {
					{
						FGameplayTag::EmptyTag,
						GameplayAttributeDataPtr->GetCurrentValue()
					},
					{
						Tag,
						Value
					}
				};

				ValueMap.Add(
				             GameplayAttributeDataPtr,
				             DataComposition
				            );
			}

			return;
		}
		break;
	case EUpdateValueType::kTemporary_Percent_Override:
		{
			if (ValueMap.Contains(GameplayAttributeDataPtr))
			{
				auto& GameplayAttributeDataMap = ValueMap[GameplayAttributeDataPtr];
				GameplayAttributeDataMap.MagnitudeMap.Add(Tag);
			}
			else
			{
				FDataComposition DataComposition;

				DataComposition.DataMap = {
					{
						FGameplayTag::EmptyTag,
						GameplayAttributeDataPtr->GetCurrentValue()
					}
				};

				DataComposition.MagnitudeMap = {
					{
						Tag,
						Value
					}
				};

				ValueMap.Add(
				             GameplayAttributeDataPtr,
				             DataComposition
				            );
			}

			return;
		}
		break;
	}
}

void IGameplayEffectDataModifyInterface::RemoveDurationModify(
	const TSharedPtr<IDurationModifyInterface>& GAEventModifySPtr
	)
{
	for (auto Iter = DurationModifysMap.begin(); Iter != DurationModifysMap.end(); Iter++)
	{
		if ((*Iter)->ID == GAEventModifySPtr->ID)
		{
			DurationModifysMap.erase(Iter);
			break;
		}
	}
}

void IGameplayEffectDataModifyInterface::UpdateTemporaryValue(
	const FGameplayTag& ModifyTypeTag,
	float Value,
	EUpdateValueType UpdateValueType,
	const FGameplayAttributeData* GameplayAttributeDataPtr
	)
{
	if (ValueMap.Contains(GameplayAttributeDataPtr))
	{
		switch (UpdateValueType)
		{
		case EUpdateValueType::kTemporary_Data_Addtive:
			{
				auto& GameplayAttributeDataMap = ValueMap[GameplayAttributeDataPtr];
				for (auto& Iter : GameplayAttributeDataMap.DataMap)
				{
					if (Value > Iter.Value)
					{
						Value -= Iter.Value;
						if (Value <= 0)
						{
							break;
						}
					}
					else
					{
						Iter.Value = Iter.Value
						             + Value;
						break;
					}
				}
			}
			break;
		case EUpdateValueType::kTemporary_Data_Override:
			{
				auto& GameplayAttributeDataMap = ValueMap[GameplayAttributeDataPtr];
				GameplayAttributeDataMap.Empty();
			}
			break;
		}
	}

	UpdateValueMap(GameplayAttributeDataPtr);
}
