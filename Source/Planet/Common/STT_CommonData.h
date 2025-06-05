// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h"
#include "Tasks/StateTreeAITask.h"
#include <EnvironmentQuery/EnvQueryTypes.h>

#include "GenerateTypes.h"
#include "STT_CommonData_Base.h"

#include "STT_CommonData.generated.h"

class AHumanCharacter_AI;

// 

#pragma region STT
USTRUCT(Blueprintable, BlueprintType)
struct PLANET_API FTaskNode_Conversation_SentenceInfo : public FTaskNode_Conversation_SentenceInfo_Base
{
	GENERATED_USTRUCT_BODY()

public:
	FTaskNode_Conversation_SentenceInfo()
	{
	}

	// 念这句词的角色，为空则是“自己”念，否则把这句推送给 AvatorCharacterPtr 念
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSoftObjectPtr<AHumanCharacter_AI> AvatorCharacterPtr = nullptr;
};
#pragma endregion
