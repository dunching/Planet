#include "GameplayTask_QuestBase.h"

void UGameplayTask_QuestBase::SetTaskID(
	const FGuid& InTaskID
	)
{
	TaskID = InTaskID;
}


EStateTreeRunStatus UGameplayTask_QuestBase::GetStateTreeRunStatus() const
{
	return StateTreeRunStatus;
}
