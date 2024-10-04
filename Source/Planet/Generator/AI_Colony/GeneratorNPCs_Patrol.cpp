
#include "GeneratorNPCs_Patrol.h"

#include "Components/ChildActorComponent.h"
#include "Components/SplineComponent.h"

#include "Component/PlanetChildActorComponent.h"
#include "HumanCharacter.h"
#include "HumanAIController.h"

AGeneratorNPCs_Patrol::AGeneratorNPCs_Patrol(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("USceneComponent"));

	SplineComponentPtr = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	SplineComponentPtr->SetupAttachment(RootComponent);
}

void AGeneratorNPCs_Patrol::BeginPlay()
{
	Super::BeginPlay();
}

