#include "GuideThreadActor.h"

#include "GameplayTasksComponent.h"
#include "StateTreeConditionBase.h"
#include "STT_GuideThread.h"

#include "GuideSystemStateTreeComponent.h"
#include "TaskNode_Guide.h"

bool FTaskNodeResuleHelper::GetIsValid() const
{
	return TaskID.IsValid();
}

bool UStateTreeGuideThreadComponentSchema::IsStructAllowed(const UScriptStruct* InScriptStruct) const
{
	return Super::IsStructAllowed(InScriptStruct) ||
		InScriptStruct->IsChildOf(FSTT_GuideThreadBase::StaticStruct());
}

TSubclassOf<UStateTreeSchema> UGuideThreadSystemStateTreeComponent::GetSchema() const
{
	return UStateTreeGuideThreadComponentSchema::StaticClass();
}

AGuideThread::AGuideThread(const FObjectInitializer& ObjectInitializer):
	Super(
		ObjectInitializer.SetDefaultSubobjectClass<UGuideThreadSystemStateTreeComponent>(
			UGuideThreadSystemStateTreeComponent::ComponentName
		)
	)
{
}

void AGuideThread::UpdateCurrentTaskNode(const TSoftObjectPtr<UPAD_TaskNode_Guide>& InTaskNode)
{
	// TaskNodeRef = InTaskNode;
	// OnCurrentTaskNodeChanged.Broadcast(TaskNodeRef);
}

void AGuideThread::UpdateCurrentTaskNode(const FTaskNodeDescript& TaskNodeDescript)
{
	CurrentTaskNodeDescript = TaskNodeDescript;
	OnCurrentTaskNodeChanged.Broadcast(CurrentTaskNodeDescript);
}

void AGuideThread::AddEvent(const FTaskNodeResuleHelper& TaskNodeResuleHelper)
{
	EventsSet.Add(TaskNodeResuleHelper.TaskID, TaskNodeResuleHelper);
}

FTaskNodeResuleHelper AGuideThread::ConsumeEvent(const FGuid& InGuid)
{
	FTaskNodeResuleHelper Result;
	if (EventsSet.Contains(InGuid))
	{
		Result = EventsSet[InGuid];
		EventsSet.Remove(InGuid);
	}
	return Result;
}
