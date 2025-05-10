
#include "STT_GuideBase.h"

FSTID_GuideBase::FSTID_GuideBase()
{
	TaskID = FGuid::NewGuid();
}

FSTID_Guide_Record::FSTID_Guide_Record()
{
	TaskID = FGuid::NewGuid();
}

const UStruct* FSTT_Guide_Record::GetInstanceDataType() const
{
	return FInstanceDataType::StaticStruct();
}

EStateTreeRunStatus FSTT_Guide_Record::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
	) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
															  *this
															 );

	if (InstanceData.GuideActorPtr)
	{
		// 读取任务进度
		const auto PreviousTaskID = InstanceData.GuideActorPtr->GetPreviousTaskID();
		if (PreviousTaskID.IsValid())
		{
			if (PreviousTaskID == InstanceData.TaskID)
			{
				InstanceData.GuideActorPtr->SetPreviousTaskID(
															  FGuid()
															 );

				return Super::EnterState(
										 Context,
										 Transition
										);
			}

			return EStateTreeRunStatus::Succeeded;
		}
	}
	else
	{
		return EStateTreeRunStatus::Failed;
	}

	// 记录当前的任务ID，
	InstanceData.GuideActorPtr->SetCurrentTaskID(
												 InstanceData.TaskID
												);

	return Super::EnterState(
							 Context,
							 Transition
							);
}
