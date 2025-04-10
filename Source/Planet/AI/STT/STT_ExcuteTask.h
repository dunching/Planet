// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h"
#include "Tasks/StateTreeAITask.h"
#include <EnvironmentQuery/EnvQueryTypes.h>

#include "GenerateType.h"

#include "STT_ExcuteTask.generated.h"

class UEnvQuery;

class AHumanCharacter;
class AHumanCharacter_AI;
class AHumanAIController;
class UAITask_ExcuteTask_Base;
class UAITask_ExcuteTemporaryTask_Base;
class UAITask_Conversation;
class USTE_AICharacterController;
class UGloabVariable;
class UPAD_TaskNode;
class UPAD_TaskNode_Preset;
class UTaskNode_Temporary;

USTRUCT(Blueprintable, BlueprintType)
struct PLANET_API FSTT_Saying_SentenceInfo
{
	GENERATED_USTRUCT_BODY()

public:

	FSTT_Saying_SentenceInfo(){}
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FString Sentence;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float DelayTime = 1.f;

	// 念这句词的角色，为空则是“自己”念，否则把这句推送给 PlayerCharacter 念
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSoftObjectPtr<AHumanCharacter_AI>AvatorCharacterPtr = nullptr;
	
};
 
template<>
struct TStructOpsTypeTraits<FSTT_Saying_SentenceInfo> :
	public TStructOpsTypeTraitsBase2<FSTT_Saying_SentenceInfo>
{
	enum
	{
		WithNetSerializer = false,
	};
};

USTRUCT()
struct PLANET_API FStateTreeExcuteTaskAutomaticInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AHumanCharacter_AI> CharacterPtr = nullptr;

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AHumanAIController> AIControllerPtr = nullptr;

	UPROPERTY(Transient)
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner = nullptr;
};

/*
 * 让AI自言自语
 */
USTRUCT()
struct PLANET_API FSTT_ExcuteTask_Automatic : public FStateTreeAIActionTaskBase //FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeExcuteTaskAutomaticInstanceData;

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

	EStateTreeRunStatus PerformGameplayTask(FStateTreeExecutionContext& Context) const;
};

