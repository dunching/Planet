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
#include "GuideThread.h"
#include "InventoryComponent.h"
#include "ProxyProcessComponent.h"
#include "STT_CommonData.h"

#include "GuideThreadGameplayTask.generated.h"

class AHumanCharacter_Player;
class ATargetPoint_Runtime;
class UGameplayAbility;
class UPlanetGameplayAbility;
class AGuideThread;
class UPAD_TaskNode_Guide_AddToTarget;
class UPAD_TaskNode_Guide_ConversationWithTarget;
class UPAD_TaskNode_Interaction_Option;
class UPAD_TaskNode_Interaction_NotifyGuideThread;

struct FCoinProxy;
struct FConsumableProxy;
struct FReceivedEventModifyDataCallback;

UCLASS()
class PLANET_API UGameplayTask_Guide : public UGameplayTask_Base
{
	GENERATED_BODY()

public:
	UGameplayTask_Guide(const FObjectInitializer& ObjectInitializer);

	void SetGuideActor(TObjectPtr<AGuideThread> InGuideActorPtr);

protected:

	TObjectPtr<AGuideThread> GuideActorPtr = nullptr;
	
private:
	
};

UCLASS()
class PLANET_API UGameplayTask_Guide_MoveToLocation : public UGameplayTask_Guide
{
	GENERATED_BODY()

public:
	UGameplayTask_Guide_MoveToLocation(const FObjectInitializer& ObjectInitializer);

	virtual void Activate() override;

	virtual void TickTask(float DeltaTime) override;

	virtual void OnDestroy(bool bInOwnerFinished) override;

	void SetUp(const FVector& TargetLocation, int32 ReachedRadius);

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

	virtual void TickTask(float DeltaTime) override;

	FKey Key = EKeys::AnyKey;

protected:
	APlayerController* PCPtr = nullptr;
};

UCLASS()
class PLANET_API UGameplayTask_Guide_Monologue : public UGameplayTask_Guide
{
	GENERATED_BODY()

public:
	UGameplayTask_Guide_Monologue(const FObjectInitializer& ObjectInitializer);

	virtual void Activate() override;

	virtual void TickTask(float DeltaTime) override;

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

	void SetUp(
		const TSubclassOf<AGuideInteraction_Actor>& InGuideInteractionActorClass,
		const TSoftObjectPtr<AHumanCharacter_AI>& InTargetCharacterPtr
	);

protected:
	void ConditionalPerformTask();

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<AGuideInteraction_Actor> GuideInteractionActorClass;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSoftObjectPtr<AHumanCharacter_AI> TargetCharacterPtr = nullptr;
};

UCLASS()
class PLANET_API UGameplayTask_Guide_ConversationWithTarget : public UGameplayTask_Guide
{
	GENERATED_BODY()

public:
	virtual void Activate() override;

	virtual void OnDestroy(bool bInOwnerFinished) override;

	void SetUp(const TSoftObjectPtr<AHumanCharacter_AI>& InTargetCharacterPtr);

protected:
	void ConditionalPerformTask();

	TSoftObjectPtr<AHumanCharacter_AI> TargetCharacterPtr;

	ATargetPoint_Runtime* TargetPointPtr = nullptr;
};

UCLASS()
class PLANET_API UGameplayTask_Guide_WaitComplete : public UGameplayTask_Guide
{
	GENERATED_BODY()

public:
	virtual void TickTask(float DeltaTime) override;

	void SetUp(const FGuid& InTaskID);

	FTaskNodeResuleHelper TaskNodeResuleHelper;

protected:
	FGuid TaskID;
};

UCLASS()
class PLANET_API UGameplayTask_Guide_CollectResource : public UGameplayTask_Guide
{
	GENERATED_BODY()

public:
	virtual void Activate() override;

	virtual void OnDestroy(bool bInOwnerFinished) override;

	void SetUp(const FGameplayTag& ResourceType, int32 Num);

	FTaskNodeDescript GetTaskNodeDescripton() const;

protected:
	void OnGetConsumableProxy(const TSharedPtr<FConsumableProxy>&, EProxyModifyType ProxyModifyType);

	void OnCoinProxyChanged(const TSharedPtr<FCoinProxy>&, EProxyModifyType ProxyModifyType, int32 Num);

	void UpdateDescription() const;

	UInventoryComponent::FOnConsumableProxyChanged::FCallbackHandleSPtr OnConsumableProxyChangedHandle;

	UInventoryComponent::FOnCoinProxyChanged::FCallbackHandleSPtr OnCoinProxyChangedHandle;

	FGameplayTag ResourceType;

	int32 CurrentNum = 0;

	int32 Num = 0;
};

UCLASS()
class PLANET_API UGameplayTask_Guide_DefeatEnemy : public UGameplayTask_Guide
{
	GENERATED_BODY()

	using FMakedDamageHandle = TCallbackHandleContainer<void(const FReceivedEventModifyDataCallback&)>::FCallbackHandleSPtr;

public:
	virtual void Activate() override;

	virtual void OnDestroy(bool bInOwnerFinished) override;

	void SetUp(const FGameplayTag& ResourceType, int32 Num);

	FTaskNodeDescript GetTaskNodeDescripton() const;

protected:
	void OnActiveGEAddedDelegateToSelf(
		const FReceivedEventModifyDataCallback& ReceivedEventModifyDataCallback
	);

	void UpdateDescription() const;

	FMakedDamageHandle DelegateHandle;

	FGameplayTag EnemyType;

	int32 CurrentNum = 0;

	int32 Num = 0;
};

UCLASS()
class PLANET_API UGameplayTask_Guide_ReturnOpenWorld : public UGameplayTask_Guide
{
	GENERATED_BODY()

	using FMakedDamageHandle = TCallbackHandleContainer<void(const FReceivedEventModifyDataCallback&)>::FCallbackHandleSPtr;

public:
	virtual void TickTask(float DeltaTime) override;

	void SetUp(int32 RemainTime);

	FTaskNodeDescript GetTaskNodeDescripton() const;

protected:

	float TotalTime = 0.0f;

	int32 RemainTime = 1;
};

UCLASS()
class PLANET_API UGameplayTask_Guide_ActiveDash : public UGameplayTask_Guide
{
	GENERATED_BODY()
public:

	virtual void Activate() override;

	TSubclassOf<UPlanetGameplayAbility>GAClass;

private:

	UFUNCTION()
	void FGenericAbilityDelegate( UGameplayAbility*GAPtr);
};

UCLASS()
class PLANET_API UGameplayTask_Guide_ActiveRun : public UGameplayTask_Guide
{
	GENERATED_BODY()
public:

	virtual void Activate() override;

	TSubclassOf<UPlanetGameplayAbility>GAClass;

private:

	UFUNCTION()
	void FGenericAbilityDelegate( UGameplayAbility*GAPtr);
};
