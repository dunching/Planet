#include "DataModifyInterface.h"

#include "AS_Character.h"
#include "CharacterAbilitySystemComponent.h"
#include "CharacterAttributesComponent.h"
#include "CharacterBase.h"
#include "GameplayTagsLibrary.h"

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
	const TObjectPtr<ACharacterBase>& Instigator,
	const TObjectPtr<ACharacterBase>& TargetCharacterPtr,
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
	const TObjectPtr<ACharacterBase>& Instigator,
	const TObjectPtr<ACharacterBase>& TargetCharacterPtr,
	TSet<FGameplayTag>& NeedModifySet,
	TMap<FGameplayTag, float>& NewDatas,
	TSet<EAdditionalModify>& AdditionalModifyAry
	)
{
	return true;
}

IOutputData_ProbabilityConfirmation_ModifyInterface::IOutputData_ProbabilityConfirmation_ModifyInterface(
	int32 InPriority
	):
	 IOutputDataModifyInterface(InPriority)
{
}

bool IOutputData_ProbabilityConfirmation_ModifyInterface::Modify(
	const TObjectPtr<ACharacterBase>& Instigator,
	const TObjectPtr<ACharacterBase>& TargetCharacterPtr,
	TSet<FGameplayTag>& NeedModifySet,
	TMap<FGameplayTag, float>& NewDatas,
	TSet<EAdditionalModify>& AdditionalModifyAry
	)
{
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
	 IInputDataModifyInterface(InPriority)
{
}

bool IInputData_ProbabilityConfirmation_ModifyInterface::Modify(
	const TObjectPtr<ACharacterBase>& Instigator,
	const TObjectPtr<ACharacterBase>& TargetCharacterPtr,
	TSet<FGameplayTag>& NeedModifySet,
	TMap<FGameplayTag, float>& NewDatas,
	TSet<EAdditionalModify>& AdditionalModifyAry
	)
{
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
	 IInputDataModifyInterface(InPriority)
{
}

bool IInputData_BasicData_ModifyInterface::Modify(
	const TObjectPtr<ACharacterBase>& Instigator,
	const TObjectPtr<ACharacterBase>& TargetCharacterPtr,
	TSet<FGameplayTag>& NeedModifySet,
	TMap<FGameplayTag, float>& NewDatas,
	TSet<EAdditionalModify>& AdditionalModifyAry
	)
{
	const int32 BaseResistance = 100;

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
	 IInputDataModifyInterface(InPriority)
{
}

bool IInputData_Shield_ModifyInterface::Modify(
	const TObjectPtr<ACharacterBase>& Instigator,
	const TObjectPtr<ACharacterBase>& TargetCharacterPtr,
	TSet<FGameplayTag>& NeedModifySet,
	TMap<FGameplayTag, float>& NewDatas,
	TSet<EAdditionalModify>& AdditionalModifyAry
	)
{
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
				TargetCharacterPtr->GetCharacterAbilitySystemComponent()->UpdateMapTemporary(
					 UGameplayTagsLibrary::GEData_ModifyType_BaseValue_Override,
					 0,
					 UAS_Character::GetShieldAttribute().GetGameplayAttributeData(
						  TargetCharacterAttributesPtr
						 )
					);
				Iter.Value = Offset;
			}
			else
			{
				TargetCharacterPtr->GetCharacterAbilitySystemComponent()->UpdateMapTemporary(
					 UGameplayTagsLibrary::GEData_ModifyType_BaseValue_Addtive,
					 -Iter.Value,
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

IOutputData_MultipleDamega_ModifyInterface::IOutputData_MultipleDamega_ModifyInterface(
	int32 InPriority,
	int32 InCount,
	float InMultiple
	):
	 IOutputDataModifyInterface(InPriority)
	 , Multiple(InMultiple)
	 , Count(InCount)
{
}

bool IOutputData_MultipleDamega_ModifyInterface::Modify(
	const TObjectPtr<ACharacterBase>& Instigator,
	const TObjectPtr<ACharacterBase>& TargetCharacterPtr,
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
