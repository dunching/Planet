// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h"
#include "Tasks/StateTreeAITask.h"
#include <EnvironmentQuery/EnvQueryTypes.h>

#include "GenerateType.h"
#include "GuideThread.h"

#include "STT_CommonData.generated.h"

class AHumanCharacter_AI;

// 

USTRUCT(Blueprintable, BlueprintType)
struct PLANET_API FTaskNode_Conversation_SentenceInfo
{
	GENERATED_USTRUCT_BODY()

public:

	FTaskNode_Conversation_SentenceInfo(){}
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FString Sentence;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float DelayTime = 1.f;

	// 念这句词的角色，为空则是“自己”念，否则把这句推送给 AvatorCharacterPtr 念
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSoftObjectPtr<AHumanCharacter_AI>AvatorCharacterPtr = nullptr;
	
};
