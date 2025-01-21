#include "TaskNode_Guide.h"

#include "StateTreeExecutionTypes.h"
#include "Engine/TargetPoint.h"

#include "GeneratorColony.h"
#include "CharacterBase.h"

#include "Net/UnrealNetwork.h"

#include "HumanAIController.h"
#include "HumanCharacter_AI.h"
#include "HumanCharacter_Player.h"

UPAD_TaskNode_Guide_MoveToLocation::UPAD_TaskNode_Guide_MoveToLocation(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	TaskNodeType = ETaskNodeType::kGuide_MoveToPoint;
}

#if WITH_EDITOR
bool UPAD_TaskNode_Guide_MoveToLocation::Modify(bool bAlwaysMarkDirty)
{
	return Super::Modify(bAlwaysMarkDirty);
}

void UPAD_TaskNode_Guide_MoveToLocation::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (TargetPointPtr.IsValid())

		SetTargetPoint();
}
#endif

void UPAD_TaskNode_Guide_MoveToLocation::PostLoad()
{
	Super::PostLoad();

#if WITH_EDITOR
	// SetTargetPoint();
#endif
}

void UPAD_TaskNode_Guide_MoveToLocation::SetTargetPoint()
{
	if (TargetPointPtr.IsValid())
	{
		TargetLocation = TargetPointPtr->GetActorLocation();
	}
	else
	{
		TargetLocation = FVector::ZeroVector;
	}
}

UPAD_TaskNode_Guide_PressKey::UPAD_TaskNode_Guide_PressKey(const FObjectInitializer& ObjectInitializer)
{
	TaskNodeType = ETaskNodeType::kGuide_PressKey;
}

UPAD_TaskNode_Guide_Monologue::UPAD_TaskNode_Guide_Monologue(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	TaskNodeType = ETaskNodeType::kGuide_Monologue;
}

UPAD_TaskNode_Guide_AddToTarget::UPAD_TaskNode_Guide_AddToTarget(const FObjectInitializer& ObjectInitializer):
Super(ObjectInitializer)
{
	TaskNodeType = ETaskNodeType::kGuide_AddToTarget;
}

UPAD_TaskNode_Guide_ConversationWithTarget::UPAD_TaskNode_Guide_ConversationWithTarget(
const FObjectInitializer& ObjectInitializer):
Super(ObjectInitializer)
{
	TaskNodeType = ETaskNodeType::kGuide_ConversationWithTarget;
}
