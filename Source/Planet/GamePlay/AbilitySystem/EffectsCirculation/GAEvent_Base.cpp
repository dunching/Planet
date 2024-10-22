
#include "GAEvent_Base.h"

UGAEvent_Base::UGAEvent_Base() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;

	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

const FGameplayEventData& UGAEvent_Base::GetCurrentEventData() const
{
	return CurrentEventData;
}

