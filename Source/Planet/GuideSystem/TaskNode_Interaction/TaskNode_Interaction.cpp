#include "TaskNode_Interaction.h"

UPAD_TaskNode_Interaction_Conversation::UPAD_TaskNode_Interaction_Conversation(
	const FObjectInitializer& ObjectInitializer
	):
	Super(ObjectInitializer)
{
	TaskNodeType = ETaskNodeType::kInteraction_Conversation;
}

UPAD_TaskNode_Interaction_Option::UPAD_TaskNode_Interaction_Option(
	const FObjectInitializer& ObjectInitializer
	):
	Super(ObjectInitializer)
{
	TaskNodeType = ETaskNodeType::kInteraction_Option;
}
