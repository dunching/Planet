
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

IGAEventModifyInterface::IGAEventModifyInterface(int32 InPriority) :
	Priority(InPriority)
{

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

void FGAEventData::SetBaseDamage(int32 Value)
{
	BaseDamage = Value;
}

void FGAEventData::AddWuXingDamage(EWuXingType WuXingType, int32 Value)
{
	const auto& CharacterAttributes =
		TriggerCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();

	int32 Level = 0;
	switch (WuXingType)
	{
	case EWuXingType::kGold:
	{
		Level = CharacterAttributes.Element.GoldElement.GetCurrentValue();
	}
	break;
	case EWuXingType::kWood:
	{
		Level = CharacterAttributes.Element.GoldElement.GetCurrentValue();
	}
	break;
	case EWuXingType::kWater:
	{
		Level = CharacterAttributes.Element.GoldElement.GetCurrentValue();
	}
	break;
	case EWuXingType::kFire:
	{
		Level = CharacterAttributes.Element.GoldElement.GetCurrentValue();
	}
	break;
	case EWuXingType::kSoil:
	{
		Level = CharacterAttributes.Element.GoldElement.GetCurrentValue();
	}
	break;
	}
	const auto Tuple = MakeTuple(
		WuXingType,
		Level,
		Value
	);
	ElementSet.Add(Tuple);
}

FGameplayAbilityTargetData_GAReceivedEvent::FGameplayAbilityTargetData_GAReceivedEvent(
	TWeakObjectPtr<ACharacterBase>  InTargetCharacterPtr,
	TWeakObjectPtr<ACharacterBase>  InTriggerCharacterPtr
) :
	Data(InTargetCharacterPtr, InTriggerCharacterPtr),
	TriggerCharacterPtr(InTriggerCharacterPtr)
{

}

FGameplayAbilityTargetData_GAReceivedEvent* FGameplayAbilityTargetData_GAReceivedEvent::Clone() const
{
	auto ResultPtr = 
		new FGameplayAbilityTargetData_GAReceivedEvent(Data.TargetCharacterPtr, TriggerCharacterPtr.Get());

	*ResultPtr = *this;

	return ResultPtr;
}

IGAEventModifySendInterface::IGAEventModifySendInterface(int32 InPriority /*= 1*/) :
	IGAEventModifyInterface(InPriority)
{

}

void IGAEventModifySendInterface::Modify(FGameplayAbilityTargetData_GASendEvent& OutGameplayAbilityTargetData_GAEvent)
{

}

IGAEventModifyReceivedInterface::IGAEventModifyReceivedInterface(int32 InPriority /*= 1*/) :
	IGAEventModifyInterface(InPriority)
{

}

void IGAEventModifyReceivedInterface::Modify(FGameplayAbilityTargetData_GAReceivedEvent& OutGameplayAbilityTargetData_GAEvent)
{

}

FGameplayAbilityTargetData_GAEventType::FGameplayAbilityTargetData_GAEventType(EEventType InEventType) :
	EventType(InEventType)
{

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
