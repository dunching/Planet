#include "GuideInteractionGameplayTask.h"

#include "Kismet/GameplayStatics.h"

#include "AssetRefMap.h"
#include "GuideActor.h"
#include "GuideInteraction.h"
#include "GuideSubSystem.h"
#include "GuideThread.h"
#include "HumanCharacter_AI.h"
#include "HumanCharacter_Player.h"
#include "HumanTransactionProcessor.h"
#include "InputProcessorSubSystem.h"
#include "InteractionConversationLayout.h"
#include "InteractionOptionsLayout.h"
#include "MainHUD.h"
#include "MainHUDLayout.h"
#include "OptionList.h"
#include "PlanetPlayerController.h"
#include "TargetPoint_Runtime.h"
#include "UIManagerSubSystem.h"

class AMainHUD;

UGameplayTask_Interaction::UGameplayTask_Interaction(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	bTickingTask = true;
	bIsPausable = true;
}

void UGameplayTask_Interaction::SetTargetCharacterPtr(
	AHumanCharacter_AI* InTargetCharacterPtr
	)
{
	TargetCharacterPtr = InTargetCharacterPtr;
}

void UGameplayTask_Interaction::SetGuideInteractionActor(
	AGuideInteraction_Actor* InTargetCharacterPtr
	)
{
	GuideActorPtr = InTargetCharacterPtr;
}

UGameplayTask_Interaction_Conversation::UGameplayTask_Interaction_Conversation(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	bTickingTask = true;
	bIsPausable = true;
}

void UGameplayTask_Interaction_Conversation::Activate()
{
	Super::Activate();

	if (bOnlyDisplyOnTitle)
	{
		
	}
	else
	{
		UUIManagerSubSystem::GetInstance()->SwitchLayout(
														 ELayoutCommon::kConversationLayout
														);
	}
	
	ConditionalPerformTask();
}

void UGameplayTask_Interaction_Conversation::TickTask(
	float DeltaTime
	)
{
	Super::TickTask(DeltaTime);

	if (bOnlyDisplyOnTitle)
	{
	}
	else
	{
		if (GuideActorPtr->bWantToStop)
		{
			EndTask();
		}
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

void UGameplayTask_Interaction_Conversation::OnDestroy(
	bool bInOwnerFinished
	)
{
	if (PlayerCharacterPtr)
	{
		TargetCharacterPtr->GetConversationComponent()->CloseConversationborder();
		
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

		if (bOnlyDisplyOnTitle)
		{
			TargetCharacterPtr->GetConversationComponent()->DisplaySentence(Ref);
		}
		else
		{
			PlayerCharacterPtr->GetConversationComponent()->DisplaySentence_Player(
																				   Ref,
																				   std::bind(
																						&ThisClass::CurrentSentenceStop,
																						this
																					   )
																				  );
		}
	}

	SentenceIndex++;
}

void UGameplayTask_Interaction_Conversation::CurrentSentenceStop()
{
	RemainingTime = 0.f;
}

UGameplayTask_Interaction_Option::UGameplayTask_Interaction_Option(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	bTickingTask = true;
	bIsPausable = true;
}

void UGameplayTask_Interaction_Option::Activate()
{
	Super::Activate();

	ConditionalPerformTask();
}

void UGameplayTask_Interaction_Option::TickTask(
	float DeltaTime
	)
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

void UGameplayTask_Interaction_Option::OnDestroy(
	bool bInOwnerFinished
	)
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

	UUIManagerSubSystem::GetInstance()->SwitchLayout(ELayoutCommon::kOptionLayout);
	UUIManagerSubSystem::GetInstance()->GetMainHUDLayout()->GetInteractionOptionsLayout()
		->GetOptions()->UpdateDisplay(OptionAry, std::bind(&ThisClass::OnSelected, this, std::placeholders::_1));
}

void UGameplayTask_Interaction_Option::OnSelected(
	int32 Index
	)
{
	UUIManagerSubSystem::GetInstance()->GetMainHUDLayout()->GetInteractionOptionsLayout()
		->CloseOption();

	SelectedIndex = Index;

	StateTreeRunStatus = EStateTreeRunStatus::Succeeded;
	EndTask();
}

void UGameplayTask_Interaction_Transaction::Activate()
{
	Super::Activate();

	UUIManagerSubSystem::GetInstance()->SwitchLayout(
	                                                 ELayoutCommon::kTransactionLayout,
	                                                 [this]()
	                                                 {
		                                                 EndTask();
	                                                 }
	                                                );
}
