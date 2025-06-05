
#include "AITask_CharacterMonologue.h"

#include "ConversationComponent.h"
#include "HumanCharacter_AI.h"
#include "STT_CommonData.h"

UAITask_CharacterMonologue::UAITask_CharacterMonologue(
	const FObjectInitializer& ObjectInitializer
) :
  Super(ObjectInitializer)
{
	bTickingTask = true;
	bIsPausable = true;
}

void UAITask_CharacterMonologue::Activate()
{
	Super::Activate();

	ConditionalPerformTask();
}

void UAITask_CharacterMonologue::TickTask(
	float DeltaTime
)
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
			EndTask();
		}
	}
}

void UAITask_CharacterMonologue::OnDestroy(
	bool bInOwnerFinished
)
{
	if (PlayerCharacterPtr)
	{
		PlayerCharacterPtr->GetConversationComponent()->CloseConversationborder();
	}

	Super::OnDestroy(bInOwnerFinished);
}

void UAITask_CharacterMonologue::SetUp(
	const TArray<FTaskNode_Conversation_SentenceInfo>& InConversationsAry
)
{
	ConversationsAry = InConversationsAry;
}

void UAITask_CharacterMonologue::ConditionalPerformTask()
{
	if (ConversationsAry.IsValidIndex(SentenceIndex))
	{
		const auto& Ref = ConversationsAry[SentenceIndex];

		RemainingTime = Ref.DelayTime;

		PlayerCharacterPtr->GetConversationComponent()->DisplaySentence(Ref);
	}

	SentenceIndex++;
}
