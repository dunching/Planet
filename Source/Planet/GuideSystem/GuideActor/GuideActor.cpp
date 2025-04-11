#include "GuideActor.h"

#include "GameplayTasksComponent.h"

#include "GuideSystemStateTreeComponent.h"


FTaskNodeDescript::FTaskNodeDescript()
{
}

FTaskNodeDescript::FTaskNodeDescript(bool bIsOnlyFresh):
	bIsOnlyFresh(bIsOnlyFresh)
{
}

bool FTaskNodeDescript::GetIsValid() const
{
	return /* bIsFreshPreviouDescription && */ (!Description.IsEmpty());
}

FTaskNodeDescript FTaskNodeDescript::Refresh = FTaskNodeDescript(true);

AGuideActor::AGuideActor(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	bAlwaysRelevant = true;
	SetReplicatingMovement(false);
	SetMinNetUpdateFrequency(1.f);

	GuideStateTreeComponentPtr = CreateDefaultSubobject<UGuideSystemStateTreeComponent>(UGuideSystemStateTreeComponent::ComponentName);
	
	GameplayTasksComponentPtr = CreateDefaultSubobject<UGameplayTasksComponent>(TEXT("GameplayTasksComponent"));
}

void AGuideActor::Destroyed()
{
	// GetGuideSystemStateTreeComponent()->StopLogic(TEXT(""));
	
	Super::Destroyed();
}

UGameplayTasksComponent* AGuideActor::GetGameplayTasksComponent() const
{
	return GameplayTasksComponentPtr;
}

UGuideSystemStateTreeComponent* AGuideActor::GetGuideSystemStateTreeComponent() const
{
	return GuideStateTreeComponentPtr;
}
