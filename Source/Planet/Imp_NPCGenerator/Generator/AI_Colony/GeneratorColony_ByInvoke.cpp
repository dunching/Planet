
#include "GeneratorColony_ByInvoke.h"

#include "Components/SceneComponent.h"
#include "Components/SplineComponent.h"

#include "PlanetChildActorComponent.h"
#include "BuildingArea.h"
#include "HumanCharacter.h"
#include "HumanAIController.h"
#include "AIComponent.h"
#include "GroupManagger_NPC.h"
#include "HumanCharacter_AI.h"

AGeneratorColony_ByInvoke::AGeneratorColony_ByInvoke(
	const FObjectInitializer& ObjectInitializer
	):
		Super(ObjectInitializer)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

	// bReplicates = true;
	// SetReplicatingMovement(true);

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.f / 60;
}

void AGeneratorColony_ByInvoke::BeginPlay()
{
	Super::BeginPlay();
}

void AGeneratorColony_ByInvoke::SpawnGeneratorActor()
{
	Super::SpawnGeneratorActor();
}
