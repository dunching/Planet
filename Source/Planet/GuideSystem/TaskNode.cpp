#include "TaskNode.h"

#include "GeneratorColony.h"
#include "CharacterBase.h"

#include "Net/UnrealNetwork.h"

#include "HumanAIController.h"

void UPAD_TaskNode_Preset::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, TaskNodeType, COND_None);
	DOREPLIFETIME_CONDITION(ThisClass, TaskNodeState, COND_None);
}

void UTaskNode_Temporary::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(ThisClass, TaskNodeType, COND_None);
}
