
#include "GeneratorColony_ByTime.h"

#include "Components/SceneComponent.h"
#include "Components/SplineComponent.h"

#include "PlanetChildActorComponent.h"
#include "BuildingArea.h"
#include "HumanCharacter.h"
#include "HumanAIController.h"
#include "AIComponent.h"

AGeneratorColony_ByTime::AGeneratorColony_ByTime(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

	// bReplicates = true;
	// SetReplicatingMovement(true);

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.f / 60;
}

void AGeneratorColony_ByTime::BeginPlay()
{
	Super::BeginPlay();
	
#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		SpawnGeneratorActor();
	}
#endif
}
