// Copyright Epic Games, Inc. All Rights Reserved.

#include "AITask_ReleaseSkill.h"

#include "UObject/Package.h"
#include "TimerManager.h"
#include "AISystem.h"
#include "AIController.h"
#include "VisualLogger/VisualLogger.h"
#include "AIResources.h"
#include "GameplayTasksComponent.h"

UAITask_ReleaseSkill::UAITask_ReleaseSkill(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsPausable = true;

	bTickingTask = true;
}

void UAITask_ReleaseSkill::Activate()
{
	Super::Activate();

	ConditionalPerformMove();
}

void UAITask_ReleaseSkill::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	LerpTime += DeltaTime;

	auto PawnPtr = ControllerPtr->GetPawn();
	if (PawnPtr)
	{
		auto Offset = GoalLocation - PawnPtr->GetActorLocation();

		Offset = Offset * (LerpTime / LerpDuration);

		PawnPtr->SetActorLocation(PawnPtr->GetActorLocation() + Offset);
	}

	if (LerpTime >= LerpDuration)
	{
		EndTask();
	}
}

void UAITask_ReleaseSkill::ConditionalPerformMove()
{
	PerformMove();
}

bool UAITask_ReleaseSkill::WasMoveSuccessful() const
{
	return LerpTime < LerpDuration;
}

void UAITask_ReleaseSkill::SetUp(AAIController* InController, const FVector& InGoalLocation)
{
	ControllerPtr = InController;
	GoalLocation = InGoalLocation;
}

void UAITask_ReleaseSkill::PerformMove()
{

}

void UAITask_ReleaseSkill::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);
}
