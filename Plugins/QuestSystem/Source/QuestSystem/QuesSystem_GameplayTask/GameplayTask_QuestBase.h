// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "UObject/ObjectMacros.h"
#include "Engine/EngineTypes.h"
#include "BehaviorTree/Tasks/BTTask_RunDynamicStateTree.h"

#include "GameplayTask_QuestBase.generated.h"

class APlanetPlayerController;
class AHumanCharacter_Player;
class ATargetPoint_Runtime;
class ISceneActorInteractionInterface;
class ASceneActor;
class AQuestChain;
class UPAD_TaskNode_Guide_AddToTarget;
class UPAD_TaskNode_Guide_ConversationWithTarget;
class UPAD_TaskNode_Interaction_Option;
class UPAD_TaskNode_Interaction_NotifyGuideThread;
class UPAD_GuideThread_WaitInteractionSceneActor;

struct FTeammate;
struct FCharacterProxy;

UCLASS()
class QUESTSYSTEM_API UGameplayTask_QuestBase : public UGameplayTask
{
	GENERATED_BODY()

public:
	EStateTreeRunStatus GetStateTreeRunStatus() const;

private:
	void SetTaskID(
		const FGuid& InTaskID
		);

protected:
	EStateTreeRunStatus StateTreeRunStatus = EStateTreeRunStatus::Running;

	FGuid TaskID;
};
