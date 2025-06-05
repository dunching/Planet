
#include "Consumable_Generic.h"

#include <Components/StaticMeshComponent.h>

#include "CharacterBase.h"

AConsumable_Generic::AConsumable_Generic(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	StaticComponentPtr = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticComponentPtr->SetupAttachment(RootComponent);
}

void AConsumable_Generic::HasbeenInteracted(
	ACharacterBase* CharacterPtr
	)
{
	Super::HasbeenInteracted(CharacterPtr);
	
	AttachToComponent(CharacterPtr->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Socket);
}

