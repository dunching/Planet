
#include "GeneratorColony_ByTime.h"

#include "Components/SceneComponent.h"
#include "Components/SplineComponent.h"

#include "PlanetChildActorComponent.h"
#include "BuildingArea.h"
#include "HumanCharacter.h"
#include "HumanAIController.h"
#include "AIComponent.h"

AGeneratorColony::AGeneratorColony(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

	// bReplicates = true;
	// SetReplicatingMovement(true);

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.f / 60;
}

void AGeneratorColony::BeginPlay()
{
	Super::BeginPlay();
	
	SpawnGeneratorActor();
}
