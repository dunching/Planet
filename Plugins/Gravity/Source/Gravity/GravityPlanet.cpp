
#include "GravityPlanet.h"

#include "Components/SphereComponent.h"

#include "Character/GravityCharacter.h"

AGravityPlanet::AGravityPlanet(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	SphereComponentPtr = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponentPtr->SetupAttachment(RootComponent);
}
