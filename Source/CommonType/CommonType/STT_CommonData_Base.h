// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "STT_CommonData_Base.generated.h"

// 

#pragma region STT
USTRUCT(Blueprintable, BlueprintType)
struct COMMONTYPE_API FTaskNode_Conversation_SentenceInfo_Base
{
	GENERATED_USTRUCT_BODY()

public:
	FTaskNode_Conversation_SentenceInfo_Base();

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FString Sentence;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float DelayTime = 1.f;
};

#pragma endregion
