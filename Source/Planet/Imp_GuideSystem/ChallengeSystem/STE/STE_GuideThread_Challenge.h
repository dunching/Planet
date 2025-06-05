// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Templates/SubclassOf.h"
#include <Perception/AIPerceptionTypes.h>
#include "Blueprint/StateTreeEvaluatorBlueprintBase.h"
#include "StateTreeEvaluatorBase.h"
#include "StateTreeExecutionContext.h"

#include "GenerateTypes.h"
#include "OpenWorldDataLayer.h"
#include "OpenWorldSystem.h"
#include "STE_Base.h"
#include "STE_GuideThread.h"

#include "STE_GuideThread_Challenge.generated.h"

class AGuideThread_Challenge;

UCLASS(Blueprintable)
class PLANET_API UGloabVariable_GuideThread_Challenge : public UGloabVariable_GuideThreadBase
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
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output)
	ETeleport Teleport = ETeleport::kChallenge_LevelType_1;

};

UCLASS(Blueprintable)
class PLANET_API USTE_TaskNode_GuideChallengeThread : public USTE_GuideThreadBase
{
	GENERATED_BODY()

public:

	virtual void TreeStart(FStateTreeExecutionContext& Context)override;

	virtual void TreeStop(FStateTreeExecutionContext& Context)override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Content)
	AGuideThread_Challenge* GuideActorPtr = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output)
	UGloabVariable_GuideThread_Challenge* GloabVariable_Challenge = nullptr;

};
