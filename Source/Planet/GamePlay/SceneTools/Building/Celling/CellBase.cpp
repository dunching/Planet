
#include "CellBase.h"

#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SceneComponent.h"

#include "CollisionDataStruct.h"
#include "InputProcessor.h"
#include "HumanCharacter.h"
#include "PlacingProcessor.h"

ACellBase::ACellBase(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	MeshPtr = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshPtr->SetGenerateOverlapEvents(true);
	MeshPtr->SetCanEverAffectNavigation(false);
	MeshPtr->SetMobility(EComponentMobility::Movable);
	MeshPtr->SetupAttachment(RootComponent);
}

void ACellBase::BeginPlay()
{
	Super::BeginPlay();
}

UStaticMeshComponent* ACellBase::GetStaticMeshComponent() const
{
	return MeshPtr;
}
