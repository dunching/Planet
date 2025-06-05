
#include "ResourceBoxBase.h"

#include <Components/SceneComponent.h>

#include "KismetGravityLibrary.h"

#include "InventoryComponent.h"
#include "CharacterBase.h"
#include "CollisionDataStruct.h"
#include "PlanetPlayerState.h"

AResourceBoxBase::AResourceBoxBase(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
}

void AResourceBoxBase::BeginPlay()
{
	Super::BeginPlay();

	HasBeenEndedLookAt();

	FVector Start = RootComponent->GetComponentLocation();
	FVector End = Start + (UKismetGravityLibrary::GetGravity(RootComponent->GetComponentLocation()) * 1000.f);

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(LandScape_Object);

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

USceneActorInteractionComponent* AResourceBoxBase::GetSceneActorInteractionComponent() const
{
	return nullptr;
}

void AResourceBoxBase::HasbeenInteracted(ACharacterBase* InCharacterPtr)
{
	InteractionImp_BoxBase(InCharacterPtr);
}

void AResourceBoxBase::HasBeenStartedLookAt(
	ACharacterBase* CharacterPtr
	)
{
}

void AResourceBoxBase::HasBeenLookingAt(
	ACharacterBase* CharacterPtr
	)
{
}

void AResourceBoxBase::HasBeenEndedLookAt()
{
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
				auto HICPtr = TargetCharacterPtr->GetInventoryComponent();
				for (const auto Iter : ProxyMap)
				{
					HICPtr->AddProxy_Pending(Iter.Key, Iter.Value, Guid);
				}

				HICPtr->SyncPendingProxy(Guid);
#if WITH_EDITORONLY_DATA
#endif
			}
		}
	}
#endif
}

