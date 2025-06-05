// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h"
#include "Tasks/StateTreeAITask.h"
#include <EnvironmentQuery/EnvQueryTypes.h>

#include "GenerateTypes.h"
#include "STT_GuideBase.h"
#include "STT_CommonData.h"
#include "STT_GuideThread.h"


#include "STT_GuideThread_Area.generated.h"

class ASceneActor;
class AResourceBoxBase;
class ATargetPoint_Runtime;
class ATargetPoint;
class AGeneratorBase;
class AGeneratorColony_ByInvoke;
class AGeneratorColony_ByTime;
class AAreaVolume;
class AGuideActor;
class AGuideThread;
class AGuideThread_Main;
class AGuideInteractionBase;
class ACharacterBase;
class AHumanCharacter;
class AHumanCharacter_AI;
class AHumanCharacter_Player;
class UPAD_TaskNode_Guide;
class UPAD_TaskNode_Interaction;
class UPAD_TaskNode_Guide_LeaveHere;
class UPAD_TaskNode_Guide_SpwanNPCColony;
class UGloabVariable_GuideBrandThread;
class UGloabVariable_GuideThread_Area;
class UGameplayTask_Base;
class UGameplayTask_Guide_WaitComplete;
class UGameplayTask_Guide_ConversationWithTarget;
class UGameplayTask_Guide_AddToTarget;
class UGameplayTask_Guide_CollectResource;
class UGameplayTask_Guide_DefeatEnemy;
class UGameplayTask_Guide_ReturnOpenWorld;
class UGameplayTask_WaitInteractionSceneActor;
class UGameplayTask_WaitPlayerEquipment;

struct FConsumableProxy;
struct FTaskNode_Conversation_SentenceInfo;

#pragma region 生成一组NPC
UCLASS(
	Blueprintable,
	BlueprintType
)
class PLANET_API UPAD_GuideThread_SpwanNPCColony : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(
		BlueprintReadOnly,
		EditAnywhere
	)
	TSoftObjectPtr<AGeneratorColony_ByInvoke> ActorPtr;
};

USTRUCT()
struct PLANET_API FSTID_GuideThreadSpwanNPCColony :
	public FSTID_GuideThreadBase
{
	GENERATED_BODY()

	UPROPERTY(
		EditAnywhere,
		Category = Param
	)
	TSoftObjectPtr<UPAD_GuideThread_SpwanNPCColony> PAD;

	UPROPERTY(
		EditAnywhere,
		Category = Context
	)
	UGloabVariable_GuideThread_Area* GloabVariable_Area = nullptr;
};

// 执行引导任务 给目标角色添加互动引导内容
USTRUCT()
struct PLANET_API FSTT_GuideThreadSpwanNPCColony :
	public FSTT_GuideThreadBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_GuideThreadSpwanNPCColony;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;

	virtual EStateTreeRunStatus Tick(
		FStateTreeExecutionContext& Context,
		const float DeltaTime
	) const override;

protected:
};
#pragma endregion

#pragma region 等待指定的NPC死亡
USTRUCT()
struct PLANET_API FSTID_GuideThreadWaitCharacterDie :
	public FSTID_GuideThreadBase
{
	GENERATED_BODY()

	UPROPERTY(
		EditAnywhere,
		Category = Context
	)
	UGloabVariable_GuideThread_Area* GloabVariable_Area = nullptr;
};

// 执行引导任务 等待目标NPC死亡
USTRUCT()
struct PLANET_API FSTT_GuideThreadWaitCharacterDie :
	public FSTT_GuideThreadBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_GuideThreadWaitCharacterDie;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;

	virtual EStateTreeRunStatus Tick(
		FStateTreeExecutionContext& Context,
		const float DeltaTime
	) const override;

protected:
	
	virtual FTaskNodeDescript GetTaskNodeDescripton(FStateTreeExecutionContext& Context) const override;
	
};
#pragma endregion
