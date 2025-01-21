#include "GuideThreadActor.h"

#include "GameplayTasksComponent.h"

#include "GuideSystemStateTreeComponent.h"
#include "TaskNode_Guide.h"

void AGuideThread::UpdateCurrentTaskNode(const TSoftObjectPtr<UPAD_TaskNode_Guide>& InTaskNode)
{
	TaskNodeRef = InTaskNode;
	OnCurrentTaskNodeChanged.Broadcast(TaskNodeRef);
}

void AGuideThread::AddEvent(const FTaskNodeResuleHelper& TaskNodeResuleHelper)
{
	EventsSet.Add(TaskNodeResuleHelper.TaskId,TaskNodeResuleHelper);
}

FTaskNodeResuleHelper AGuideThread::ConsumeEvent(const FGuid& InGuid)
{
	if (EventsSet.Contains(InGuid))
	{
		EventsSet.Remove(InGuid);
		return EventsSet[InGuid];
	}
	return FTaskNodeResuleHelper();
}
