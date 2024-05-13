// Copyright Epic Games, Inc. All Rights Reserved.

#include "AITask_DashToLeader.h"

#include "UObject/Package.h"
#include "TimerManager.h"
#include "AISystem.h"
#include "AIController.h"
#include "VisualLogger/VisualLogger.h"
#include "AIResources.h"
#include "GameplayTasksComponent.h"

UAITask_DashToLeader::UAITask_DashToLeader(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsPausable = true;

	bTickingTask = true;
}

void UAITask_DashToLeader::Activate()
{
	Super::Activate();

	ConditionalPerformMove();
}

void UAITask_DashToLeader::TickTask(float DeltaTime)
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

void UAITask_DashToLeader::ConditionalPerformMove()
{
	PerformMove();
}

bool UAITask_DashToLeader::WasMoveSuccessful() const
{
	return LerpTime < LerpDuration;
}

void UAITask_DashToLeader::SetUp(AAIController* InController, const FVector& InGoalLocation)
{
	ControllerPtr = InController;
	GoalLocation = InGoalLocation;
}

void UAITask_DashToLeader::PerformMove()
{

}

void UAITask_DashToLeader::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);
}
