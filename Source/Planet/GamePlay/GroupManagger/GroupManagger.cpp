
#include "GroupManagger.h"

#include "TeamMatesHelperComponent.h"

#include "InventoryComponent.h"
#include "UGSAbilitySystemComponent.h"

AGroupManagger::AGroupManagger(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	bAlwaysRelevant = true;
	SetReplicatingMovement(false);
	SetNetUpdateFrequency(1.f);

	TeamMatesHelperComponentPtr = CreateDefaultSubobject<UTeamMatesHelperComponent>(UTeamMatesHelperComponent::ComponentName);
	InventoryComponentPtr = CreateDefaultSubobject<UInventoryComponent>(UInventoryComponent::ComponentName);
	
	AbilitySystemComponentPtr = CreateDefaultSubobject<UGSAbilitySystemComponent>(UGSAbilitySystemComponent::ComponentName);
	AbilitySystemComponentPtr->SetIsReplicated(true);
	AbilitySystemComponentPtr->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
}

void AGroupManagger::BeginPlay()
{
	Super::BeginPlay();
}

void AGroupManagger::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UWorld* World = GetWorld();
	if ((World->IsGameWorld()))
	{
		//
		GetTeamMatesHelperComponent()->SwitchTeammateOption(ETeammateOption::kInitialize);
	}
}

UPlanetAbilitySystemComponent* AGroupManagger::GetAbilitySystemComponent() const
{
	return AbilitySystemComponentPtr;
}

UTeamMatesHelperComponent* AGroupManagger::GetTeamMatesHelperComponent()
{
	return TeamMatesHelperComponentPtr;
}

UInventoryComponent* AGroupManagger::GetHoldingItemsComponent()
{
	return InventoryComponentPtr;
}
