#include "GameplayTask_GuideTask_Base.h"

void UGameplayTask_GuideTask_Base::SetTaskID(
	const FGuid& InTaskID
	)
{
	TaskID = InTaskID;
}


EStateTreeRunStatus UGameplayTask_GuideTask_Base::GetStateTreeRunStatus() const
{
	return StateTreeRunStatus;
}
