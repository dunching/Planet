#include "ConversationComponent.h"

#include "Blueprint/UserWidget.h"

#include "Planet_Tools.h"
#include "ConversationBorder.h"
#include "UICommon.h"
#include "CharacterBase.h"
#include "CharacterTitleComponent.h"


UConversationComponent::UConversationComponent(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void UConversationComponent::DisplaySentence_Player(
	const FTaskNode_Conversation_SentenceInfo& Sentence,
	const std::function<void()>&SentenceStop_
	)
{
}

void UConversationComponent::CloseConversationborder_Player()
{
}

void UConversationComponent::CloseConversationborder_Implementation()
{
#if UE_EDITOR || UE_CLIENT
	if (GetOwnerRole() < ROLE_Authority)
	{
		auto OwnerCharacterOtr = GetOwner<FOwnerType>();
		OwnerCharacterOtr->GetCharacterTitleComponent()->CloseConversationborder();
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
		auto OwnerCharacterOtr = GetOwner<FOwnerType>();
		OwnerCharacterOtr->GetCharacterTitleComponent()->DisplaySentence(Sentence);
	}
#endif
}

FName UConversationComponent::ComponentName = TEXT("ConversationComponent");
