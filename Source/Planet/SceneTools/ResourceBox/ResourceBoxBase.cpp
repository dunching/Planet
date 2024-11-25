
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

	InteractionImp_BoxBase(InCharacterPtr);
}

void AResourceBoxBase::InteractionImp_BoxBase_Implementation(ACharacterBase* InCharacterPtr)
{
	TargetCharacterPtr = InCharacterPtr;
	bIsOpend = true;
}

void AResourceBoxBase::AddItemsToTarget()
{
#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		if (TargetCharacterPtr)
		{
			{
				FGuid Guid = FGuid::NewGuid();
				auto HICPtr = TargetCharacterPtr->GetHoldingItemsComponent();
				for (const auto Iter : UnitMap)
				{
					HICPtr->AddUnit_Pending(Iter.Key, Iter.Value, Guid);
				}

				HICPtr->SyncPendingUnit(Guid);
#if WITH_EDITORONLY_DATA
#endif
			}
		}
	}
#endif
}

