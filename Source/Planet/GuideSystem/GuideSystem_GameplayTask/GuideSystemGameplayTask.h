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

UCLASS()
class PLANET_API UGameplayTask_Base : public UGameplayTask
{
	GENERATED_BODY()

public:
	
	void SetPlayerCharacter(AHumanCharacter_Player* PlayerCharacterPtr);

	EStateTreeRunStatus GetStateTreeRunStatus()const;
	
protected:

	EStateTreeRunStatus StateTreeRunStatus = EStateTreeRunStatus::Running;
	
	AHumanCharacter_Player* PlayerCharacterPtr = nullptr;
};

UCLASS()
class PLANET_API UGameplayTask_Guide : public UGameplayTask_Base
{
	GENERATED_BODY()

public:
	
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
class PLANET_API UGameplayTask_Interaction  : public UGameplayTask_Base
{
	GENERATED_BODY()

public:
	
	ACharacterBase* TargetCharacterPtr = nullptr;
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
		const TArray<FTaskNode_Conversation_SentenceInfo>& InConversationsAry,
		ACharacterBase* InTargetCharacterPtr
		);

protected:
	
	void ConditionalPerformTask();

	float RemainingTime = 0.f;
	
	TArray<FTaskNode_Conversation_SentenceInfo> ConversationsAry;
	
	int32 SentenceIndex = 0;
	
};
