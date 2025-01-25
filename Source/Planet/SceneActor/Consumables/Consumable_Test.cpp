
#include "Consumable_Test.h"

#include <Components/StaticMeshComponent.h>

#include "CharacterBase.h"

AConsumable_Test::AConsumable_Test(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	StaticComponentPtr = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticComponentPtr->SetupAttachment(RootComponent);

	SetReplicates(true);
}

void AConsumable_Test::HasbeenInteracted(ACharacterBase* CharacterPtr)
{
	Super::HasbeenInteracted(CharacterPtr);

	AttachToComponent(CharacterPtr->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Socket);
}

