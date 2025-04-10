// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h"
#include "Tasks/StateTreeAITask.h"
#include <EnvironmentQuery/EnvQueryTypes.h>

#include "GenerateType.h"
#include "GuideActor.h"
#include "STT_GuideBase.h"
#include "STT_GuideThread.h"


#include "STT_GuideThread_Challenge.generated.h"

class ATeleport;
class AHumanCharacter_AI;
class UGloabVariable_GuideChallengeThread;

#pragma region ChallengeBase
USTRUCT()
struct PLANET_API FSTID_GuideThreadChallengeBase :
	public FSTID_GuideThreadBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = Context)
	UGloabVariable_GuideChallengeThread* GloabVariable = nullptr;
};

USTRUCT()
struct PLANET_API FSTT_GuideThreadChallengeBase :
	public FSTT_GuideThreadBase
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
	
	UPROPERTY(EditAnywhere, Category = Param)
	TArray<ETeleport> ChallengeAry;
	
	UPROPERTY(EditAnywhere, Category = Param)
	int32 Distance = 200;
	
	UPROPERTY(Transient)
	TSoftObjectPtr<ATeleport> TeleportRef;
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
	TArray<TSoftClassPtr<AHumanCharacter_AI>> NPCAry;
};

USTRUCT()
struct PLANET_API FSTID_GuideThreadSpawnNPCs :
	public FSTID_GuideThreadChallengeBase
{
	GENERATED_BODY()
	
	/**
	 * 每波生成的NPC信息
	 */
	UPROPERTY(EditAnywhere, Category = Param)
	TArray<FSpawnNPCsHelper>PerWaveNum;

	int32 WaveIndex = 0;

	TArray<FGuid>CharacterIDAry;
	
	TArray<TWeakObjectPtr<AHumanCharacter_AI>>CharacterAry;
};

/**
 * 生成多波NPC并等待消亡
 */
USTRUCT()
struct PLANET_API FSTT_GuideThreadSpawnNPCs :
	public FSTT_GuideThreadChallengeBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_GuideThreadSpawnNPCs;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	
	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;
	
	virtual EStateTreeRunStatus Tick(
		FStateTreeExecutionContext& Context,
		const float DeltaTime
	) const override;

	bool SpawnNPC(FStateTreeExecutionContext& Context)const;
	
};
#pragma endregion
