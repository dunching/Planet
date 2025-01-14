
// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "UObject/ObjectMacros.h"
#include "Engine/EngineTypes.h"

#include "AITask_ExcuteTask_Base.h"

#include "AITask_Conversation.generated.h"


struct FSocket_FASI;

class UGameplayAbility;

class UPAD_TaskNode;
class UAIComponent;

UCLASS()
class PLANET_API UAITask_Conversation : public UAITask_ExcuteTask_Base
{
	GENERATED_BODY()

public:

	UAITask_Conversation(const FObjectInitializer& ObjectInitializer);

	void ConditionalPerformTask();

protected:

	virtual void Activate() override;
	
	virtual void TickTask(float DeltaTime)override;

	virtual void OnDestroy(bool bInOwnerFinished) override;
	
	float RemainingTime = 0.f;

	int32 SentenceIndex = 0;
	
};

UCLASS()
class PLANET_API UAITask_Conversation_SingleSentence : public UAITask_ExcuteTemporaryTask_Base
{
	GENERATED_BODY()

public:

	UAITask_Conversation_SingleSentence(const FObjectInitializer& ObjectInitializer);

	void ConditionalPerformTask();

protected:

	virtual void Activate() override;
	
	virtual void TickTask(float DeltaTime)override;

	virtual void OnDestroy(bool bInOwnerFinished) override;

	float RemainingTime = 0.f;

};
