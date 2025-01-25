
#include "WallWithDoorBase.h"

#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SceneComponent.h"

#include "CollisionDataStruct.h"

AWallWithDoorBase::AWallWithDoorBase(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	MeshPtr = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshPtr->SetGenerateOverlapEvents(true);
	MeshPtr->SetCanEverAffectNavigation(false);
	MeshPtr->SetMobility(EComponentMobility::Movable);
	MeshPtr->SetupAttachment(RootComponent);
}

void AWallWithDoorBase::BeginPlay()
{
	Super::BeginPlay();
}

UStaticMeshComponent* AWallWithDoorBase::GetStaticMeshComponent() const
{
	return MeshPtr;
}
