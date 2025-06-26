#include "DataModifyInterface.h"

#include "Kismet/KismetMathLibrary.h"

#include "AS_Character.h"
#include "CharacterAbilitySystemComponent.h"
#include "CharacterAttributesComponent.h"
#include "CharacterBase.h"
#include "GameplayTagsLibrary.h"


IOutputData_ModifyInterface_Base::IOutputData_ModifyInterface_Base(
	int32 InPriority
	):
	 IOutputDataModifyInterface(InPriority)
{
}

IInputData_ModifyInterface_Base::IInputData_ModifyInterface_Base(
	int32 InPriority
	):
	 IInputDataModifyInterface(InPriority)
{
}

IOutputData_ProbabilityConfirmation_ModifyInterface::IOutputData_ProbabilityConfirmation_ModifyInterface(
	int32 InPriority
	):
	 IOutputData_ModifyInterface_Base(InPriority)
{
}

bool IOutputData_ProbabilityConfirmation_ModifyInterface::Modify(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput,
	TSet<FGameplayTag>& NeedModifySet,
	TMap<FGameplayTag, float>& NewDatas,
	TSet<EAdditionalModify>& AdditionalModifyAry
	)
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FGameplayEffectContextHandle Context = Spec.GetContext();
	auto Instigator = Cast<FPawnType>(Context.GetInstigator());

	auto TargetCharacterPtr = Cast<FPawnType>(ExecutionParams.GetTargetAbilitySystemComponent()->GetOwnerActor());

	const auto InstigatorCharacterAttributesPtr = Instigator->GetCharacterAttributesComponent()->
	                                                          GetCharacterAttributes();

	const auto TargetCharacterAttributesPtr = TargetCharacterPtr->GetCharacterAttributesComponent()->
	                                                              GetCharacterAttributes();

	for (auto& Iter : NewDatas)
	{
		if (
			Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Damage_Metal) ||
			Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Damage_Wood) ||
			Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Damage_Water) ||
			Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Damage_Fire) ||
			Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Damage_Earth)
		)
		{
			const auto CriticalHitRate = InstigatorCharacterAttributesPtr->GetCriticalHitRate();
			const auto RandNum = FMath::RandRange(0, 100);
			if (RandNum <= CriticalHitRate)
			{
				AdditionalModifyAry.Add(EAdditionalModify::kIsCritical);

				const auto CriticalMagnification = InstigatorCharacterAttributesPtr->GetCriticalDamage() / 100;
				Iter.Value = Iter.Value * CriticalMagnification;
			}
		}
	}

	return false;
}

IInputData_ProbabilityConfirmation_ModifyInterface::IInputData_ProbabilityConfirmation_ModifyInterface(
	int32 InPriority
	):
	 IInputData_ModifyInterface_Base(InPriority)
{
}

bool IInputData_ProbabilityConfirmation_ModifyInterface::Modify(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput,
	TSet<FGameplayTag>& NeedModifySet,
	TMap<FGameplayTag, float>& NewDatas,
	TSet<EAdditionalModify>& AdditionalModifyAry
	)
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FGameplayEffectContextHandle Context = Spec.GetContext();
	auto Instigator = Cast<FPawnType>(Context.GetInstigator());

	auto TargetCharacterPtr = Cast<FPawnType>(ExecutionParams.GetTargetAbilitySystemComponent()->GetOwnerActor());

	const auto InstigatorCharacterAttributesPtr = Instigator->GetCharacterAttributesComponent()->
	                                                          GetCharacterAttributes();

	const auto TargetCharacterAttributesPtr = TargetCharacterPtr->GetCharacterAttributesComponent()->
	                                                              GetCharacterAttributes();

	for (auto& Iter : NewDatas)
	{
		if (
			Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Damage_Metal) ||
			Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Damage_Wood) ||
			Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Damage_Water) ||
			Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Damage_Fire) ||
			Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Damage_Earth)
		)
		{
			const auto HitRate = InstigatorCharacterAttributesPtr->GetHitRate();
			const auto Evade = InstigatorCharacterAttributesPtr->GetEvadeRate();

			const auto RandNum = FMath::RandRange(0, 100);
			if (RandNum <= (HitRate - Evade))
			{
			}
			else
			{
				AdditionalModifyAry.Add(EAdditionalModify::kIsEvade);
				Iter.Value = 0;
			}
		}
	}

	return false;
}

IInputData_BasicData_ModifyInterface::IInputData_BasicData_ModifyInterface(
	int32 InPriority
	):
	 IInputData_ModifyInterface_Base(InPriority)
{
}

bool IInputData_BasicData_ModifyInterface::Modify(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput,
	TSet<FGameplayTag>& NeedModifySet,
	TMap<FGameplayTag, float>& NewDatas,
	TSet<EAdditionalModify>& AdditionalModifyAry
	)
{
	const int32 BaseResistance = 100;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FGameplayEffectContextHandle Context = Spec.GetContext();
	auto Instigator = Cast<FPawnType>(Context.GetInstigator());

	auto TargetCharacterPtr = Cast<FPawnType>(ExecutionParams.GetTargetAbilitySystemComponent()->GetOwnerActor());

	const auto InstigatorCharacterAttributesPtr = Instigator->GetCharacterAttributesComponent()->
	                                                          GetCharacterAttributes();

	const auto TargetCharacterAttributesPtr = TargetCharacterPtr->GetCharacterAttributesComponent()->
	                                                              GetCharacterAttributes();

	auto Lambda = [this, InstigatorCharacterAttributesPtr, TargetCharacterAttributesPtr](
		TTuple<FGameplayTag, float>& Iter,
		const auto Penetration,
		const auto PercentPenetration,
		const auto Resistance
		)
	{
		auto ActulyResistance = FMath::Clamp(
		                                     Resistance - (Resistance * (PercentPenetration / 100.f)) - Penetration,
		                                     0,
		                                     TargetCharacterAttributesPtr->MaxElementalResistance
		                                    );

		const auto Percent = FMath::Clamp(ActulyResistance / (ActulyResistance + BaseResistance), 0, 1);

		Iter.Value = Iter.Value - (Iter.Value * Percent);
	};
	for (auto& Iter : NewDatas)
	{
		if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Damage_Metal))
		{
			Lambda(
			       Iter,
			       InstigatorCharacterAttributesPtr->GetMetalPenetration(),
			       InstigatorCharacterAttributesPtr->GetMetalPercentPenetration(),
			       TargetCharacterAttributesPtr->GetMetalResistance()
			      );
		}
		else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Damage_Wood))
		{
			Lambda(
			       Iter,
			       InstigatorCharacterAttributesPtr->GetWoodPenetration(),
			       InstigatorCharacterAttributesPtr->GetWoodPercentPenetration(),
			       TargetCharacterAttributesPtr->GetWoodResistance()
			      );
		}
		else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Damage_Water))
		{
			Lambda(
			       Iter,
			       InstigatorCharacterAttributesPtr->GetWaterPenetration(),
			       InstigatorCharacterAttributesPtr->GetWaterPercentPenetration(),
			       TargetCharacterAttributesPtr->GetWaterResistance()
			      );
		}
		else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Damage_Fire))
		{
			Lambda(
			       Iter,
			       InstigatorCharacterAttributesPtr->GetFirePenetration(),
			       InstigatorCharacterAttributesPtr->GetFirePercentPenetration(),
			       TargetCharacterAttributesPtr->GetFireResistance()
			      );
		}
		else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Damage_Earth))
		{
			Lambda(
			       Iter,
			       InstigatorCharacterAttributesPtr->GetEarthPenetration(),
			       InstigatorCharacterAttributesPtr->GetEarthPercentPenetration(),
			       TargetCharacterAttributesPtr->GetEarthResistance()
			      );
		}
	}

	return false;
}

IInputData_Shield_ModifyInterface::IInputData_Shield_ModifyInterface(
	int32 InPriority
	):
	 IInputData_ModifyInterface_Base(InPriority)
{
}

bool IInputData_Shield_ModifyInterface::Modify(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput,
	TSet<FGameplayTag>& NeedModifySet,
	TMap<FGameplayTag, float>& NewDatas,
	TSet<EAdditionalModify>& AdditionalModifyAry
	)
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FGameplayEffectContextHandle Context = Spec.GetContext();
	auto Instigator = Cast<FPawnType>(Context.GetInstigator());

	auto TargetCharacterPtr = Cast<FPawnType>(ExecutionParams.GetTargetAbilitySystemComponent()->GetOwnerActor());

	const auto InstigatorCharacterAttributesPtr = Instigator->GetCharacterAttributesComponent()->
	                                                          GetCharacterAttributes();

	const auto TargetCharacterAttributesPtr = TargetCharacterPtr->GetCharacterAttributesComponent()->
	                                                              GetCharacterAttributes();

	for (auto& Iter : NewDatas)
	{
		if (
			Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Damage_Metal) ||
			Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Damage_Wood) ||
			Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Damage_Water) ||
			Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Damage_Fire) ||
			Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Damage_Earth)
		)
		{
			if (Iter.Value <= 0)
			{
				continue;
			}

			const auto ShieldValue = TargetCharacterAttributesPtr->GetShield();
			if (ShieldValue <= 0)
			{
				return false;
			}

			NeedModifySet.Add(UGameplayTagsLibrary::GEData_ModifyItem_Shield);

			const auto Offset = Iter.Value - ShieldValue;
			if (Offset > 0)
			{
				TargetCharacterPtr->GetCharacterAbilitySystemComponent()->UpdateTemporaryValue(
					 UGameplayTagsLibrary::GEData_ModifyType_Temporary_Data_Override,
					 0,
					 EUpdateValueType::kTemporary_Data_Override,
					 UAS_Character::GetShieldAttribute().GetGameplayAttributeData(
						  TargetCharacterAttributesPtr
						 )
					);
				Iter.Value = Offset;
			}
			else
			{
				TargetCharacterPtr->GetCharacterAbilitySystemComponent()->UpdateTemporaryValue(
					 UGameplayTagsLibrary::GEData_ModifyType_Temporary_Data_Override,
					 -Iter.Value,
					 EUpdateValueType::kTemporary_Data_Addtive,
					 UAS_Character::GetShieldAttribute().GetGameplayAttributeData(
						  TargetCharacterAttributesPtr
						 )
					);
				Iter.Value = 0;
			}
		}
	}

	return false;
}

IGetValueGenericcModifyInterface::IGetValueGenericcModifyInterface(
	int32 InPriority
	):
	 IGetValueModifyInterface(InPriority)
{
}

int32 IGetValueGenericcModifyInterface::GetValue(
	const FDataComposition& DataComposition,
	int32 PreviouValue
	) const
{
	float Result = 0.f;

	auto& GameplayAttributeDataMap = DataComposition;

	// 
	for (const auto Iter : GameplayAttributeDataMap.DataMap)
	{
		Result += Iter.Value;
	}

	//
	const auto OriginlResult = Result;
	for (const auto Iter : GameplayAttributeDataMap.MagnitudeMap)
	{
		const auto Percent = Iter.Value / 100;
		Result += (OriginlResult * Percent);
	}

	// 去掉小数部分
	Result = UKismetMathLibrary::Round(Result);

	return Result;
}

IBasicGostModifyInterface::IBasicGostModifyInterface(
	int32 InPriority
	):
	 IGostModifyInterface(InPriority)
{
}

TMap<FGameplayTag, int32> IBasicGostModifyInterface::GetCost(
	const TMap<FGameplayTag, int32>& Original,
	const TMap<FGameplayTag, int32>& CurrentOriginal
	) const
{
	return Original;
}

IBasicDurationModifyInterface::IBasicDurationModifyInterface(
	int32 InPriority
	):
	 IDurationModifyInterface(InPriority)
{
}

float IBasicDurationModifyInterface::GetDuration(
	const UAS_Character* AS_CharacterAttributePtr,
	float Duration
	) const
{
	return Duration;
}

IBasicCooldownModifyInterface::IBasicCooldownModifyInterface(
	int32 InPriority
	):
	 ICooldownModifyInterface(InPriority)
{
}

int32 IBasicCooldownModifyInterface::GetCooldown(
	const UAS_Character* AS_CharacterAttributePtr,
	int32 Cooldown
	) const
{
	const auto Haste = AS_CharacterAttributePtr->GetHaste();

	const auto Scale = 1 + (Haste / 100.f);

	return Cooldown / Scale;
}

IOutputData_MultipleDamega_ModifyInterface::IOutputData_MultipleDamega_ModifyInterface(
	int32 InPriority,
	int32 InCount,
	float InMultiple
	):
	 IOutputData_ModifyInterface_Base(InPriority)
	 , Multiple(InMultiple)
	 , Count(InCount)
{
}

bool IOutputData_MultipleDamega_ModifyInterface::Modify(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput,
	TSet<FGameplayTag>& NeedModifySet,
	TMap<FGameplayTag, float>& NewDatas,
	TSet<EAdditionalModify>& AdditionalModifyAry
	)
{
	for (auto& Iter : NewDatas)
	{
		if (
			Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Damage_Metal) ||
			Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Damage_Wood) ||
			Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Damage_Water) ||
			Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Damage_Fire) ||
			Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Damage_Earth)
		)
		{
			const auto OldValue = CurrentCount;
			CurrentCount++;
			CallbackContainerHelper.ValueChanged(OldValue, CurrentCount);

			Iter.Value = Iter.Value * Multiple;
			if (CurrentCount >= Count)
			{
				return true;
			}
		}
	}

	return false;
}

IGostModify_ReplaceWithOther_Interface::IGostModify_ReplaceWithOther_Interface(
	int32 InPriority,
	FGameplayTag InCostAttributeTag,
	int32 InManaPercent,
	int32 InNewResourcePercent
	):
	 IGostModifyInterface(InPriority)
	 , CostAttributeTag(InCostAttributeTag)
	 , ManaPercent(InManaPercent)
	 , NewResourcePercent(InNewResourcePercent)
{
}

TMap<FGameplayTag, int32> IGostModify_ReplaceWithOther_Interface::GetCost(
	const TMap<FGameplayTag, int32>& Original,
	const TMap<FGameplayTag, int32>& CurrentOriginal
	) const
{
	TMap<FGameplayTag, int32> Result = CurrentOriginal;

	if (Original.Contains(UGameplayTagsLibrary::GEData_ModifyItem_Mana))
	{
		const auto ManeValue = Original[UGameplayTagsLibrary::GEData_ModifyItem_Mana];
		const auto OffsetValue = ManeValue * (ManaPercent / 100.f);
		Result.Add(UGameplayTagsLibrary::GEData_ModifyItem_Mana, ManeValue - OffsetValue);
		Result.Add(CostAttributeTag, OffsetValue * (100 / 100.f));
	}

	return Result;
}

IGostModify_Multiple_Interface::IGostModify_Multiple_Interface(
	int32 InPriority,
	float InMultiple
	):
	 IGostModifyInterface(InPriority)
	 , Multiple(InMultiple)
{
}

TMap<FGameplayTag, int32> IGostModify_Multiple_Interface::GetCost(
	const TMap<FGameplayTag, int32>& Original,
	const TMap<FGameplayTag, int32>& CurrentOriginal
	) const
{
	TMap<FGameplayTag, int32> Result = CurrentOriginal;

	if (Original.Contains(UGameplayTagsLibrary::GEData_ModifyItem_Mana))
	{
		const auto ManeValue = Original[UGameplayTagsLibrary::GEData_ModifyItem_Mana];
		const auto NewValue = ManeValue * Multiple;
		Result.Add(UGameplayTagsLibrary::GEData_ModifyItem_Mana, NewValue);
	}

	return Result;
}
