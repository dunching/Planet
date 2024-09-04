
#include "ResourceBox.h"

#include "ActorSequenceComponent.h"
#include "ActorSequence.h"
#include <Components/BoxComponent.h>
#include <Components/WidgetComponent.h>
#include "ActorSequencePlayer.h"

AResourceBox::AResourceBox(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	BoxComponentPtr = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponentPtr->SetupAttachment(RootComponent);

	InteractionWidgetCompoentPtr = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	InteractionWidgetCompoentPtr->SetupAttachment(RootComponent);
}

void AResourceBox::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AResourceBox::EndLookAt()
{
	if (InteractionWidgetCompoentPtr)
	{
		InteractionWidgetCompoentPtr->SetVisibility(false);
	}
}

void AResourceBox::StartLookAt(ACharacterBase* InCharacterPtr)
{
	if (InteractionWidgetCompoentPtr)
	{
		InteractionWidgetCompoentPtr->SetVisibility(true);
	}
}

void AResourceBox::BeginPlay()
{
	Super::BeginPlay();

	if (ActorSequenceComponent && ActorSequenceComponent->GetSequence())
	{
		ActorSequenceComponent->GetSequencePlayer()->OnFinished.AddDynamic(this, &ThisClass::OnAnimationFinished);
	}

	EndLookAt();
}

void AResourceBox::Interaction_Implementation(ACharacterBase* InCharacterPtr)
{
	Super::Interaction(InCharacterPtr);

	Interaction_Imp(InCharacterPtr);
}

void AResourceBox::Interaction1_Implementation(ACharacterBase* InCharacterPtr)
{
	Super::Interaction(InCharacterPtr);

	Interaction_Imp(InCharacterPtr);
}

void AResourceBox::Interaction_Imp_Implementation(ACharacterBase* CharacterPtr)
{
	if (ActorSequenceComponent)
	{
		ActorSequenceComponent->PlaySequence();
	}
}

void AResourceBox::OnAnimationFinished()
{
#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		AddItemsToTarget();

		Destroy();
	}
#endif
}
