// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Templates/SubclassOf.h"
#include <Perception/AIPerceptionTypes.h>
#include "Blueprint/StateTreeEvaluatorBlueprintBase.h"
#include "StateTreeEvaluatorBase.h"
#include "StateTreeExecutionContext.h"

#include "GenerateType.h"
#include "STE_TaskNode.h"

#include "STE_TaskNode_GuideThread.generated.h"

class AGuideActor;
class AGuideMainThread;
class AGuideBranchThread;
class AGuideChallengeThread;
class AGuideInteraction_Actor;
class ACharacterBase;
class AHumanCharacter;
class AHumanCharacter_Player;

UCLASS(Blueprintable)
class PLANET_API USTE_TaskNode_GuideThread : public USTE_TaskNode
{
	GENERATED_BODY()

public:

	virtual void TreeStart(FStateTreeExecutionContext& Context)override;

	virtual void TreeStop(FStateTreeExecutionContext& Context)override;

	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime)override;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = Output)
	AHumanCharacter_Player* PlayerCharacter = nullptr;

protected:

};

UCLASS(Blueprintable)
class PLANET_API USTE_TaskNode_GuideMainThread : public USTE_TaskNode_GuideThread
{
	GENERATED_BODY()

public:

	virtual auto TreeStop(FStateTreeExecutionContext& Context) -> void override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Content)
	AGuideMainThread* GuideActorPtr = nullptr;

};

UCLASS(Blueprintable)
class PLANET_API UGloabVariable_GuideBrandThread : public UObject
{
	GENERATED_BODY()

public:

	TArray<TWeakObjectPtr<ACharacterBase>> SpwanedCharacterAry;

	/**
	 * 引导生成的即时Actor
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output)
	TArray<TObjectPtr<AActor>> TemporaryActorAry;

};

UCLASS(Blueprintable)
class PLANET_API USTE_TaskNode_GuideBrandThread : public USTE_TaskNode_GuideThread
{
	GENERATED_BODY()

public:

	virtual void TreeStart(FStateTreeExecutionContext& Context)override;

	virtual void TreeStop(FStateTreeExecutionContext& Context)override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Content)
	AGuideBranchThread* GuideActorPtr = nullptr;

	/**
	 * 任务之间共享数据
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output)
	UGloabVariable_GuideBrandThread* GloabVariable = nullptr;

};
