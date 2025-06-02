#include "ArticleBase.h"

#include "Components/SceneComponent.h"
#include <Kismet/GameplayStatics.h>

#include "CollisionDataStruct.h"
#include "GameInstance/PlanetGameInstance.h"

ARawMaterialBase::ARawMaterialBase(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	SceneCompPtr = CreateDefaultSubobject<USceneComponent>(ARawMaterialBase::SceneCompName);
	SceneCompPtr->SetMobility(EComponentMobility::Static);
	RootComponent = SceneCompPtr;

	StaticMeshCompPtr = CreateDefaultSubobject<UStaticMeshComponent>(ARawMaterialBase::StaticMeshCompName);
	StaticMeshCompPtr->SetMobility(EComponentMobility::Stationary);
	StaticMeshCompPtr->SetupAttachment(SceneCompPtr);
}

void ARawMaterialBase::OnConstruction(
	const FTransform& Transform
	)
{
	Super::OnConstruction(Transform);
}

void ARawMaterialBase::BeginPlay()
{
	Super::BeginPlay();
}

USceneActorInteractionComponent* ARawMaterialBase::GetSceneActorInteractionComponent() const
{
	return nullptr;
}

void ARawMaterialBase::HasbeenInteracted(
	ACharacterBase* CharacterPtr
	)
{
}

void ARawMaterialBase::HasBeenStartedLookAt(
	ACharacterBase* CharacterPtr
	)
{
}

void ARawMaterialBase::HasBeenLookingAt(
	ACharacterBase* CharacterPtr
	)
{
}

void ARawMaterialBase::HasBeenEndedLookAt()
{
}

FName ARawMaterialBase::SceneCompName = TEXT("SceneComp");

FName ARawMaterialBase::StaticMeshCompName = TEXT("StaticMeshComp");

URawMaterialInteractionComponent::URawMaterialInteractionComponent(
	const FObjectInitializer& ObjectInitializer
	):
	 Super(ObjectInitializer)
{
}

EPickType URawMaterialInteractionComponent::GetPickType() const
{
	return PickType;
}
