// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Templates/SubclassOf.h"
#include <Perception/AIPerceptionTypes.h>
#include "Blueprint/StateTreeEvaluatorBlueprintBase.h"
#include "StateTreeEvaluatorBase.h"
#include "StateTreeExecutionContext.h"

#include "GenerateType.h"
#include "OpenWorldDataLayer.h"
#include "OpenWorldSystem.h"
#include "STE_TaskNode.h"
#include "STE_TaskNode_GuideThread.h"

#include "STE_GuideThread_Challenge.generated.h"

class AGuideChallengeThread;

UCLASS(Blueprintable)
class PLANET_API UGloabVariable_GuideChallengeThread : public UObject
{
	GENERATED_BODY()

public:

	TArray<TWeakObjectPtr<ACharacterBase>> SpwanedCharacterAry;

	/**
	 * 引导生成的即时Actor
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output)
	TArray<TObjectPtr<AActor>> TemporaryActorAry;

	/**
	 * 选择的关卡类型
	 */
	ETeleport Teleport = ETeleport::kChallenge_LevelType_1;
	
};

UCLASS(Blueprintable)
class PLANET_API USTE_TaskNode_GuideChallengeThread : public USTE_TaskNode_GuideThread
{
	GENERATED_BODY()

public:

	virtual void TreeStart(FStateTreeExecutionContext& Context)override;

	virtual void TreeStop(FStateTreeExecutionContext& Context)override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Content)
	AGuideChallengeThread* GuideActorPtr = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output)
	UGloabVariable_GuideChallengeThread* GloabVariable = nullptr;

};
