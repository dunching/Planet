#include "GuideThreadChallenge.h"

#include "GameplayTasksComponent.h"
#include "StateTreeConditionBase.h"
#include "STT_GuideThread.h"

#include "GuideSystemStateTreeComponent.h"
#include "PlanetPlayerController.h"
#include "PlanetPlayerState.h"

#include "STT_GuideThread_Challenge.h"

void AGuideThread_Challenge::Destroyed()
{
	auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
	if (PCPtr)
	{
		PCPtr->GetPlayerState<APlanetPlayerState>()->SetEntryChanlleng(false);
	}
	
	Super::Destroyed();
}

void AGuideThread_Challenge::ActiveGuide()
{
	Super::ActiveGuide();
	
	// 确认是否在挑战模式
	auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
	if (PCPtr)
	{
		PCPtr->GetPlayerState<APlanetPlayerState>()->SetEntryChanlleng(true);
	}
}

AGuideThread_Challenge::AGuideThread_Challenge(
	const FObjectInitializer& ObjectInitializer
):
 Super(
	 ObjectInitializer.
	 SetDefaultSubobjectClass<
		 UGuideChallengeThreadSystemStateTreeComponent>(
		 UGuideChallengeThreadSystemStateTreeComponent::ComponentName
	 )
 )
{
}

FString AGuideThread_Challenge::GetGuideThreadTitle() const
{
	return FString::Printf(TEXT("%s·第%d层"),*TaskName, CurrentLevel);
}

void AGuideThread_Challenge::SetCurrentLevel(
	int32 NewCurrentLevel
	)
{
	CurrentLevel = NewCurrentLevel;
	OnGuideThreadNameChagned.Broadcast(GetGuideThreadTitle());
}

int32 AGuideThread_Challenge::GetCurrentLevel() const
{
	return CurrentLevel;
}

UStateTreeGuideChallengeThreadComponentSchema::UStateTreeGuideChallengeThreadComponentSchema()
{
	ContextActorClass = FOwnerType::StaticClass();

	ContextDataDescs[0].Struct = ContextActorClass.Get();
}

bool UStateTreeGuideChallengeThreadComponentSchema::IsStructAllowed(
	const UScriptStruct* InScriptStruct
) const
{
	return Super::IsStructAllowed(InScriptStruct) ||
		InScriptStruct->IsChildOf(FSTT_GuideThreadChallengeBase::StaticStruct()) ;
}

TSubclassOf<UStateTreeSchema> UGuideChallengeThreadSystemStateTreeComponent::GetSchema() const
{
	return UStateTreeGuideChallengeThreadComponentSchema::StaticClass();
}

bool UGuideChallengeThreadSystemStateTreeComponent::SetContextRequirements(
	FStateTreeExecutionContext& Context,
	bool bLogErrors
)
{
	Context.SetCollectExternalDataCallback(
		FOnCollectStateTreeExternalData::CreateUObject(
			this,
			&ThisClass::CollectExternalData
		)
	);
	return UStateTreeGuideChallengeThreadComponentSchema::SetContextRequirements(
		*this,
		Context,
		bLogErrors
	);
}
