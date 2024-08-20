
#include "GAEvent_Base.h"

UGAEvent_Base::UGAEvent_Base() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
}

const FGameplayEventData& UGAEvent_Base::GetCurrentEventData() const
{
	return CurrentEventData;
}

