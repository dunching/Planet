
#include "GAEvent_Helper.h"

#include "CharacterBase.h"
#include "CharacterAttributesComponent.h"
#include "CharacterAttibutes.h"

FGameplayAbilityTargetData_GASendEvent* FGameplayAbilityTargetData_GASendEvent::Clone() const
{
	FGameplayAbilityTargetData_GASendEvent* ResultPtr = new FGameplayAbilityTargetData_GASendEvent(TriggerCharacterPtr.Get());

	*ResultPtr = *this;

	return ResultPtr;
}

FGameplayAbilityTargetData_GASendEvent::FGameplayAbilityTargetData_GASendEvent(
	TWeakObjectPtr<ACharacterBase>  InTriggerCharacterPtr
) :
	TriggerCharacterPtr(InTriggerCharacterPtr)
{
}

UScriptStruct* FGameplayAbilityTargetData_GASendEvent::GetScriptStruct() const
{
	return FGameplayAbilityTargetData_GASendEvent::StaticStruct();
}

bool FGameplayAbilityTargetData_GASendEvent::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	if (Ar.IsSaving())
	{
		Ar << TriggerCharacterName;

		int32 Num = DataAry.Num();

		Ar << Num;

		for (auto& Iter : DataAry)
		{
			Iter.NetSerialize(Ar, Map, bOutSuccess);
		}

		return true;
	}
	else if (Ar.IsLoading())
	{
		Ar << TriggerCharacterName;

		int32 Num = 0;

		Ar << Num;

		DataAry.SetNumZeroed(Num);
		for (int32 Index = 0; Index < Num; Index++)
		{
			DataAry[Index].NetSerialize(Ar, Map, bOutSuccess);
		}

		return true;
	}

	return false;
}

FGameplayAbilityTargetData_GASendEvent::FGameplayAbilityTargetData_GASendEvent()
{

}

IGAEventModifyInterface::IGAEventModifyInterface(int32 InPriority) :
	Priority(InPriority)
{
	ID = FMath::Rand32();
}

bool IGAEventModifyInterface::operator<(const IGAEventModifyInterface& RightValue)const
{
	return (Priority > RightValue.Priority) && (ID == RightValue.ID);
}

FGAEventData::FGAEventData(
	TWeakObjectPtr<ACharacterBase>  InTargetCharacterPtr,
	TWeakObjectPtr<ACharacterBase>  InTriggerCharacterPtr
) :
	TriggerCharacterPtr(InTriggerCharacterPtr),
	TargetCharacterPtr(InTargetCharacterPtr)
{

}

bool FGAEventData::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	Ar << bIsWeaponAttack;
	Ar << AttackEffectType;
	Ar << RepelDirection;
	Ar << RepelDistance;
	Ar << bIsRespawn;
	Ar << bIsCantEvade;

	Ar << AD_Penetration;
	Ar << AD_PercentPenetration;
	Ar << HitRate;
	Ar << CriticalHitRate;
	Ar << CriticalDamage;

	Ar << TrueDamage;
	Ar << BaseDamage;

	Ar << DataSource;

	Ar << TriggerCharacterPtr;
	Ar << TargetCharacterPtr;

	if (Ar.IsSaving())
	{
		int32 Num = DataModify.Num();
		Ar << Num;
		for (auto Iter : DataModify)
		{
			Ar << Iter.Key;
			Iter.Value.NetSerialize(Ar, Map, bOutSuccess);
		}
	}
	else if (Ar.IsLoading())
	{
		int32 Num = 0;
		Ar << Num;
		for (int32 Index = 0; Index < Num; Index++)
		{
			ECharacterPropertyType Key;
			FBaseProperty Value;
			Ar << Key;
			Value.NetSerialize(Ar, Map, bOutSuccess);

			DataModify.Add(Key, Value);
		}
	}

	return true;
}

FGAEventData::FGAEventData()
{

}

void FGAEventData::SetBaseDamage(int32 Value)
{
	BaseDamage = Value;
}

void FGAEventData::AddWuXingDamage(EWuXingType WuXingType, int32 Value)
{
	const auto CharacterAttributesComponentPtr = TriggerCharacterPtr->GetCharacterAttributesComponent();
	// auto& CharacterAttributes =
	// 	CharacterAttributesComponentPtr->GetCharacterAttributes();
	//
	// int32 Level = 0;
	// switch (WuXingType)
	// {
	// case EWuXingType::kGold:
	// {
	// 	Level = CharacterAttributes.GoldElement.GetCurrentValue();
	// }
	// break;
	// case EWuXingType::kWood:
	// {
	// 	Level = CharacterAttributes.GoldElement.GetCurrentValue();
	// }
	// break;
	// case EWuXingType::kWater:
	// {
	// 	Level = CharacterAttributes.GoldElement.GetCurrentValue();
	// }
	// break;
	// case EWuXingType::kFire:
	// {
	// 	Level = CharacterAttributes.GoldElement.GetCurrentValue();
	// }
	// break;
	// case EWuXingType::kSoil:
	// {
	// 	Level = CharacterAttributes.GoldElement.GetCurrentValue();
	// }
	// break;
	// }
	// const auto Tuple = MakeTuple(
	// 	WuXingType,
	// 	Level,
	// 	Value
	// );
	// ElementSet.Add(Tuple);
}

bool FGAEventData::GetIsHited() const
{
	return HitRate >= 100;
}

bool FGAEventData::GetIsCriticalHited() const
{
	return CriticalHitRate >= 100;
}

FGameplayAbilityTargetData_GAReceivedEvent::FGameplayAbilityTargetData_GAReceivedEvent()
{
}

FGameplayAbilityTargetData_GAReceivedEvent::FGameplayAbilityTargetData_GAReceivedEvent(
	TWeakObjectPtr<ACharacterBase>  InTargetCharacterPtr,
	TWeakObjectPtr<ACharacterBase>  InTriggerCharacterPtr
) :
	Data(InTargetCharacterPtr, InTriggerCharacterPtr),
	TriggerCharacterPtr(InTriggerCharacterPtr)
{

}

UScriptStruct* FGameplayAbilityTargetData_GAReceivedEvent::GetScriptStruct() const
{
	return FGameplayAbilityTargetData_GAReceivedEvent::StaticStruct();
}

bool FGameplayAbilityTargetData_GAReceivedEvent::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Data.NetSerialize(Ar, Map, bOutSuccess);

	Ar << TriggerCharacterPtr;

	return true;
}

FGameplayAbilityTargetData_GAReceivedEvent* FGameplayAbilityTargetData_GAReceivedEvent::Clone() const
{
	auto ResultPtr =
		new FGameplayAbilityTargetData_GAReceivedEvent(Data.TargetCharacterPtr, TriggerCharacterPtr.Get());

	*ResultPtr = *this;

	return ResultPtr;
}

TSharedPtr<FGameplayAbilityTargetData_GAReceivedEvent> FGameplayAbilityTargetData_GAReceivedEvent::Clone_SmartPtr() const
{
	return TSharedPtr<FGameplayAbilityTargetData_GAReceivedEvent>(Clone());
}

IGAEventModifySendInterface::IGAEventModifySendInterface(int32 InPriority /*= 1*/) :
	IGAEventModifyInterface(InPriority)
{

}

bool IGAEventModifySendInterface::Modify(
		TMap<FGameplayTag, float>&	SetByCallerTagMagnitudes
		)
{
	return true;
}

IGAEventModifyReceivedInterface::IGAEventModifyReceivedInterface(int32 InPriority /*= 1*/) :
	IGAEventModifyInterface(InPriority)
{

}

bool IGAEventModifyReceivedInterface::Modify(
		TMap<FGameplayTag, float>&	SetByCallerTagMagnitudes
	)
{
	return true;
}

FGameplayAbilityTargetData_GAEventType::FGameplayAbilityTargetData_GAEventType(EEventType InEventType) :
	EventType(InEventType)
{

}

UScriptStruct* FGameplayAbilityTargetData_GAEventType::GetScriptStruct() const
{
	return FGameplayAbilityTargetData_GAEventType::StaticStruct();
}

FGameplayAbilityTargetData_GAEventType::FGameplayAbilityTargetData_GAEventType()
{

}

FGameplayAbilityTargetData_GAEventType* FGameplayAbilityTargetData_GAEventType::Clone() const
{
	auto ResultPtr =
		new FGameplayAbilityTargetData_GAEventType;

	*ResultPtr = *this;

	return ResultPtr;
}

bool FGameplayAbilityTargetData_GAEventType::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	if (Ar.IsSaving())
	{
		Ar << EventType;
		return true;
	}
	else if (Ar.IsLoading())
	{
		Ar << EventType;
		return true;
	}
	return false;
}
