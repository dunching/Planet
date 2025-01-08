#include "ConversationComponent.h"

#include "Blueprint/UserWidget.h"

#include "Planet_Tools.h"
#include "ConversationBorder.h"
#include "UICommon.h"
#include "CharacterBase.h"
#include "TaskNode.h"

UConversationComponent::UConversationComponent(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void UConversationComponent::CloseConversationborder_Implementation()
{
#if UE_EDITOR || UE_CLIENT
	if (GetOwnerRole() < ROLE_Authority)
	{
		if (ConversationBorderPtr)
		{
			ConversationBorderPtr->RemoveFromParent();
			ConversationBorderPtr = nullptr;
		}
	}
#endif
}

void UConversationComponent::DisplaySentence_Implementation(
	UPAD_TaskNode_Conversation* InPAD_TaskNode_ConversationPtr,
	int32 InSentenceIndex
	)
{
#if UE_EDITOR || UE_CLIENT
	if (GetOwnerRole() < ROLE_Authority)
	{
		PAD_TaskNode_ConversationPtr = InPAD_TaskNode_ConversationPtr;
		SentenceIndex = InSentenceIndex;

		if (ConversationBorderPtr)
		{
			ConversationBorderPtr->SetSentence(InPAD_TaskNode_ConversationPtr->ConversationsAry[SentenceIndex]);
		}
		else
		{
			ConversationBorderPtr = CreateWidget<UConversationBorder>(GetWorldImp(), UConversationBorderClass);
			if (ConversationBorderPtr)
			{
				ConversationBorderPtr->CharacterPtr = GetOwner<ACharacterBase>();
				ConversationBorderPtr->SetSentence(InPAD_TaskNode_ConversationPtr->ConversationsAry[SentenceIndex]);
				ConversationBorderPtr->AddToViewport(EUIOrder::kConversationBorder);
			}
		}
	}
#endif
}

FName UConversationComponent::ComponentName = TEXT("ConversationComponent");
