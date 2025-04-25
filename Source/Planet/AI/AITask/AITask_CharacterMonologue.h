// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "AITask_CharacterBase.h"
#include "STT_CommonData.h"

#include "AITask_CharacterMonologue.generated.h"

/**
 * NPC或Player的独白，不会阻挡输入或进入其他状态
 */
UCLASS()
class PLANET_API UAITask_CharacterMonologue : public UAITask_CharacterBase
{
	GENERATED_BODY()

public:
	UAITask_CharacterMonologue(
		const FObjectInitializer& ObjectInitializer
	);

	virtual void Activate() override;

	virtual void TickTask(
		float DeltaTime
	) override;

	virtual void OnDestroy(
		bool bInOwnerFinished
	) override;

	void SetUp(
		const TArray<FTaskNode_Conversation_SentenceInfo>& InConversationsAry
	);

protected:
	void ConditionalPerformTask();

	float RemainingTime = 0.f;

	TArray<FTaskNode_Conversation_SentenceInfo> ConversationsAry;

	int32 SentenceIndex = 0;
};
