#include "ChallengeEntry.h"

#include "Async/TaskGraphInterfaces.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/WidgetComponent.h"
#include "Engine/Texture2D.h"
#include "Components/BoxComponent.h"

#include "HumanCharacter_Player.h"
#include "QuestInteraction.h"
#include "QuestInteractionBase.h"

AChallengeEntry::AChallengeEntry(const FObjectInitializer& ObjectInitializer):
                                                                             Super(
	                                                                              ObjectInitializer.
	                                                                              SetDefaultSubobjectClass<USceneChallengeEntryInteractionComponent>(
	                                                                               USceneChallengeEntryInteractionComponent::ComponentName)
	                                                                             )
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
	
	BoxComponentPtr = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponentPtr->SetupAttachment(RootComponent);

	SceneActorInteractionComponentPtr = CreateDefaultSubobject<USceneActorInteractionComponent>(
	USceneActorInteractionComponent::ComponentName);
	
	InteractionWidgetCompoentPtr = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	InteractionWidgetCompoentPtr->SetupAttachment(RootComponent);
}

void AChallengeEntry::BeginPlay()
{
	Super::BeginPlay();

	HasBeenEndedLookAt();
}

USceneActorInteractionComponent* AChallengeEntry::GetSceneActorInteractionComponent() const
{
	return SceneActorInteractionComponentPtr;
}

void AChallengeEntry::HasbeenInteracted(ACharacterBase* InCharacterPtr)
{
}

void AChallengeEntry::HasBeenStartedLookAt(ACharacterBase* InCharacterPtr)
{
	HasBeenLookingAt(InCharacterPtr);
}

void AChallengeEntry::HasBeenLookingAt(ACharacterBase* InCharacterPtr)
{
	if (
		InteractionWidgetCompoentPtr &&
		(FVector::Distance(InCharacterPtr->GetActorLocation(), GetActorLocation()) < SceneActorInteractionComponentPtr->Range)
		)
	{
		InteractionWidgetCompoentPtr->SetVisibility(true);
	}
	else
	{
		HasBeenEndedLookAt();
	}
}

void AChallengeEntry::HasBeenEndedLookAt()
{
	if (InteractionWidgetCompoentPtr)
	{
		InteractionWidgetCompoentPtr->SetVisibility(false);
	}
}

void USceneChallengeEntryInteractionComponent::StartInteractionItem(
	const TSubclassOf<AQuestInteractionBase>& Item
)
{
	Super::StartInteractionItem(Item);
	
	auto OnwerActorPtr = GetOwner<FOwnerType>();
	if (!OnwerActorPtr)
	{
		return;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.CustomPreSpawnInitalization = [OnwerActorPtr, this](AActor*ActorPtr)
	{
		auto GuideInteractionActorPtr= Cast<AGuideInteraction_ChallengeEntry>(ActorPtr);
		if (GuideInteractionActorPtr)
		{
			GuideInteractionActorPtr->ChallengeEntryPtr = OnwerActorPtr;
		}
	};

	auto GuideInteraction_ActorPtr = GetWorld()->SpawnActor<AGuideInteraction_ChallengeEntry>(
		Item, SpawnParameters
		);

	StartInteractionImp(Item, GuideInteraction_ActorPtr);
}
