
#include "CS_Base.h"

FStateInfo::FStateInfo()
{

}

FGameplayAbilityTargetData_CS_Base::FGameplayAbilityTargetData_CS_Base(
	const FGameplayTag& InTag
) :
	Tag(InTag)
{

}

FGameplayAbilityTargetData_CS_Base::FGameplayAbilityTargetData_CS_Base()
{

}

FGameplayAbilityTargetData_CS_Base* FGameplayAbilityTargetData_CS_Base::Clone() const
{
	auto ResultPtr =
		new FGameplayAbilityTargetData_CS_Base;

	*ResultPtr = *this;

	return ResultPtr;
}
