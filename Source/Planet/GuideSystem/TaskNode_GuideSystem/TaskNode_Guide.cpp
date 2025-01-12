#include "TaskNode_Guide.h"

#include "StateTreeExecutionTypes.h"
#include "Engine/TargetPoint.h"

#include "GeneratorColony.h"
#include "CharacterBase.h"

#include "Net/UnrealNetwork.h"

#include "HumanAIController.h"
#include "HumanCharacter_Player.h"

FString UPAD_TaskNode_Guide::GetDescription() const
{
	return TEXT("");
}

UPAD_TaskNode_Guide_MoveToLocation::UPAD_TaskNode_Guide_MoveToLocation(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	TaskNodeType = ETaskNodeType::kWorldProcess_MoveToPoint;
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

FString UPAD_TaskNode_Guide_MoveToLocation::GetDescription() const
{
	return Description;
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
