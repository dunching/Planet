// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Templates/SubclassOf.h"
#include <Perception/AIPerceptionTypes.h>
#include "Blueprint/StateTreeEvaluatorBlueprintBase.h"
#include "StateTreeEvaluatorBase.h"
#include "StateTreeExecutionContext.h"

#include "GenerateTypes.h"
#include "STE_Base.h"
#include "STE_QuestInteractionBase.h"

#include "STE_Interaction.generated.h"

class AGuideActor;
class AQuestChain_Main;
class AQuestInteractionBase;
class AGuideInteraction_HumanCharacter_AI;
class AGuideInteraction_ChallengeEntry;
class ACharacterBase;
class AHumanCharacter;
class AHumanCharacter_Player;
class AHumanCharacter_AI;

UCLASS(Blueprintable)
class PLANET_API USTE_Interaction_HumanCharacter_AI : public USTE_QuestInteractionBase
{
	GENERATED_BODY()

public:

	virtual void TreeStart(FStateTreeExecutionContext& Context)override;

	virtual void TreeStop(FStateTreeExecutionContext& Context)override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Content)
	AGuideInteraction_HumanCharacter_AI* GuideActorPtr = nullptr;

};

UCLASS(Blueprintable)
class PLANET_API USTE_Interaction_ChallengeEntry : public USTE_QuestInteractionBase
{
	GENERATED_BODY()

public:

	virtual void TreeStart(FStateTreeExecutionContext& Context)override;

	virtual void TreeStop(FStateTreeExecutionContext& Context)override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Content)
	AGuideInteraction_ChallengeEntry* GuideActorPtr = nullptr;

};
