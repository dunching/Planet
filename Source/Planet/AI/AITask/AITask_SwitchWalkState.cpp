// Copyright Epic Games, Inc. All Rights Reserved.

#include "AITask_SwitchWalkState.h"

#include "UObject/Package.h"
#include "TimerManager.h"
#include "AISystem.h"
#include "AIController.h"
#include "VisualLogger/VisualLogger.h"
#include "AIResources.h"
#include "GameplayTasksComponent.h"
#include "CharacterBase.h"
#include "GameplayTagsSubSystem.h"
#include "InteractiveBaseGAComponent.h"

UAITask_SwitchWalkState::UAITask_SwitchWalkState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsPausable = true;

	bTickingTask = true;
}

void UAITask_SwitchWalkState::Activate()
{
	Super::Activate();

	ConditionalPerformTask();
}

void UAITask_SwitchWalkState::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
}

void UAITask_SwitchWalkState::ConditionalPerformTask()
{
	PerformTask();
}

bool UAITask_SwitchWalkState::WasMoveSuccessful() const
{
	return true;
}

void UAITask_SwitchWalkState::SetUp(
	AAIController* InController,
	bool InbIsSwitchToRun,
	bool InbIscontinueCheck
)
{
	ControllerPtr = InController;
	bIsSwitchToRun = InbIsSwitchToRun;
	bIscontinueCheck = InbIscontinueCheck;
}

void UAITask_SwitchWalkState::PerformTask()
{
	if (SwitchWalkState())
	{

	}
	else
	{
	}

	if (bIscontinueCheck)
	{
	}
	else
	{
		EndTask();
	}
}

bool UAITask_SwitchWalkState::SwitchWalkState()
{
	auto OnwerActorPtr = Cast<ACharacterBase>(ControllerPtr->GetCharacter());

	if (OnwerActorPtr)
	{
		return OnwerActorPtr->GetInteractiveBaseGAComponent()->SwitchWalkState(bIsSwitchToRun);
	}

	return false;
}

void UAITask_SwitchWalkState::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);
}
