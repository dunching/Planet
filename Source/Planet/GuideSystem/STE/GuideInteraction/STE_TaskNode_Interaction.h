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

#include "STE_TaskNode_Interaction.generated.h"

class AGuideActor;
class AGuideMainThread;
class AGuideInteractionActor;
class ACharacterBase;
class AHumanCharacter;
class AHumanCharacter_Player;

UCLASS(Blueprintable)
class PLANET_API USTE_TaskNode_Interaction : public UStateTreeEvaluatorBlueprintBase
{
	GENERATED_BODY()

public:

	virtual void TreeStop(FStateTreeExecutionContext& Context)override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Content)
	AGuideInteractionActor* GuideActorPtr = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Content)
	AHumanCharacter_Player* PlayerCharacter = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Content)
	AHumanCharacter* TargetCharacter = nullptr;

	// // 上条任务的输出参数
	// UPROPERTY(EditAnywhere, Category = Output)
	// int32 LastTaskOut = 0;

protected:

};
