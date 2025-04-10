#include "GuideInteractionGameplayTask.h"

#include "Kismet/GameplayStatics.h"

#include "AssetRefMap.h"
#include "ConversationLayout.h"
#include "GuideActor.h"
#include "GuideInteraction.h"
#include "GuideSubSystem.h"
#include "GuideThread.h"
#include "HumanCharacter_AI.h"
#include "HumanCharacter_Player.h"
#include "MainHUD.h"
#include "MainHUDLayout.h"
#include "OptionList.h"
#include "PlanetPlayerController.h"
#include "TargetPoint_Runtime.h"



class AMainHUD;

UGameplayTask_Interaction::UGameplayTask_Interaction(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true;
	bIsPausable = true;
}

void UGameplayTask_Interaction::SetTargetCharacterPtr(AHumanCharacter* InTargetCharacterPtr)
{
	TargetCharacterPtr = InTargetCharacterPtr;
}

void UGameplayTask_Interaction::SetGuideInteractionActor(AGuideInteraction_Actor* InTargetCharacterPtr)
{
	GuideActorPtr = InTargetCharacterPtr;
}

UGameplayTask_Interaction_Conversation::UGameplayTask_Interaction_Conversation(
	const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true;
	bIsPausable = true;
}

void UGameplayTask_Interaction_Conversation::Activate()
{
	Super::Activate();

	ConditionalPerformTask();
}

void UGameplayTask_Interaction_Conversation::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if (GuideActorPtr->bWantToStop)
	{
		EndTask();
	}
	
	RemainingTime -= DeltaTime;

	if (RemainingTime <= 0.f)
	{
		if (SentenceIndex < ConversationsAry.Num())
		{
			ConditionalPerformTask();
		}
		else
		{
			EndTask();
		}
	}
}

void UGameplayTask_Interaction_Conversation::OnDestroy(bool bInOwnerFinished)
{
	if (PlayerCharacterPtr)
	{
		PlayerCharacterPtr->GetConversationComponent()->CloseConversationborder_Player();
	}

	Super::OnDestroy(bInOwnerFinished);
}

void UGameplayTask_Interaction_Conversation::SetUp(
	const TArray<FTaskNode_Conversation_SentenceInfo>& InConversationsAry
)
{
	ConversationsAry = InConversationsAry;
}

void UGameplayTask_Interaction_Conversation::ConditionalPerformTask()
{
	if (ConversationsAry.IsValidIndex(SentenceIndex))
	{
		const auto& Ref = ConversationsAry[SentenceIndex];

		RemainingTime = Ref.DelayTime;

		PlayerCharacterPtr->GetConversationComponent()->DisplaySentence_Player(Ref,std::bind(&ThisClass::CurrentSentenceStop, this));
	}

	SentenceIndex++;
}

void UGameplayTask_Interaction_Conversation::CurrentSentenceStop()
{
	RemainingTime = 0.f;
}

UGameplayTask_Interaction_Option::UGameplayTask_Interaction_Option(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true;
	bIsPausable = true;
}

void UGameplayTask_Interaction_Option::Activate()
{
	Super::Activate();

	ConditionalPerformTask();
}

void UGameplayTask_Interaction_Option::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if (DelayTime > 0.f)
	{
		RemainingTime -= DeltaTime;

		if (RemainingTime <= 0.f)
		{
			StateTreeRunStatus = EStateTreeRunStatus::Succeeded;
			EndTask();
		}
	}
	else
	{
	}
}

void UGameplayTask_Interaction_Option::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);
}

void UGameplayTask_Interaction_Option::SetUp(
	const TSoftObjectPtr<UPAD_TaskNode_Interaction_Option>& InTaskNodeRef
)
{
}

void UGameplayTask_Interaction_Option::SetUp(
	const TArray<FString>& InOptionAry,
	float InDelayTime
	)
{
	OptionAry = InOptionAry;
	DelayTime = InDelayTime;
}

void UGameplayTask_Interaction_Option::ConditionalPerformTask()
{
	if (DelayTime > 0.f)
	{
		RemainingTime = DelayTime;
	}

	Cast<APlanetPlayerController>(UGameplayStatics::GetPlayerController(this, 0))
		->GetHUD<AMainHUD>()
		->GetMainHUDLayout()
		->GetConversationLayout()
		->GetOptions()->UpdateDisplay(OptionAry, std::bind(&ThisClass::OnSelected, this, std::placeholders::_1));
}

void UGameplayTask_Interaction_Option::OnSelected(int32 Index)
{
	Cast<APlanetPlayerController>(UGameplayStatics::GetPlayerController(this, 0))
		->GetHUD<AMainHUD>()
		->GetMainHUDLayout()
		->GetConversationLayout()
		->CloseOption();

	SelectedIndex = Index;

	StateTreeRunStatus = EStateTreeRunStatus::Succeeded;
	EndTask();
}
