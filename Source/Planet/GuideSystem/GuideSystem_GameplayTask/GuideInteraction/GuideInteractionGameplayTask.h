// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "UObject/ObjectMacros.h"
#include "Engine/EngineTypes.h"
#include "AITypes.h"
#include "Navigation/PathFollowingComponent.h"
#include "Tasks/AITask.h"
#include "BehaviorTree/Tasks/BTTask_RunDynamicStateTree.h"

#include "GuideSystemGameplayTask.h"
#include "ProxyProcessComponent.h"
#include "STT_CommonData.h"


#include "GuideInteractionGameplayTask.generated.h"

class AHumanCharacter_AI;
class AHumanCharacter_Player;
class ATargetPoint_Runtime;
class AGuideThread;
class AGuideInteraction_Actor;
class UPAD_TaskNode_Guide_AddToTarget;
class UPAD_TaskNode_Guide_ConversationWithTarget;
class UPAD_TaskNode_Interaction_Option;
class UPAD_TaskNode_Interaction_NotifyGuideThread;

UCLASS()
class PLANET_API UGameplayTask_Interaction  : public UGameplayTask_Base
{
	GENERATED_BODY()

public:

	UGameplayTask_Interaction(const FObjectInitializer& ObjectInitializer);

	void SetTargetCharacterPtr(AHumanCharacter_AI* InTargetCharacterPtr);

	void SetGuideInteractionActor(AGuideInteraction_Actor* InTargetCharacterPtr);

	TObjectPtr<AGuideInteraction_Actor> GuideActorPtr = nullptr;
	
	// 激活互动的Character
	AHumanCharacter_AI* TargetCharacterPtr = nullptr;
};

UCLASS()
class PLANET_API UGameplayTask_Interaction_Conversation : public UGameplayTask_Interaction
{
	GENERATED_BODY()

public:
	
	UGameplayTask_Interaction_Conversation(const FObjectInitializer& ObjectInitializer);

	virtual void Activate() override;
	
	virtual void TickTask(float DeltaTime)override;

	virtual void OnDestroy(bool bInOwnerFinished) override;
	
	void SetUp(
		const TArray<FTaskNode_Conversation_SentenceInfo>& InConversationsAry
		);
	
protected:
	
	void ConditionalPerformTask();

	void CurrentSentenceStop();
	
	float RemainingTime = 0.f;
	
	TArray<FTaskNode_Conversation_SentenceInfo> ConversationsAry;
	
	int32 SentenceIndex = 0;
	
};

UCLASS()
class PLANET_API UGameplayTask_Interaction_Option : public UGameplayTask_Interaction
{
	GENERATED_BODY()

public:
	
	UGameplayTask_Interaction_Option(const FObjectInitializer& ObjectInitializer);

	virtual void Activate() override;
	
	virtual void TickTask(float DeltaTime)override;

	virtual void OnDestroy(bool bInOwnerFinished) override;
	
	void SetUp(
		const TSoftObjectPtr<UPAD_TaskNode_Interaction_Option>& InTaskNodeRef
		);

	void SetUp(
		const TArray<FString>& InOptionAry,
		float DelayTime
		);

	int32 SelectedIndex = 0;
	
protected:
	
	void ConditionalPerformTask();

	void OnSelected(int32 Index);
	
	float RemainingTime = 0.f;
	
	// 
	TArray<FString> OptionAry;

	// 时间限制，<0为无限制
	float DelayTime = -1.f;
	
};

UCLASS()
class PLANET_API UGameplayTask_Interaction_Transaction : public UGameplayTask_Interaction
{
	GENERATED_BODY()

public:
	
	virtual void Activate() override;
	
};
