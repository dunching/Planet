#include "QuestsActorBase.h"

#include "GameplayTasksComponent.h"

#include "QuestSystemStateTreeComponent.h"

AQuestActorBase::AQuestActorBase(
	const FObjectInitializer& ObjectInitializer
	):
	 Super(ObjectInitializer)
{
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	bAlwaysRelevant = true;
	SetReplicatingMovement(false);
	SetMinNetUpdateFrequency(1.f);

	GuideStateTreeComponentPtr = CreateDefaultSubobject<UQuestSystemStateTreeComponent>(
		 UQuestSystemStateTreeComponent::ComponentName
		);
	GuideStateTreeComponentPtr->SetStartLogicAutomatically(false);

	GameplayTasksComponentPtr = CreateDefaultSubobject<UGameplayTasksComponent>(TEXT("GameplayTasksComponent"));

	GuideID = FGuid::NewGuid();
}

void AQuestActorBase::Destroyed()
{
	Super::Destroyed();
}

void AQuestActorBase::ActiveGuide()
{
	GetGuideSystemStateTreeComponent()->StartLogic();
}

UGameplayTasksComponent* AQuestActorBase::GetGameplayTasksComponent() const
{
	return GameplayTasksComponentPtr;
}

UQuestSystemStateTreeComponent* AQuestActorBase::GetGuideSystemStateTreeComponent() const
{
	return GuideStateTreeComponentPtr;
}

FGuid AQuestActorBase::GetGuideID() const
{
	return GuideID;
}

void AQuestActorBase::SetCurrentTaskID(
	const FGuid& TaskID
	)
{
	CurrentTaskID = TaskID;
}

FGuid AQuestActorBase::GetPreviousTaskID() const
{
	return PreviousTaskID;
}

void AQuestActorBase::SetPreviousTaskID(
	const FGuid& PreviousGuideID_
	)
{
	PreviousTaskID = PreviousGuideID_;
}

FGuid AQuestActorBase::GetCurrentTaskID() const
{
	return CurrentTaskID;
}
