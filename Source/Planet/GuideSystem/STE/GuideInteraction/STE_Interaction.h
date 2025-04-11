// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Templates/SubclassOf.h"
#include <Perception/AIPerceptionTypes.h>
#include "Blueprint/StateTreeEvaluatorBlueprintBase.h"
#include "StateTreeEvaluatorBase.h"
#include "StateTreeExecutionContext.h"

#include "GenerateType.h"
#include "GuideActor.h"
#include "STE_Base.h"

#include "STE_Interaction.generated.h"

class AGuideActor;
class AGuideThread_Main;
class AGuideInteraction_Actor;
class AGuideInteraction_HumanCharacter_AI;
class ACharacterBase;
class AHumanCharacter;
class AHumanCharacter_Player;
class AHumanCharacter_AI;

UCLASS(Blueprintable)
class PLANET_API UGloabVariable_Interaction : public UObject
{
	GENERATED_BODY()

public:

	// 上条任务的输出参数
	UPROPERTY(EditAnywhere, Category = Output)
	FTaskNodeResuleHelper TaskNodeResuleHelper;

};

UCLASS(Blueprintable)
class PLANET_API USTE_Interaction : public USTE_Base
{
	GENERATED_BODY()

public:

	virtual void TreeStart(FStateTreeExecutionContext& Context)override;

	virtual void TreeStop(FStateTreeExecutionContext& Context)override;

	/**
	 * 任务之间共享数据
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output)
	UGloabVariable_Interaction* GloabVariable = nullptr;

	// // 上条任务的输出参数
	// UPROPERTY(EditAnywhere, Category = Output)
	// int32 LastTaskOut = 0;

protected:

};

UCLASS(Blueprintable)
class PLANET_API USTE_Interaction_HumanCharacter_AI : public USTE_Interaction
{
	GENERATED_BODY()

public:

	virtual void TreeStart(FStateTreeExecutionContext& Context)override;

	virtual void TreeStop(FStateTreeExecutionContext& Context)override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Content)
	AGuideInteraction_HumanCharacter_AI* GuideActorPtr = nullptr;

};
