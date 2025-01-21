// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "UObject/ObjectMacros.h"
#include "Engine/EngineTypes.h"
#include "AITypes.h"
#include "Navigation/PathFollowingComponent.h"
#include "Tasks/AITask.h"
#include "BehaviorTree/Tasks/BTTask_RunDynamicStateTree.h"

#include "GuideInteractionGameplayTask.h"
#include "ProxyProcessComponent.h"
#include "TaskNode.h"

#include "GuideThreadGameplayTask.generated.h"

class AHumanCharacter_Player;
class ATargetPoint_Runtime;
class AGuideThread;
class UPAD_TaskNode_Guide_AddToTarget;
class UPAD_TaskNode_Guide_ConversationWithTarget;
class UPAD_TaskNode_Interaction_Option;
class UPAD_TaskNode_Interaction_NotifyGuideThread;

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

	int32 SelectedIndex = -1;
	
protected:
	
	void ConditionalPerformTask();

	UPAD_TaskNode_Guide_ConversationWithTarget* TaskNodePtr = nullptr;
	
	ATargetPoint_Runtime* TargetPointPtr = nullptr;
	
};
