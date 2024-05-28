
#include "GAEvent_Helper.h"

FGameplayAbilityTargetData_GAEvent * FGameplayAbilityTargetData_GAEvent::Clone() const
{
	FGameplayAbilityTargetData_GAEvent * ResultPtr = new FGameplayAbilityTargetData_GAEvent;

	*ResultPtr = *this;

	return ResultPtr;
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
