
#include "BuildingArea.h"

#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include <Components/SphereComponent.h>

#include "CollisionDataStruct.h"

ABuildingArea::ABuildingArea(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	AreaPtr = CreateDefaultSubobject<USphereComponent>(TEXT("AreaComponent"));
	AreaPtr->SetupAttachment(RootComponent);

	MeshPtr = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshPtr->SetGenerateOverlapEvents(true);
	MeshPtr->SetCanEverAffectNavigation(false);
	MeshPtr->SetMobility(EComponentMobility::Movable);
	MeshPtr->SetupAttachment(RootComponent);
}

void ABuildingArea::BeginPlay()
{
	Super::BeginPlay();
}