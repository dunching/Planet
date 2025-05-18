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
	TMap<FGameplayTag, float>& SetByCallerTagMagnitudes
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
	TMap<FGameplayTag, float>& SetByCallerTagMagnitudes
	)
{
	return true;
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
	TMap<FGameplayTag, float>& SetByCallerTagMagnitudes
	)
{
	const int32 BaseResistance = 100;

	const auto InstigatorCharacterAttributesPtr = Instigator->GetCharacterAttributesComponent()->
	                                                          GetCharacterAttributes();
	const auto TargetCharacterAttributesPtr = TargetCharacterPtr->GetCharacterAttributesComponent()->
	                                                              GetCharacterAttributes();
	for (auto& Iter : SetByCallerTagMagnitudes)
	{
		if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Damage_Metal))
		{
			const auto Penetration = InstigatorCharacterAttributesPtr->GetMetalPenetration();
			const auto PercentPenetration = InstigatorCharacterAttributesPtr->GetMetalPercentPenetration();

			const auto Resistance = TargetCharacterAttributesPtr->GetMetalResistance();

			auto ActulyResistance = FMath::Clamp(
			                                     Resistance - (Resistance * (PercentPenetration / 100.f)) - Penetration,
			                                     0,
			                                     TargetCharacterAttributesPtr->MaxElementalResistance
			                                    );

			const auto Percent = FMath::Clamp(ActulyResistance / (ActulyResistance + BaseResistance), 0, 1);

			Iter.Value = Iter.Value - (Iter.Value * Percent);
		}
		else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Damage_Wood))
		{
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
	TMap<FGameplayTag, float>& SetByCallerTagMagnitudes
	)
{
	const auto InstigatorCharacterAttributesPtr = Instigator->GetCharacterAttributesComponent()->
	                                                          GetCharacterAttributes();
	const auto TargetCharacterAttributesPtr = TargetCharacterPtr->GetCharacterAttributesComponent()->
	                                                              GetCharacterAttributes();
	for (auto& Iter : SetByCallerTagMagnitudes)
	{
		if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Damage_Metal))
		{
			const auto ShieldValue = TargetCharacterAttributesPtr->GetShield();
			if (ShieldValue <= 0)
			{
				return false;
			}

			NeedModifySet.Add(UGameplayTagsLibrary::GEData_ModifyItem_Shield);
			
			const auto Offset = Iter.Value - ShieldValue;
			if (Offset > 0)
			{
				Iter.Value = Offset;
				TargetCharacterPtr->GetCharacterAbilitySystemComponent()->UpdateMapTemporary(
					 UGameplayTagsLibrary::GEData_ModifyType_BaseValue_Override,
					 0,
					 UAS_Character::GetShieldAttribute().GetGameplayAttributeData(
						  TargetCharacterAttributesPtr
						 )
					);
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
		else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Damage_Wood))
		{
		}
	}

	return false;
}
