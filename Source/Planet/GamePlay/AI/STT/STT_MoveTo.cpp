
#include "STT_MoveTo.h"

void FSTT_MoveToTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	Super::ExitState(Context, Transition);

	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.MoveToTask)
	{
		InstanceData.MoveToTask = nullptr;
	}
}
