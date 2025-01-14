#include "GuideActor.h"

#include "GameplayTasksComponent.h"

#include "GuideSystemStateTreeComponent.h"
#include "TaskNode_Guide.h"

AGuideActor::AGuideActor(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	bAlwaysRelevant = true;
	SetReplicatingMovement(false);
	NetUpdateFrequency = 1;

	GuideStateTreeComponentPtr = CreateDefaultSubobject<UGuideSystemStateTreeComponent>(UGuideSystemStateTreeComponent::ComponentName);
	
	GameplayTasksComponentPtr = CreateDefaultSubobject<UGameplayTasksComponent>(TEXT("GameplayTasksComponent"));
}

UGameplayTasksComponent* AGuideActor::GetGameplayTasksComponent() const
{
	return GameplayTasksComponentPtr;
}

UGuideSystemStateTreeComponent* AGuideActor::GetGuideSystemStateTreeComponent() const
{
	return GuideStateTreeComponentPtr;
}

void AGuideActor::UpdateCurrentTaskNode(const TSoftObjectPtr<UPAD_TaskNode_Guide>& InTaskNode)
{
	TaskNodeRef = InTaskNode;
	OnCurrentTaskNodeChanged.Broadcast(TaskNodeRef);
}

AGuideInteractionActor::AGuideInteractionActor(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	GuideStateTreeComponentPtr->SetStartLogicAutomatically(true);
}
