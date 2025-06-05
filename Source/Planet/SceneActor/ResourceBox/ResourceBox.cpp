#include "ResourceBox.h"

#include "ActorSequenceComponent.h"
#include "ActorSequence.h"
#include <Components/BoxComponent.h>
#include <Components/WidgetComponent.h>
#include "ActorSequencePlayer.h"

#include "LogWriter.h"

#include "CharacterBase.h"
#include "GameOptions.h"
#include "Kismet/GameplayStatics.h"

AResourceBox::AResourceBox(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	BoxComponentPtr = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponentPtr->SetupAttachment(RootComponent);

	InteractionWidgetCompoentPtr = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	InteractionWidgetCompoentPtr->SetupAttachment(RootComponent);

	PrimaryActorTick.bCanEverTick = true;
}

void AResourceBox::OnConstruction(
	const FTransform& Transform
	)
{
	Super::OnConstruction(Transform);
}

void AResourceBox::HasBeenEndedLookAt()
{
	if (InteractionWidgetCompoentPtr)
	{
		InteractionWidgetCompoentPtr->SetVisibility(false);
	}

	Super::HasBeenEndedLookAt();
}

void AResourceBox::HasBeenLookingAt(
	ACharacterBase* InCharacterPtr
	)
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
		HasBeenEndedLookAt();
	}

	Super::HasBeenEndedLookAt();
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
}

void AResourceBox::Tick(
	float DeltaSeconds
	)
{
	Super::Tick(DeltaSeconds);
}

void AResourceBox::HasbeenInteracted(
	ACharacterBase* InCharacterPtr
	)
{
	if (
		!bIsOpend &&
		(FVector::Distance(InCharacterPtr->GetActorLocation(), GetActorLocation()) < Range)
	)
	{
		InteractionImp();

		Super::HasbeenInteracted(InCharacterPtr);
	}
	else
	{
		PRINTINVOKEWITHSTR(FString(TEXT("Box ss Opening")));
	}
}

void AResourceBox::HasBeenStartedLookAt(
	ACharacterBase* InCharacterPtr
	)
{
	Super::HasBeenStartedLookAt(InCharacterPtr);

	HasBeenLookingAt(InCharacterPtr);
}

void AResourceBox::InteractionImp_Implementation()
{
#if UE_EDITOR || UE_CLIENT
	if (GetNetMode() == NM_Client)
	{
		UGameplayStatics::PlaySoundAtLocation(
		                                      this,
		                                      OpenBoxSoundRef.LoadSynchronous(),
		                                      GetActorLocation()
		                                     );
	}
#endif

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
