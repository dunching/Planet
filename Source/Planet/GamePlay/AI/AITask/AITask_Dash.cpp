// Copyright Epic Games, Inc. All Rights Reserved.

#include "AITask_Dash.h"

#include "UObject/Package.h"
#include "TimerManager.h"
#include "AISystem.h"
#include "AIController.h"
#include "VisualLogger/VisualLogger.h"
#include "AIResources.h"
#include "GameplayTasksComponent.h"

UAITask_Dash::UAITask_Dash(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsPausable = true;

	bTickingTask = true;
}

void UAITask_Dash::Activate()
{
	Super::Activate();

	ConditionalPerformTask();
}

void UAITask_Dash::TickTask(float DeltaTime)
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

void UAITask_Dash::ConditionalPerformTask()
{
	PerformTask();
}

bool UAITask_Dash::WasMoveSuccessful() const
{
// 	auto PawnPtr = ControllerPtr->GetPawn();
// 	if (PawnPtr)
// 	{
// 		auto Offset = GoalLocation - PawnPtr->GetActorLocation();
// 
// 		return Offset.IsNearlyZero();
// 	}

	return LerpTime < LerpDuration;
}

void UAITask_Dash::SetUp(AAIController* InController, const FVector& InGoalLocation)
{
	ControllerPtr = InController;
	GoalLocation = InGoalLocation;
}

void UAITask_Dash::PerformTask()
{

}

void UAITask_Dash::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);
}
