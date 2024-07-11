
#include "Consumable_Test.h"

#include <Components/StaticMeshComponent.h>

#include "CharacterBase.h"

AConsumable_Test::AConsumable_Test(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	StaticComponentPtr = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticComponentPtr->SetupAttachment(RootComponent);
}

void AConsumable_Test::Interaction(ACharacterBase* CharacterPtr)
{
	Super::Interaction(CharacterPtr);

	AttachToComponent(CharacterPtr->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Socket);
}

