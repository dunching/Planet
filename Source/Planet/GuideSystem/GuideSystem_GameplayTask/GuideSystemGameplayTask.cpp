#include "GuideSystemGameplayTask.h"

#include "Kismet/GameplayStatics.h"

#include "AssetRefMap.h"
#include "ConversationLayout.h"
#include "GuideActor.h"
#include "GuideSubSystem.h"
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

void UGameplayTask_Base::SetPlayerCharacter(AHumanCharacter_Player* InPlayerCharacterPtr)
{
	PlayerCharacterPtr = InPlayerCharacterPtr;
}

void UGameplayTask_Base::SetTaskID(const FGuid& InTaskID)
{
	TaskID = InTaskID;
}

void UGameplayTask_Base::SetGuideActor(TObjectPtr<AGuideThread> InGuideActorPtr)
{
	GuideActorPtr = InGuideActorPtr;
}

EStateTreeRunStatus UGameplayTask_Base::GetStateTreeRunStatus() const
{
	return StateTreeRunStatus;
}

UGameplayTask_Guide::UGameplayTask_Guide(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true;
	bIsPausable = true;
}

UGameplayTask_Guide_MoveToLocation::UGameplayTask_Guide_MoveToLocation(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true;
	bIsPausable = true;
}

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

void UGameplayTask_Guide_MoveToLocation::Activate()
{
	Super::Activate();

	if (PlayerCharacterPtr)
	{
		FTransform AbsoluteTransform = FTransform::Identity;
		AbsoluteTransform.SetLocation(TargetLocation);

		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = GetOwnerActor();

		TargetPointPtr = PlayerCharacterPtr->GetWorld()->SpawnActor<ATargetPoint_Runtime>(
			UAssetRefMap::GetInstance()->TargetPoint_RuntimeClass,
			AbsoluteTransform,
			SpawnParameters
		);
	}
}

void UGameplayTask_Guide_MoveToLocation::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if (FVector::Distance(TargetLocation, PlayerCharacterPtr->GetActorLocation()) < ReachedRadius)
	{
		StateTreeRunStatus = EStateTreeRunStatus::Succeeded;
		EndTask();
	}
}

void UGameplayTask_Guide_MoveToLocation::OnDestroy(bool bInOwnerFinished)
{
	if (TargetPointPtr)
	{
		TargetPointPtr->Destroy();
		TargetPointPtr = nullptr;
	}

	Super::OnDestroy(bInOwnerFinished);
}

void UGameplayTask_Guide_MoveToLocation::SetUp(const FVector& InTargetLocation, int32 InReachedRadius)
{
	TargetLocation = InTargetLocation;
	ReachedRadius = InReachedRadius;
}

UGameplayTask_Guide_WaitInputKey::UGameplayTask_Guide_WaitInputKey(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true;
	bIsPausable = true;
}

void UGameplayTask_Guide_WaitInputKey::Activate()
{
	Super::Activate();

	PCPtr = UGameplayStatics::GetPlayerController(this, 0);
	if (PCPtr)
	{
	}
}

void UGameplayTask_Guide_WaitInputKey::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if (PCPtr->IsInputKeyDown(Key))
	{
		StateTreeRunStatus = EStateTreeRunStatus::Succeeded;
		EndTask();
	}
}

UGameplayTask_Guide_Monologue::UGameplayTask_Guide_Monologue(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true;
	bIsPausable = true;
}

void UGameplayTask_Guide_Monologue::Activate()
{
	Super::Activate();

	ConditionalPerformTask();
}

void UGameplayTask_Guide_Monologue::TickTask(float DeltaTime)
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

void UGameplayTask_Guide_Monologue::OnDestroy(bool bInOwnerFinished)
{
	if (PlayerCharacterPtr)
	{
		PlayerCharacterPtr->GetConversationComponent()->CloseConversationborder();
	}

	Super::OnDestroy(bInOwnerFinished);
}

void UGameplayTask_Guide_Monologue::SetUp(const TArray<FTaskNode_Conversation_SentenceInfo>& InConversationsAry)
{
	ConversationsAry = InConversationsAry;
}

void UGameplayTask_Guide_Monologue::ConditionalPerformTask()
{
	if (ConversationsAry.IsValidIndex(SentenceIndex))
	{
		const auto& Ref = ConversationsAry[SentenceIndex];

		RemainingTime = Ref.DelayTime;

		PlayerCharacterPtr->GetConversationComponent()->DisplaySentence(Ref);
	}

	SentenceIndex++;
}

UGameplayTask_Guide_AddToTarget::UGameplayTask_Guide_AddToTarget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true;
	bIsPausable = true;
}

void UGameplayTask_Guide_AddToTarget::Activate()
{
	Super::Activate();

	ConditionalPerformTask();
}

void UGameplayTask_Guide_AddToTarget::SetUp(UPAD_TaskNode_Guide_AddToTarget* InTaskNodePtr)
{
	TaskNodePtr = InTaskNodePtr;
}

void UGameplayTask_Guide_AddToTarget::ConditionalPerformTask()
{
	if (
		TaskNodePtr &&
		TaskNodePtr->TargetCharacterPtr.IsValid() &&
		TaskNodePtr->GuideInteractionActorClass.Get()
	)
	{
		TaskNodePtr->TargetCharacterPtr->GetSceneActorInteractionComponent()->AddGuideActor(
			TaskNodePtr->GuideInteractionActorClass);

		StateTreeRunStatus = EStateTreeRunStatus::Succeeded;
		EndTask();
	}
}

void UGameplayTask_Guide_ConversationWithTarget::Activate()
{
	Super::Activate();

	ConditionalPerformTask();
}

void UGameplayTask_Guide_ConversationWithTarget::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if (GuideActorPtr)
	{
		if (GuideActorPtr->ConsumeEvent(TaskID))
		{
			StateTreeRunStatus = EStateTreeRunStatus::Succeeded;
			EndTask();
		}
		return;
	}
	
	StateTreeRunStatus = EStateTreeRunStatus::Failed;
	EndTask();
}

void UGameplayTask_Guide_ConversationWithTarget::OnDestroy(bool bInOwnerFinished)
{
	if (TargetPointPtr)
	{
		TargetPointPtr->Destroy();
		TargetPointPtr = nullptr;
	}
	
	Super::OnDestroy(bInOwnerFinished);
}

void UGameplayTask_Guide_ConversationWithTarget::SetUp(UPAD_TaskNode_Guide_ConversationWithTarget* InTaskNodePtr)
{
	TaskNodePtr = InTaskNodePtr;
}

void UGameplayTask_Guide_ConversationWithTarget::ConditionalPerformTask()
{
	if (
		TaskNodePtr &&
		TaskNodePtr->TargetCharacterPtr.IsValid()
	)
	{
		if (PlayerCharacterPtr)
		{
			FTransform AbsoluteTransform = FTransform::Identity;

			FActorSpawnParameters SpawnParameters;
			SpawnParameters.Owner = GetOwnerActor();

			TargetPointPtr = PlayerCharacterPtr->GetWorld()->SpawnActor<ATargetPoint_Runtime>(
				UAssetRefMap::GetInstance()->TargetPoint_RuntimeClass,
				AbsoluteTransform,
				SpawnParameters
			);

			TargetPointPtr->AttachToActor(TaskNodePtr->TargetCharacterPtr.Get(),
			                              FAttachmentTransformRules::KeepRelativeTransform);
		}
	}
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
	UGuideSubSystem::GetInstance()->GetCurrentGuideThread()->AddEvent(TaskNodeRef->TaskID);

	if (TargetCharacterPtr)
	{
		TargetCharacterPtr->GetSceneActorInteractionComponent()->RemoveGuideActor(TaskNodeRef->GuideInteractionActorClass);
	}
	
	StateTreeRunStatus = EStateTreeRunStatus::Succeeded;
	EndTask();
}
