#include "GroupManaggerBase.h"

#include "InventoryComponentBase.h"
#include "TeamMatesHelperComponentBase.h"
#include "UGSAbilitySystemComponent.h"


FName AGroupManaggerBase::ComponentName = TEXT("GroupManagger");

AGroupManaggerBase::AGroupManaggerBase(
	const FObjectInitializer& ObjectInitializer
):
 Super(ObjectInitializer)
{
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	bAlwaysRelevant = true;
	SetReplicatingMovement(false);
	SetNetUpdateFrequency(1.f);

	// Note: this is very important to set to false. Though all replication infos are spawned at run time, during seamless travel
	// they are held on to and brought over into the new world. In ULevel::InitializeActors, these PlayerStates may be treated as map/startup actors
	// and given static NetGUIDs. This also causes their deletions to be recorded and sent to new clients, which if unlucky due to name conflicts,
	// may end up deleting the new PlayerStates they had just spaned.
	bNetLoadOnClient = false;

	bIsSpatiallyLoaded = false;

	TeamMatesHelperComponentPtr = CreateDefaultSubobject<UTeamMatesHelperComponentBase>(
		UTeamMatesHelperComponentBase::ComponentName
	);
	InventoryComponentPtr = CreateDefaultSubobject<UInventoryComponentBase>(UInventoryComponentBase::ComponentName);

	AbilitySystemComponentPtr = CreateDefaultSubobject<UGSAbilitySystemComponent>(
		UGSAbilitySystemComponent::ComponentName
	);
	AbilitySystemComponentPtr->SetIsReplicated(true);
	AbilitySystemComponentPtr->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
}

void AGroupManaggerBase::BeginPlay()
{
	Super::BeginPlay();
}

void AGroupManaggerBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UWorld* World = GetWorld();
	if ((World->IsGameWorld()))
	{
		//
		GetTeamMatesHelperComponentBase()->SwitchTeammateOption(ETeammateOption::kInitialize);
	}
}

UPlanetAbilitySystemComponent* AGroupManaggerBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponentPtr;
}

UTeamMatesHelperComponentBase* AGroupManaggerBase::GetTeamMatesHelperComponentBase()
{
	return TeamMatesHelperComponentPtr;
}

UInventoryComponentBase* AGroupManaggerBase::GetInventoryComponentBase()
{
	return InventoryComponentPtr;
}
