
#include "GroupSharedInfo.h"

#include "TeamMatesHelperComponent.h"

#include "InventoryComponent.h"
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
	InventoryComponentPtr = CreateDefaultSubobject<UInventoryComponent>(UInventoryComponent::ComponentName);
	
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
		//
		GetTeamMatesHelperComponent()->SwitchTeammateOption(ETeammateOption::kInitialize);
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

UInventoryComponent* AGroupSharedInfo::GetHoldingItemsComponent()
{
	return InventoryComponentPtr;
}
