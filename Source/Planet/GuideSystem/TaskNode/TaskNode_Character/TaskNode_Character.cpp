#include "TaskNode_Character.h"

#include "GeneratorColony.h"
#include "CharacterBase.h"

#include "Net/UnrealNetwork.h"

#include "HumanAIController.h"

FTaskNode_Conversation_SentenceInfo::FTaskNode_Conversation_SentenceInfo()
{
}

void UPAD_TaskNode_Preset_Conversation::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(ThisClass, ConversationsAry, COND_None);
}

UPAD_TaskNode_Preset_AutomaticConversation::UPAD_TaskNode_Preset_AutomaticConversation(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	TaskNodeType = ETaskNodeType::kAutomatic_Conversatin;
}

UTaskNode_Temporary_Conversation::UTaskNode_Temporary_Conversation(const FObjectInitializer& ObjectInitializer)
{
	TaskNodeType = ETaskNodeType::kTemporary_Conversation;
}

void UTaskNode_Temporary_Conversation::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(ThisClass, Sentence, COND_None);
}

UPAD_TaskNode_Preset_Conversation::UPAD_TaskNode_Preset_Conversation(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	TaskNodeType = ETaskNodeType::kConversatin;
}
 