#include "AITask_Conversation.h"
// Copyright Epic Games, Inc. All Rights Reserved.

#include "UObject/Package.h"
#include "TimerManager.h"
#include "AISystem.h"
#include "AIController.h"
#include "VisualLogger/VisualLogger.h"
#include "AIResources.h"
#include "GameplayTasksComponent.h"

#include "ProxyProcessComponent.h"

#include "CharacterBase.h"
#include "Skill_Base.h"
#include "AssetRefMap.h"
#include "GameplayTagsLibrary.h"
#include "CharacterAbilitySystemComponent.h"

#include "AIComponent.h"
#include "AITask_ReleaseSkill.h"
#include "ConversationComponent.h"
#include "HumanCharacter_AI.h"
#include "TaskNode.h"
#include "TaskNode_Character/TaskNode_Character.h"

UAITask_Conversation::UAITask_Conversation(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true;
	bIsPausable = true;
}

void UAITask_Conversation::Activate()
{
	Super::Activate();

	ConditionalPerformTask();
}

void UAITask_Conversation::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	RemainingTime -= DeltaTime;

	if (RemainingTime <= 0.f)
	{
		if (CharacterPtr)
		{
			CharacterPtr->GetConversationComponent()->CloseConversationborder();
		}

		SentenceIndex++;
		ConditionalPerformTask();
	}
}

void UAITask_Conversation::ConditionalPerformTask()
{
	if (CurrentTaskNodePtr->IsA(UPAD_TaskNode_Preset_Conversation::StaticClass()))
	{
		auto ConversationTaskNodePtr = Cast<UPAD_TaskNode_Preset_Conversation>(CurrentTaskNodePtr);
		if (ConversationTaskNodePtr)
		{
			if (SentenceIndex < ConversationTaskNodePtr->ConversationsAry.Num())
			{
				auto CurrentSentence = ConversationTaskNodePtr->ConversationsAry[SentenceIndex];
				RemainingTime = CurrentSentence.DelayTime;

				// 这句由Owner念
				if (CurrentSentence.AvatorCharacterPtr == nullptr)
				{
					CharacterPtr->GetConversationComponent()->DisplaySentence(ConversationTaskNodePtr->ConversationsAry[SentenceIndex]);
				}
				// 这句由AvatorCharacterPtr念
				else if (CurrentSentence.AvatorCharacterPtr != CharacterPtr)
				{
					auto TaskNodePtr = NewObject<UTaskNode_Temporary_Conversation>(CharacterPtr);

					TaskNodePtr->Sentence = CurrentSentence;
					CurrentSentence.AvatorCharacterPtr->GetAIComponent()->AddTemporaryTaskNode(TaskNodePtr);
				}
			}
			else
			{
				EndTask();
			}
		}
	}
}

void UAITask_Conversation::OnDestroy(bool bInOwnerFinished)
{
	SentenceIndex = 0;

	if (CharacterPtr)
	{
		CharacterPtr->GetConversationComponent()->CloseConversationborder();
	}

	Super::OnDestroy(bInOwnerFinished);
}

UAITask_Conversation_SingleSentence::UAITask_Conversation_SingleSentence(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true;
	bIsPausable = true;
}

void UAITask_Conversation_SingleSentence::Activate()
{
	Super::Activate();

	ConditionalPerformTask();
}

void UAITask_Conversation_SingleSentence::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	RemainingTime -= DeltaTime;

	if (RemainingTime <= 0.f)
	{
		EndTask();
	}
}

void UAITask_Conversation_SingleSentence::ConditionalPerformTask()
{
	if (CurrentTaskNodePtr->IsA(UTaskNode_Temporary_Conversation::StaticClass()))
	{
		auto ConversationTaskNodePtr = Cast<UTaskNode_Temporary_Conversation>(CurrentTaskNodePtr);
		if (ConversationTaskNodePtr)
		{
			RemainingTime = ConversationTaskNodePtr->Sentence.DelayTime;
			
			CharacterPtr->GetConversationComponent()->DisplaySentence(ConversationTaskNodePtr->Sentence);
		}
	}
}

void UAITask_Conversation_SingleSentence::OnDestroy(bool bInOwnerFinished)
{
	if (CharacterPtr)
	{
		CharacterPtr->GetConversationComponent()->CloseConversationborder();
	}

	Super::OnDestroy(bInOwnerFinished);
}