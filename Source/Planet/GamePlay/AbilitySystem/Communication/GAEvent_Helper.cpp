
#include "GAEvent_Helper.h"

#include "CharacterBase.h"
#include "CharacterAttributesComponent.h"
#include "CharacterAttibutes.h"

FGameplayAbilityTargetData_GAEvent* FGameplayAbilityTargetData_GAEvent::Clone() const
{
	FGameplayAbilityTargetData_GAEvent* ResultPtr = new FGameplayAbilityTargetData_GAEvent(TriggerCharacterPtr.Get());

	*ResultPtr = *this;

	return ResultPtr;
}

FGameplayAbilityTargetData_GAEvent::FGameplayAbilityTargetData_GAEvent(
	ACharacterBase* InTriggerCharacterPtr
) :
	TriggerCharacterPtr(InTriggerCharacterPtr)
{
	Data.TriggerCharacterPtr = TriggerCharacterPtr;
}

IGAEventModifyInterface::IGAEventModifyInterface(int32 InPriority) :
	Priority(InPriority)
{

}

void IGAEventModifyInterface::Modify(FGameplayAbilityTargetData_GAEvent& OutGameplayAbilityTargetData_GAEvent)
{

}

bool IGAEventModifyInterface::operator<(const IGAEventModifyInterface& RightValue)const
{
	return (Priority > RightValue.Priority) && (ID == RightValue.ID);
}

void FGameplayAbilityTargetData_GAEvent::FData::SetBaseDamage(int32 Value)
{
	BaseDamage = Value;
}

void FGameplayAbilityTargetData_GAEvent::FData::SetWuXingDamage(EWuXingType WuXingType, int32 Value)
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
