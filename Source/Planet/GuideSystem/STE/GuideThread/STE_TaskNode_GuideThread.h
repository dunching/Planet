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
class AGuideInteractionActor;
class ACharacterBase;
class AHumanCharacter;
class AHumanCharacter_Player;

UCLASS(Blueprintable)
class PLANET_API USTE_TaskNode_GuideMainThread : public USTE_TaskNode
{
	GENERATED_BODY()

public:

	virtual void TreeStart(FStateTreeExecutionContext& Context)override;

	virtual void TreeStop(FStateTreeExecutionContext& Context)override;

	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime)override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Content)
	AGuideMainThread* GuideActorPtr = nullptr;

protected:

};
