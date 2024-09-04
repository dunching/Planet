
#include "ResourceBoxBase.h"

#include <Components/SceneComponent.h>

#include "KismetGravityLibrary.h"

#include "HoldingItemsComponent.h"
#include "CharacterBase.h"
#include "PlanetPlayerState.h"

AResourceBoxBase::AResourceBoxBase(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
}

void AResourceBoxBase::BeginPlay()
{
	Super::BeginPlay();

	FVector Start = RootComponent->GetComponentLocation();
	FVector End = Start + (UKismetGravityLibrary::GetGravity(RootComponent->GetComponentLocation()) * 1000.f);

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	FHitResult OutHit;
	if (GetWorld()->LineTraceSingleByObjectType(OutHit, Start, End, ObjectQueryParams, Params))
	{
		if (FVector::Distance(OutHit.ImpactPoint, Start) > 10.f)
		{
			SetActorLocation(OutHit.ImpactPoint);
		}
	}
}

void AResourceBoxBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AResourceBoxBase::Interaction(ACharacterBase* InCharacterPtr)
{
	Super::Interaction(InCharacterPtr);

	TargetCharacterPtr = InCharacterPtr;
}

void AResourceBoxBase::AddItemsToTarget()
{
	if (TargetCharacterPtr)
	{
		{
			for (const auto Iter : UnitMap)
			{
				TargetCharacterPtr->GetHoldingItemsComponent()->AddUnit(Iter.Key, Iter.Value);
			}
#if WITH_EDITORONLY_DATA
#endif
		}
	}
}

