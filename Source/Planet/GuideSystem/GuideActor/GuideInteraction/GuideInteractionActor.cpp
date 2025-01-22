#include "GuideInteractionActor.h"

#include "GameplayTasksComponent.h"

#include "GuideSystemStateTreeComponent.h"
#include "STT_GuideInteraction.h"
#include "TaskNode_Guide.h"

bool UStateTreeGuideInteractionComponentSchema::IsStructAllowed(const UScriptStruct* InScriptStruct) const
{
	return Super::IsStructAllowed(InScriptStruct) ||
		InScriptStruct->IsChildOf(FSTT_GuideInteractionBase::StaticStruct());
}

TSubclassOf<UStateTreeSchema> UGuideInteractionSystemStateTreeComponent::GetSchema() const
{
	return UStateTreeGuideInteractionComponentSchema::StaticClass();
}

AGuideInteractionActor::AGuideInteractionActor(const FObjectInitializer& ObjectInitializer):
	Super(
		ObjectInitializer.SetDefaultSubobjectClass<UGuideInteractionSystemStateTreeComponent>(
			UGuideInteractionSystemStateTreeComponent::ComponentName
		)
	)
{
	GuideStateTreeComponentPtr->SetStartLogicAutomatically(true);
}
