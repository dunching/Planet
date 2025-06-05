// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h"
#include "Tasks/StateTreeAITask.h"
#include <EnvironmentQuery/EnvQueryTypes.h>

#include "AITask_CharacterMonologue.h"
#include "STT_CharacterBase.h"

#include "STT_CharacterMonologue.generated.h"

class UEnvQuery;

class AHumanCharacter;
class AHumanCharacter_AI;
class AHumanAIController;
class UAITask_CharacterMonologue;
class UAITask_ExcuteTemporaryTask_Base;
class UAITask_Conversation;
class USTE_Assistance;
class UGloabVariable_Character;
class UPAD_TaskNode;
class UPAD_TaskNode_Preset;
class UTaskNode_Temporary;
class UAITask_CharacterMonologue;

struct FTaskNode_Conversation_SentenceInfo;

USTRUCT()
struct PLANET_API FSTID_CharacterMonologue : public FSTID_CharacterBase
{
	GENERATED_BODY()

	UPROPERTY(
		EditAnywhere,
		Category = Param
	)
	TArray<FTaskNode_Conversation_SentenceInfo> ConversationsAry;

	UPROPERTY(
		Transient
	)
	TObjectPtr<UAITask_CharacterMonologue> GameplayTaskPtr = nullptr;

	UPROPERTY(
		Transient
	)
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner = nullptr;
};

/*
 * 让AI自言自语
 */
USTRUCT()
struct PLANET_API FSTT_CharacterMonologue : public FStateTreeAIActionTaskBase //FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_CharacterMonologue;

	using FAITaskType_Conversation = UAITask_Conversation;

	using FAITaskType_MoveToLocation = UAITask_Conversation;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;

	virtual void ExitState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;

	virtual EStateTreeRunStatus Tick(
		FStateTreeExecutionContext& Context,
		const float DeltaTime
	) const override;

	EStateTreeRunStatus PerformGameplayTask(
		FStateTreeExecutionContext& Context
	) const;
};
