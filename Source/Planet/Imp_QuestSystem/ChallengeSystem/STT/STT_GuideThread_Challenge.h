// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h"

#include "STT_QuestBase.h"
#include "STT_QuestChain.h"


#include "STT_GuideThread_Challenge.generated.h"

class ATeleport;
class AHumanCharacter_AI;
class UGloabVariable_GuideThread_Challenge;
class AGuideThread_Challenge;

#pragma region ChallengeBase
USTRUCT()
struct PLANET_API FSTID_GuideThreadChallengeBase :
	public FSTID_QuestChainBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Context)
	UGloabVariable_GuideThread_Challenge* GloabVariable_Challenge = nullptr;
};

USTRUCT()
struct PLANET_API FSTT_GuideThreadChallengeBase :
	public FSTT_QuestChainBase
{
	GENERATED_BODY()
	using FInstanceDataType = FSTID_GuideThreadChallengeBase;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); };
};
#pragma endregion

#pragma region EntryNextLevel

enum class ETeleport : uint8;

USTRUCT()
struct PLANET_API FSTID_GuideThreadEntryNextLevel :
	public FSTID_GuideThreadChallengeBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Context)
	AGuideThread_Challenge* ChallengeGuideTrheadActorPtr = nullptr;
	
	/**
	 * 会在这里地图里随机选择
	 */
	UPROPERTY(EditAnywhere, Category = Param)
	TArray<ETeleport> ChallengeAry;
};

// 执行引导任务 
USTRUCT()
struct PLANET_API FSTT_GuideThreadEntryNextLevel :
	public FSTT_GuideThreadChallengeBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_GuideThreadEntryNextLevel;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); };

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;

	virtual EStateTreeRunStatus Tick(
		FStateTreeExecutionContext& Context,
		const float DeltaTime
	) const override;
};
#pragma endregion

#pragma region SpawnNPCs

USTRUCT(BlueprintType)
struct PLANET_API FSpawnNPCsHelper
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<AHumanCharacter_AI>> NPCAry;
};

USTRUCT()
struct PLANET_API FSTID_GuideThread_Challenge_SpawnNPCs :
	public FSTID_GuideThreadChallengeBase
{
	GENERATED_BODY()

	/**
	 * 每波生成的NPC信息
	 */
	UPROPERTY(EditAnywhere, Category = Param)
	TArray<FSpawnNPCsHelper> PerWaveNum;

	int32 WaveIndex = 0;

	TArray<FGuid> CharacterIDAry;

	TArray<TWeakObjectPtr<AHumanCharacter_AI>> CharacterAry;
	
	/**
	 * 间隔时间
	 */
	UPROPERTY(
		EditAnywhere,
		Category = Param
	)
	int32 DelayTime = 1;

	float RemainTime = 0.f;
};

/**
 * 生成多波NPC并等待消亡
 */
USTRUCT()
struct PLANET_API FSTT_GuideThread_Challenge_SpawnNPCs :
	public FSTT_GuideThreadChallengeBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_GuideThread_Challenge_SpawnNPCs;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;

	virtual EStateTreeRunStatus Tick(
		FStateTreeExecutionContext& Context,
		const float DeltaTime
	) const override;

	virtual void ExitState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;

protected:
	virtual FTaskNodeDescript GetTaskNodeDescripton(
		FStateTreeExecutionContext& Context
	) const override;

	bool SpawnNPC(
		FStateTreeExecutionContext& Context
	) const;
};
#pragma endregion
