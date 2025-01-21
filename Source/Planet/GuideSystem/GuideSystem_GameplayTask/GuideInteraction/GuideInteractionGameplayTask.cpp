#include "GuideInteractionGameplayTask.h"

#include "Kismet/GameplayStatics.h"

#include "AssetRefMap.h"
#include "ConversationLayout.h"
#include "GuideActor.h"
#include "GuideSubSystem.h"
#include "GuideThreadActor.h"
#include "HumanCharacter_AI.h"
#include "HumanCharacter_Player.h"
#include "MainHUD.h"
#include "MainHUDLayout.h"
#include "OptionList.h"
#include "PlanetPlayerController.h"
#include "TargetPoint_Runtime.h"
#include "TaskNode_Guide.h"
#include "TaskNode_Interaction.h"

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

	RemainingTime -= DeltaTime;

	if (RemainingTime <= 0.f)
	{
		if (SentenceIndex < ConversationsAry.Num())
		{
			ConditionalPerformTask();
		}
		else
		{
			StateTreeRunStatus = EStateTreeRunStatus::Succeeded;
			EndTask();
		}
	}
}

void UGameplayTask_Interaction_Conversation::OnDestroy(bool bInOwnerFinished)
{
	if (PlayerCharacterPtr)
	{
		PlayerCharacterPtr->GetConversationComponent()->CloseConversationborder();
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

		PlayerCharacterPtr->GetConversationComponent()->DisplaySentence(Ref);
	}

	SentenceIndex++;
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

	if (TaskNodeRef->DelayTime > 0.f)
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
	TaskNodeRef = InTaskNodeRef;
}

void UGameplayTask_Interaction_Option::ConditionalPerformTask()
{
	if (TaskNodeRef->DelayTime > 0.f)
	{
		RemainingTime = TaskNodeRef->DelayTime;
	}

	Cast<APlanetPlayerController>(UGameplayStatics::GetPlayerController(this, 0))
		->GetHUD<AMainHUD>()
		->GetMainHUDLayout()
		->GetConversationLayout()
		->GetOptions()->UpdateDisplay(TaskNodeRef, std::bind(&ThisClass::OnSelected, this, std::placeholders::_1));
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

void UGameplayTask_Interaction_NotifyGuideThread::Activate()
{
	Super::Activate();

	ConditionalPerformTask();
}

void UGameplayTask_Interaction_NotifyGuideThread::SetUp(
	const TSoftObjectPtr<UPAD_TaskNode_Interaction_NotifyGuideThread>& InTaskNodeRef
	)
{
	TaskNodeRef = InTaskNodeRef;
}

void UGameplayTask_Interaction_NotifyGuideThread::ConditionalPerformTask()
{
	FTaskNodeResuleHelper TaskNodeResuleHelper;

	TaskNodeResuleHelper.TaskId = TaskNodeRef->TaskID;
	TaskNodeResuleHelper.Output_1 = 1;
	
	UGuideSubSystem::GetInstance()->GetCurrentGuideThread()->AddEvent(TaskNodeResuleHelper);

	if (TargetCharacterPtr)
	{
		TargetCharacterPtr->GetSceneActorInteractionComponent()->RemoveGuideActor(TaskNodeRef->GuideInteractionActorClass);
	}
	
	StateTreeRunStatus = EStateTreeRunStatus::Succeeded;
	EndTask();
}
