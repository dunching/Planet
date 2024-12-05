
#include "GroupSharedInfo.h"

#include "GroupMnaggerComponent.h"

#include "HoldingItemsComponent.h"
#include "ProxySycHelperComponent.h"

AGroupSharedInfo::AGroupSharedInfo(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	bReplicates = true;
	
	TeamMatesHelperComponentPtr = CreateDefaultSubobject<UTeamMatesHelperComponent>(UTeamMatesHelperComponent::ComponentName);
	HoldingItemsComponentPtr = CreateDefaultSubobject<UHoldingItemsComponent>(UHoldingItemsComponent::ComponentName);
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

UHoldingItemsComponent* AGroupSharedInfo::GetHoldingItemsComponent()
{
	return HoldingItemsComponentPtr;
}
