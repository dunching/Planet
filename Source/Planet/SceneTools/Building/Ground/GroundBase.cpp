
#include "GroundBase.h"

#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SceneComponent.h"

#include "CollisionDataStruct.h"
#include "InputProcessor.h"
#include "HumanCharacter.h"
#include "PlacingProcessor.h"

AGroundBase::AGroundBase(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	MeshPtr = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshPtr->SetGenerateOverlapEvents(true);
	MeshPtr->SetCanEverAffectNavigation(false);
	MeshPtr->SetMobility(EComponentMobility::Movable);
	MeshPtr->SetupAttachment(RootComponent);
}

void AGroundBase::BeginPlay()
{
	Super::BeginPlay();
}

UStaticMeshComponent* AGroundBase::GetStaticMeshComponent() const
{
	return MeshPtr;
}
