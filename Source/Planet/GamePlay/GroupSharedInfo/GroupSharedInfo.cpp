
#include "GroupSharedInfo.h"

#include "TeamMatesHelperComponent.h"

#include "HoldingItemsComponent.h"

AGroupSharedInfo::AGroupSharedInfo(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	bReplicates = true;
	
	TeamMatesHelperComponentPtr = CreateDefaultSubobject<UTeamMatesHelperComponent>(UTeamMatesHelperComponent::ComponentName);
	HoldingItemsComponentPtr = CreateDefaultSubobject<UHoldingItemsComponent>(UHoldingItemsComponent::ComponentName);
	
	AbilitySystemComponentPtr = CreateDefaultSubobject<UPlanetAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponentPtr->SetIsReplicated(true);
	AbilitySystemComponentPtr->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
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

UPlanetAbilitySystemComponent* AGroupSharedInfo::GetAbilitySystemComponent() const
{
	return AbilitySystemComponentPtr;
}

UTeamMatesHelperComponent* AGroupSharedInfo::GetTeamMatesHelperComponent()
{
	return TeamMatesHelperComponentPtr;
}

UHoldingItemsComponent* AGroupSharedInfo::GetHoldingItemsComponent()
{
	return HoldingItemsComponentPtr;
}
