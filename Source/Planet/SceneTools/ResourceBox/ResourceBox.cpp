
#include "ResourceBox.h"

#include "ActorSequenceComponent.h"
#include "ActorSequence.h"
#include <Components/BoxComponent.h>
#include <Components/WidgetComponent.h>
#include "ActorSequencePlayer.h"
#include "CharacterBase.h"

AResourceBox::AResourceBox(const FObjectInitializer& ObjectInitializer) :
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
	LookingAt(InCharacterPtr);
}

void AResourceBox::LookingAt(ACharacterBase* InCharacterPtr)
{
	if (
		InteractionWidgetCompoentPtr &&
		!bIsOpend &&
		(FVector::Distance(InCharacterPtr->GetActorLocation(), GetActorLocation()) < Range)
		)
	{
		InteractionWidgetCompoentPtr->SetVisibility(true);
	}
	else
	{
		EndLookAt();
	}
}

void AResourceBox::BeginPlay()
{
	Super::BeginPlay();

#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		if (ActorSequenceComponent && ActorSequenceComponent->GetSequence())
		{
			ActorSequenceComponent->GetSequencePlayer()->OnFinished.AddDynamic(this, &ThisClass::OnAnimationFinished);
		}
	}
#endif

	EndLookAt();
}

void AResourceBox::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AResourceBox::Interaction(ACharacterBase* InCharacterPtr)
{
	if (bIsOpend)
	{
	}
	else
	{
		InteractionImp();
	}

	Super::Interaction(InCharacterPtr);
}

void AResourceBox::InteractionImp_Implementation()
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
