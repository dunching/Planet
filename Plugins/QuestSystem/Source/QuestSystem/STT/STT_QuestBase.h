// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h"

#include "QuestSystem_GenericType.h"

#include "STT_QuestBase.generated.h"

class AQuestActorBase;
class AQuestChainBase;
class AQuestChain_MainBase;
class UPAD_TaskNode_Guide;
class UPAD_TaskNode_Interaction;
class UGameplayTask_Quest;

USTRUCT()
struct QUESTSYSTEM_API FSTID_QuestBase
{
	GENERATED_BODY()

	FSTID_QuestBase();

	/**
	 * 每个阶段的ID
	 * 如要求玩家按下W为一个阶段
	 * 要求玩家按下S为一个阶段，ID需要不同
	 */
	UPROPERTY(VisibleAnywhere, Category = Param)
	FGuid TaskID;
	
	// 任务的输出参数
	UPROPERTY(EditAnywhere, Category = Output)
	FTaskNodeResuleHelper TaskNodeResuleHelper;

};

template<>
struct TStructOpsTypeTraits<FSTID_QuestBase> :
	public TStructOpsTypeTraitsBase2<FSTID_QuestBase>
{
	enum
	{
		WithZeroConstructor = true,
	};
};

#pragma region Record
USTRUCT()
struct QUESTSYSTEM_API FSTID_Quest_Record
{
	GENERATED_BODY()

	FSTID_Quest_Record();

	/**
	 * EditAnywhere 确保我们能够修改
	 */
	UPROPERTY(EditAnywhere, Category = Param)
	FGuid TaskID;
	
	UPROPERTY(
		EditAnywhere,
		Category = Context
	)
	TObjectPtr<AQuestActorBase> GuideActorPtr = nullptr;
};

/**
 * 引导任务的记录点
 */
USTRUCT()
struct QUESTSYSTEM_API FSTT_Guide_Record :
	public FStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_Quest_Record;

	virtual const UStruct* GetInstanceDataType() const override;

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;

protected:
};
#pragma endregion
