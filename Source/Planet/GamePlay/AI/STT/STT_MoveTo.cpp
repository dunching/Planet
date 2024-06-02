
#include "STT_MoveTo.h"

#include "Tasks/StateTreeMoveToTask.h"
#include <Tasks/AITask_MoveTo.h>
#include <GameplayTask.h>

void FSTT_MoveToTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
#if WITH_EDITOR
	{
		FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
		if (InstanceData.MoveToTask && InstanceData.MoveToTask->GetState() != EGameplayTaskState::Finished)
		{
			UE_LOG(LogTemp, Log, TEXT("MoveToTask Failed"));
		}
	}
#endif

	Super::ExitState(Context, Transition);

	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.MoveToTask)
	{
		InstanceData.MoveToTask = nullptr;
	}
}
