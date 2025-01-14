#include "TaskNode_SceneActor.h"

FString UPAD_TaskNode_Interaction::GetName() const
{
	return TEXT("");
}

UPAD_TaskNode_Interaction_Conversation::UPAD_TaskNode_Interaction_Conversation(
	const FObjectInitializer& ObjectInitializer
	):
	Super(ObjectInitializer)
{
	TaskNodeType = ETaskNodeType::kInteraction_Conversation;
}

FString UPAD_TaskNode_Interaction_Conversation::GetName() const
{
	return Name;
}
