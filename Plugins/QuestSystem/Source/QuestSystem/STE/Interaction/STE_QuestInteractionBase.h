// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Templates/SubclassOf.h"
#include "StateTreeEvaluatorBase.h"

#include "QuestsActorBase.h"
#include "STE_Base.h"

#include "STE_QuestInteractionBase.generated.h"

class AGuideActor;
class AQuestChain_Main;
class AGuideInteraction_Actor;
class AGuideInteraction_HumanCharacter_AI;
class AGuideInteraction_ChallengeEntry;
class ACharacterBase;
class AHumanCharacter;
class AHumanCharacter_Player;
class AHumanCharacter_AI;

UCLASS(Blueprintable)
class QUESTSYSTEM_API UGloabVariable_InteractionBase : public UObject
{
	GENERATED_BODY()

public:

	// 上条任务的输出参数
	UPROPERTY(EditAnywhere, Category = Output)
	FTaskNodeResuleHelper TaskNodeResuleHelper;

};

UCLASS(Blueprintable)
class QUESTSYSTEM_API USTE_QuestInteractionBase : public USTE_Base
{
	GENERATED_BODY()

public:

	virtual void TreeStart(FStateTreeExecutionContext& Context)override;

	virtual void TreeStop(FStateTreeExecutionContext& Context)override;

	/**
	 * 任务之间共享数据
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output)
	UGloabVariable_InteractionBase* GloabVariable = nullptr;

	// // 上条任务的输出参数
	// UPROPERTY(EditAnywhere, Category = Output)
	// int32 LastTaskOut = 0;

protected:

};
