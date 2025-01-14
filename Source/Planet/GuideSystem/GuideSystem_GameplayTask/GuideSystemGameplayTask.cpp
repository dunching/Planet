#include "GuideSystemGameplayTask.h"

#include "AssetRefMap.h"
#include "HumanCharacter_Player.h"
#include "TargetPoint_Runtime.h"
#include "Kismet/GameplayStatics.h"

void UGameplayTask_Base::SetPlayerCharacter(AHumanCharacter_Player* InPlayerCharacterPtr)
{
	PlayerCharacterPtr = InPlayerCharacterPtr;
}

EStateTreeRunStatus UGameplayTask_Base::GetStateTreeRunStatus() const
{
	return StateTreeRunStatus;
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
		const auto &Ref = ConversationsAry[SentenceIndex];
		
		RemainingTime = Ref.DelayTime;

		PlayerCharacterPtr->GetConversationComponent()->DisplaySentence(Ref);
	}
	
	SentenceIndex++;
}

UGameplayTask_Interaction_Conversation::UGameplayTask_Interaction_Conversation(const FObjectInitializer& ObjectInitializer)
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
	const TArray<FTaskNode_Conversation_SentenceInfo>& InConversationsAry,
		ACharacterBase* InTargetCharacterPtr
	)
{
	ConversationsAry = InConversationsAry;
	TargetCharacterPtr = InTargetCharacterPtr;
}

void UGameplayTask_Interaction_Conversation::ConditionalPerformTask()
{
	if (ConversationsAry.IsValidIndex(SentenceIndex))
	{
		const auto &Ref = ConversationsAry[SentenceIndex];
		
		RemainingTime = Ref.DelayTime;

		PlayerCharacterPtr->GetConversationComponent()->DisplaySentence(Ref);
	}
	
	SentenceIndex++;
}
