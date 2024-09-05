
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

	PrimaryActorTick.bCanEverTick = true;
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

void AResourceBox::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AResourceBox::Interaction(ACharacterBase* InCharacterPtr)
{
	Super::Interaction(InCharacterPtr);

	Interaction2Server();
}

void AResourceBox::Interaction2Server_Implementation()
{
	Interaction2Client();
}

void AResourceBox::Interaction2Client_Implementation()
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
