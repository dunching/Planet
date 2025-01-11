
#include "RoofBase.h"

#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SceneComponent.h"

#include "CollisionDataStruct.h"

ARoofBase::ARoofBase(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	MeshPtr = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshPtr->SetGenerateOverlapEvents(true);
	MeshPtr->SetCanEverAffectNavigation(false);
	MeshPtr->SetMobility(EComponentMobility::Movable);
	MeshPtr->SetupAttachment(RootComponent);
}

void ARoofBase::BeginPlay()
{
	Super::BeginPlay();
}

UStaticMeshComponent* ARoofBase::GetStaticMeshComponent() const
{
	return MeshPtr;
}
