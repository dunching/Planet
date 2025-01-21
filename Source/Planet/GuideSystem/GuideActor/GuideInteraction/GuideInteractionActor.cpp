#include "GuideInteractionActor.h"

#include "GameplayTasksComponent.h"

#include "GuideSystemStateTreeComponent.h"
#include "TaskNode_Guide.h"

AGuideInteractionActor::AGuideInteractionActor(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	GuideStateTreeComponentPtr->SetStartLogicAutomatically(true);
}
