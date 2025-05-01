#include "GroupManagger.h"

#include "HumanCharacter.h"
#include "TeamMatesHelperComponent.h"

#include "InventoryComponent.h"
#include "PlanetPlayerController.h"
#include "UGSAbilitySystemComponent.h"

AGroupManagger::AGroupManagger(
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

	TeamMatesHelperComponentPtr = CreateDefaultSubobject<UTeamMatesHelperComponent>(
		UTeamMatesHelperComponent::ComponentName
	);
	InventoryComponentPtr = CreateDefaultSubobject<UInventoryComponent>(UInventoryComponent::ComponentName);

	AbilitySystemComponentPtr = CreateDefaultSubobject<UGSAbilitySystemComponent>(
		UGSAbilitySystemComponent::ComponentName
	);
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

UInventoryComponent* AGroupManagger::GetInventoryComponent()
{
	return InventoryComponentPtr;
}

void AGroupManagger::SetOwnerCharacterProxyPtr(
	ACharacterBase* CharacterPtr
)
{
	if (CharacterPtr != nullptr)
	{
		TeamMatesHelperComponentPtr->SetOwnerCharacterProxy(CharacterPtr->GetCharacterProxy());
	}
}
