// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "UObject/ObjectMacros.h"
#include "Engine/EngineTypes.h"
#include "AITypes.h"
#include "Navigation/PathFollowingComponent.h"
#include "Tasks/AITask.h"
#include "ProxyProcessComponent.h"
#include "TaskNode.h"
#include "BehaviorTree/Tasks/BTTask_RunDynamicStateTree.h"

#include "GuideSystemGameplayTask.generated.h"

class AHumanCharacter_Player;
class ATargetPoint_Runtime;
class AGuideThread;
class UPAD_TaskNode_Guide_AddToTarget;
class UPAD_TaskNode_Guide_ConversationWithTarget;
class UPAD_TaskNode_Interaction_Option;
class UPAD_TaskNode_Interaction_NotifyGuideThread;

UCLASS()
class PLANET_API UGameplayTask_Base : public UGameplayTask
{
	GENERATED_BODY()

public:
	
	void SetPlayerCharacter(AHumanCharacter_Player* PlayerCharacterPtr);

	void SetTaskID(const FGuid& InTaskID);

	void SetGuideActor(TObjectPtr<AGuideThread> InGuideActorPtr);

	EStateTreeRunStatus GetStateTreeRunStatus()const;
	
protected:

	EStateTreeRunStatus StateTreeRunStatus = EStateTreeRunStatus::Running;
	
	AHumanCharacter_Player* PlayerCharacterPtr = nullptr;

	TObjectPtr<AGuideThread> GuideActorPtr = nullptr;

	FGuid TaskID;
	
};

UCLASS()
class PLANET_API UGameplayTask_Guide : public UGameplayTask_Base
{
	GENERATED_BODY()

public:
	
	UGameplayTask_Guide(const FObjectInitializer& ObjectInitializer);

};

UCLASS()
class PLANET_API UGameplayTask_Guide_MoveToLocation : public UGameplayTask_Guide
{
	GENERATED_BODY()

public:
	
	UGameplayTask_Guide_MoveToLocation(const FObjectInitializer& ObjectInitializer);

	virtual void Activate() override;
	
	virtual void TickTask(float DeltaTime)override;

	virtual void OnDestroy(bool bInOwnerFinished) override;
	
	void SetUp(const FVector& TargetLocation,int32 ReachedRadius );

protected:
	
	FVector TargetLocation = FVector::ZeroVector;
	
	int32 ReachedRadius = 100;

	ATargetPoint_Runtime* TargetPointPtr = nullptr;
	
};

UCLASS()
class PLANET_API UGameplayTask_Guide_WaitInputKey : public UGameplayTask_Guide
{
	GENERATED_BODY()

public:
	
	UGameplayTask_Guide_WaitInputKey(const FObjectInitializer& ObjectInitializer);

	virtual void Activate() override;
	
	virtual void TickTask(float DeltaTime)override;

	FKey Key = EKeys::AnyKey;

protected:

	APlayerController*PCPtr = nullptr;
	
};

UCLASS()
class PLANET_API UGameplayTask_Guide_Monologue : public UGameplayTask_Guide
{
	GENERATED_BODY()

public:
	
	UGameplayTask_Guide_Monologue(const FObjectInitializer& ObjectInitializer);

	virtual void Activate() override;
	
	virtual void TickTask(float DeltaTime)override;

	virtual void OnDestroy(bool bInOwnerFinished) override;
	
	void SetUp(const TArray<FTaskNode_Conversation_SentenceInfo>& InConversationsAry);

protected:
	
	void ConditionalPerformTask();

	float RemainingTime = 0.f;
	
	TArray<FTaskNode_Conversation_SentenceInfo> ConversationsAry;
	
	int32 SentenceIndex = 0;
	
};

UCLASS()
class PLANET_API UGameplayTask_Guide_AddToTarget : public UGameplayTask_Guide
{
	GENERATED_BODY()

public:
	
	UGameplayTask_Guide_AddToTarget(const FObjectInitializer& ObjectInitializer);

	virtual void Activate() override;
	
	void SetUp(UPAD_TaskNode_Guide_AddToTarget* InTaskNodePtr);

protected:
	
	void ConditionalPerformTask();

	UPAD_TaskNode_Guide_AddToTarget* TaskNodePtr = nullptr;
	
};

UCLASS()
class PLANET_API UGameplayTask_Guide_ConversationWithTarget : public UGameplayTask_Guide
{
	GENERATED_BODY()

public:
	
	virtual void Activate() override;
	
	virtual void TickTask(float DeltaTime)override;

	virtual void OnDestroy(bool bInOwnerFinished) override;
	
	void SetUp(UPAD_TaskNode_Guide_ConversationWithTarget* InTaskNodePtr);

protected:
	
	void ConditionalPerformTask();

	UPAD_TaskNode_Guide_ConversationWithTarget* TaskNodePtr = nullptr;
	
	ATargetPoint_Runtime* TargetPointPtr = nullptr;
	
};

UCLASS()
class PLANET_API UGameplayTask_Interaction  : public UGameplayTask_Base
{
	GENERATED_BODY()

public:

	UGameplayTask_Interaction(const FObjectInitializer& ObjectInitializer);

	void SetTargetCharacterPtr(AHumanCharacter* InTargetCharacterPtr);

	// 激活互动的Character
	AHumanCharacter* TargetCharacterPtr = nullptr;
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

	int32 SelectedIndex = 0;
	
protected:
	
	void ConditionalPerformTask();

	void OnSelected(int32 Index);
	
	float RemainingTime = 0.f;
	
	TSoftObjectPtr<UPAD_TaskNode_Interaction_Option> TaskNodeRef;
	
};

UCLASS()
class PLANET_API UGameplayTask_Interaction_NotifyGuideThread : public UGameplayTask_Interaction
{
	GENERATED_BODY()

public:
	
	virtual void Activate() override;
	
	void SetUp(
		const TSoftObjectPtr<UPAD_TaskNode_Interaction_NotifyGuideThread>& InTaskNodeRef
		);

protected:
	
	void ConditionalPerformTask();

	TSoftObjectPtr<UPAD_TaskNode_Interaction_NotifyGuideThread> TaskNodeRef;
	
};
