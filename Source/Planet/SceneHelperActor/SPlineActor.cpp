#include "SPlineActor.h"

#include "Components/SplineComponent.h"

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

ATractionPoint::ATractionPoint(
	const FObjectInitializer& ObjectInitializer
):
 Super(ObjectInitializer)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

	bReplicates = true;
	SetReplicatingMovement(true);
}
