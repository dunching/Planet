// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h"
#include "Tasks/StateTreeAITask.h"
#include <EnvironmentQuery/EnvQueryTypes.h>

#include "GenerateTypes.h"
#include "GuideThread.h"

#include "STT_GuideBase.generated.h"

class AGuideActor;
class AGuideThread;
class AGuideThread_Main;
class AGuideInteraction_Actor;
class UPAD_TaskNode_Guide;
class UPAD_TaskNode_Interaction;
class ACharacterBase;
class AHumanCharacter;
class AHumanCharacter_Player;
class UGameplayTask_Base;

USTRUCT()
struct PLANET_API FSTID_GuideBase
{
	GENERATED_BODY()

	FSTID_GuideBase();

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
struct TStructOpsTypeTraits<FSTID_GuideBase> :
	public TStructOpsTypeTraitsBase2<FSTID_GuideBase>
{
	enum
	{
		WithZeroConstructor = true,
	};
};

#pragma region Record
USTRUCT()
struct PLANET_API FSTID_Guide_Record
{
	GENERATED_BODY()

	FSTID_Guide_Record();

	/**
	 * EditAnywhere 确保我们能够修改
	 */
	UPROPERTY(EditAnywhere, Category = Param)
	FGuid TaskID;
	
	UPROPERTY(
		EditAnywhere,
		Category = Context
	)
	TObjectPtr<AGuideActor> GuideActorPtr = nullptr;
};

/**
 * 引导任务的记录点
 */
USTRUCT()
struct PLANET_API FSTT_Guide_Record :
	public FStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_Guide_Record;

	virtual const UStruct* GetInstanceDataType() const override;

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;

protected:
};
#pragma endregion
