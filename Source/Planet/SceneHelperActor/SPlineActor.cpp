#include "SPlineActor.h"

#include "CharacterAbilitySystemComponent.h"
#include "CollisionDataStruct.h"
#include "HumanCharacter.h"
#include "Skill_Active_Traction.h"
#include "Camera/CameraComponent.h"
#include "Components/SplineComponent.h"
#include "Engine/OverlapResult.h"
#include "Net/UnrealNetwork.h"

ASPlineActor::ASPlineActor(
	const FObjectInitializer& ObjectInitializer
) :
  Super(ObjectInitializer)
{
	SplineComponentPtr = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	RootComponent = SplineComponentPtr;

	bReplicates = true;
	SetReplicatingMovement(true);
}

void ASPlineActor::GetActorEyesViewPoint(
	FVector& Location,
	FRotator& Rotation
	) const
{
	TInlineComponentArray<UCameraComponent*> Cameras;
	GetComponents(/*out*/ Cameras);

	for (UCameraComponent* CameraComponent : Cameras)
	{
		if (CameraComponent->IsActive())
		{
			Location = CameraComponent->GetComponentLocation();
			Rotation = CameraComponent->GetComponentRotation();
			return;
		}
	}
	
	Super::GetActorEyesViewPoint(Location, Rotation);
}
