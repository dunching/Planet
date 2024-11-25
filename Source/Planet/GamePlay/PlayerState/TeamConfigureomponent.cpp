
#include "TeamConfigureomponent.h"

#include "Net/UnrealNetwork.h"

#include "PlanetPlayerState.h"
#include "CharacterBase.h"

FName UTeamConfigureComponent::ComponentName = TEXT("TeamConfigureomponent");

UTeamConfigureComponent::UTeamConfigureComponent(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	bWantsInitializeComponent = true;

	SetIsReplicatedByDefault(true);
}

void UTeamConfigureComponent::UpdateConfigure(const FTeammate_FASI& Teammate_FASI)
{
	Teammate_FASI_Container.UpdateItem(Teammate_FASI);
}

void UTeamConfigureComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

//	DOREPLIFETIME(ThisClass, Teammate_FASI_Container);
}

void UTeamConfigureComponent::InitializeComponent()
{
	Super::InitializeComponent();

	Teammate_FASI_Container.OwnerPtr = GetOwner<FOwnerType>();
}
