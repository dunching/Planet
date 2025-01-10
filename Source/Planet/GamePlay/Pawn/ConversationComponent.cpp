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
		const FTaskNode_Conversation_SentenceInfo&Sentence
	)
{
#if UE_EDITOR || UE_CLIENT
	if (GetOwnerRole() < ROLE_Authority)
	{
		if (ConversationBorderPtr)
		{
			ConversationBorderPtr->SetSentence(Sentence);
		}
		else
		{
			ConversationBorderPtr = CreateWidget<UConversationBorder>(GetWorldImp(), UConversationBorderClass);
			if (ConversationBorderPtr)
			{
				ConversationBorderPtr->CharacterPtr = GetOwner<ACharacterBase>();
				ConversationBorderPtr->SetSentence(Sentence);
				ConversationBorderPtr->AddToViewport(EUIOrder::kConversationBorder);
			}
		}
	}
#endif
}

FName UConversationComponent::ComponentName = TEXT("ConversationComponent");
