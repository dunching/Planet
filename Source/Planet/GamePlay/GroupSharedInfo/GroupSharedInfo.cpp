
#include "GroupSharedInfo.h"

#include "TeamMatesHelperComponent.h"

#include "HoldingItemsComponent.h"
#include "UGSAbilitySystemComponent.h"

AGroupSharedInfo::AGroupSharedInfo(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	bAlwaysRelevant = true;
	SetReplicatingMovement(false);
	NetUpdateFrequency = 1;

	TeamMatesHelperComponentPtr = CreateDefaultSubobject<UTeamMatesHelperComponent>(UTeamMatesHelperComponent::ComponentName);
	HoldingItemsComponentPtr = CreateDefaultSubobject<UHoldingItemsComponent>(UHoldingItemsComponent::ComponentName);
	
	AbilitySystemComponentPtr = CreateDefaultSubobject<UGSAbilitySystemComponent>(UGSAbilitySystemComponent::ComponentName);
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
