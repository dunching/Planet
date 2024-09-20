
#include "SPlineActor.h"

#include "Components/SplineComponent.h"

ASPlineActor::ASPlineActor(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	SplineComponentPtr = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	RootComponent = SplineComponentPtr;

//	SetReplicatingMovement(true);
}
