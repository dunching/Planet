#include "GuideActorBase.h"

#include "GameplayTasksComponent.h"

#include "GuideSystemStateTreeComponent.h"

AGuideActorBase::AGuideActorBase(
	const FObjectInitializer& ObjectInitializer
	):
	 Super(ObjectInitializer)
{
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	bAlwaysRelevant = true;
	SetReplicatingMovement(false);
	SetMinNetUpdateFrequency(1.f);

	GuideStateTreeComponentPtr = CreateDefaultSubobject<UGuideSystemStateTreeComponent>(
		 UGuideSystemStateTreeComponent::ComponentName
		);
	GuideStateTreeComponentPtr->SetStartLogicAutomatically(false);

	GameplayTasksComponentPtr = CreateDefaultSubobject<UGameplayTasksComponent>(TEXT("GameplayTasksComponent"));

	GuideID = FGuid::NewGuid();
}

void AGuideActorBase::Destroyed()
{
	Super::Destroyed();
}

void AGuideActorBase::ActiveGuide()
{
	GetGuideSystemStateTreeComponent()->StartLogic();
}

UGameplayTasksComponent* AGuideActorBase::GetGameplayTasksComponent() const
{
	return GameplayTasksComponentPtr;
}

UGuideSystemStateTreeComponent* AGuideActorBase::GetGuideSystemStateTreeComponent() const
{
	return GuideStateTreeComponentPtr;
}

FGuid AGuideActorBase::GetGuideID() const
{
	return GuideID;
}

void AGuideActorBase::SetCurrentTaskID(
	const FGuid& TaskID
	)
{
	CurrentTaskID = TaskID;
}

FGuid AGuideActorBase::GetPreviousTaskID() const
{
	return PreviousTaskID;
}

void AGuideActorBase::SetPreviousTaskID(
	const FGuid& PreviousGuideID_
	)
{
	PreviousTaskID = PreviousGuideID_;
}

FGuid AGuideActorBase::GetCurrentTaskID() const
{
	return CurrentTaskID;
}
