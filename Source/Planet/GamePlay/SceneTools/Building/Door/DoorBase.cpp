
#include "DoorBase.h"

#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SceneComponent.h"

#include "CollisionDataStruct.h"

ADoorBase::ADoorBase(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	MeshPtr = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshPtr->SetGenerateOverlapEvents(true);
	MeshPtr->SetCanEverAffectNavigation(false);
	MeshPtr->SetMobility(EComponentMobility::Movable);
	MeshPtr->SetupAttachment(RootComponent);
}

void ADoorBase::BeginPlay()
{
	Super::BeginPlay();
}

UStaticMeshComponent* ADoorBase::GetStaticMeshComponent() const
{
	return MeshPtr;
}
