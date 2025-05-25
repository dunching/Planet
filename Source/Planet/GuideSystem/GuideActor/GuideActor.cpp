#include "GuideActor.h"

#include "GameplayTasksComponent.h"
#include "GroupManagger.h"

#include "GuideSystemStateTreeComponent.h"
#include "PlanetPlayerController.h"


FTaskNodeDescript::FTaskNodeDescript()
{
}

FTaskNodeDescript::FTaskNodeDescript(
	bool bIsOnlyFresh
	):
	 bIsOnlyFresh(bIsOnlyFresh)
{
}

bool FTaskNodeDescript::GetIsValid() const
{
	return /* bIsFreshPreviouDescription && */ (!Description.IsEmpty());
}

FTaskNodeDescript FTaskNodeDescript::Refresh = FTaskNodeDescript(true);

AGuideActor::AGuideActor(
	const FObjectInitializer& ObjectInitializer
	):
	 Super(ObjectInitializer)
{
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	bAlwaysRelevant = true;
	SetReplicatingMovement(false);
	SetMinNetUpdateFrequency(1.f);

	GuideStateTreeComponentPtr = CreateDefaultSubobject<UGuideSystemStateTreeComponent>(
		 UGuideSystemStateTreeComponent::ComponentName
		);
	GuideStateTreeComponentPtr->SetStartLogicAutomatically(false);

	GameplayTasksComponentPtr = CreateDefaultSubobject<UGameplayTasksComponent>(TEXT("GameplayTasksComponent"));

	GuideID = FGuid::NewGuid();
}

void AGuideActor::Destroyed()
{
	auto PCPtr =  Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
	if (PCPtr)
	{
		auto GroupManaggerGASPtr =PCPtr->
								   GetGroupManagger()->GetAbilitySystemComponent();
		if (GroupManaggerGASPtr)
		{
			GroupManaggerGASPtr->RemoveLooseGameplayTags(
					 ActivedTags
					);
		}
	}
	
	Super::Destroyed();
}

void AGuideActor::ActiveGuide()
{
	GetGuideSystemStateTreeComponent()->StartLogic();

	auto PCPtr =  Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
	if (PCPtr)
	{
		auto GroupManaggerGASPtr =PCPtr->
								   GetGroupManagger()->GetAbilitySystemComponent();
		if (GroupManaggerGASPtr)
		{
			GroupManaggerGASPtr->AddLooseGameplayTags(
					 ActivedTags
					);
		}
	}
}

UGameplayTasksComponent* AGuideActor::GetGameplayTasksComponent() const
{
	return GameplayTasksComponentPtr;
}

UGuideSystemStateTreeComponent* AGuideActor::GetGuideSystemStateTreeComponent() const
{
	return GuideStateTreeComponentPtr;
}

FGuid AGuideActor::GetGuideID() const
{
	return GuideID;
}

void AGuideActor::SetCurrentTaskID(
	const FGuid& TaskID
	)
{
	CurrentTaskID = TaskID;
}

FGuid AGuideActor::GetPreviousTaskID() const
{
	return PreviousTaskID;
}

void AGuideActor::SetPreviousTaskID(
	const FGuid& PreviousGuideID_
	)
{
	PreviousTaskID = PreviousGuideID_;
}

FGuid AGuideActor::GetCurrentTaskID() const
{
	return CurrentTaskID;
}
