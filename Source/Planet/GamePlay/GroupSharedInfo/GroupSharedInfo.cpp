
#include "GroupSharedInfo.h"

#include "GroupMnaggerComponent.h"

#include "HoldingItemsComponent.h"
#include "TeamConfigureomponent.h"

AGroupSharedInfo::AGroupSharedInfo(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	bReplicates = true;
	
	TeamMatesHelperComponentPtr = CreateDefaultSubobject<UTeamMatesHelperComponent>(UTeamMatesHelperComponent::ComponentName);
	HoldingItemsComponentPtr = CreateDefaultSubobject<UHoldingItemsComponent>(UHoldingItemsComponent::ComponentName);
	TeamConfigureompConentPtr = CreateDefaultSubobject<UTeamConfigureComponent>(UTeamConfigureComponent::ComponentName);
}

void AGroupSharedInfo::BeginPlay()
{
	Super::BeginPlay();
}

void AGroupSharedInfo::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UWorld* World = GetWorld();
	if ((World->IsGameWorld()))
	{
	}
}

UTeamMatesHelperComponent* AGroupSharedInfo::GetTeamMatesHelperComponent()
{
	return TeamMatesHelperComponentPtr;
}

UTeamConfigureComponent* AGroupSharedInfo::GetTeamConfigureomponent() const
{
	return TeamConfigureompConentPtr;
}
