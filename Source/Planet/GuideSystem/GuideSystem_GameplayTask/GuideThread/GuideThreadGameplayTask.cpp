#include "GuideThreadGameplayTask.h"

#include "Kismet/GameplayStatics.h"

#include "AssetRefMap.h"
#include "ConversationLayout.h"
#include "GuideActor.h"
#include "GuideInteractionActor.h"
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
	TargetCharacterPtr = InTaskNodePtr->TargetCharacterPtr;
}

void UGameplayTask_Guide_ConversationWithTarget::SetUp(const TSoftObjectPtr<AHumanCharacter_AI>& InTargetCharacterPtr)
{
	TargetCharacterPtr = InTargetCharacterPtr;
}

void UGameplayTask_Guide_ConversationWithTarget::ConditionalPerformTask()
{
	if (
		TargetCharacterPtr.IsValid()
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

			TargetPointPtr->AttachToActor(TargetCharacterPtr.Get(),
			                              FAttachmentTransformRules::KeepRelativeTransform);
		}
	}
}

void UGameplayTask_Guide_WaitComplete::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
	
	if (GuideActorPtr)
	{
		const auto Result = GuideActorPtr->ConsumeEvent(TaskID);
		if (Result.GetIsValid())
		{
			TaskNodeResuleHelper = Result;
			
			StateTreeRunStatus = EStateTreeRunStatus::Succeeded;
			EndTask();
		}
	}
}

void UGameplayTask_Guide_WaitComplete::SetUp(const FGuid& InTaskID)
{
	TaskID = InTaskID;
}
