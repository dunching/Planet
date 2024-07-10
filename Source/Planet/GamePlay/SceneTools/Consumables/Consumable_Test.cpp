
#include "Consumable_Test.h"

#include <Components/StaticMeshComponent.h>

AConsumable_Test::AConsumable_Test(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));

	StaticComponentPtr = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticComponentPtr->SetupAttachment(RootComponent);
}

