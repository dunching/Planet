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
#include "ModifyItemProxyStrategy.h"
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
struct FOnEffectedTawrgetCallback;

UCLASS()
class PLANET_API UGameplayTask_Guide : public UGameplayTask_Base
{
	GENERATED_BODY()

public:
	UGameplayTask_Guide(
		const FObjectInitializer& ObjectInitializer
	);

	void SetGuideActor(
		TObjectPtr<AGuideThreadBase> InGuideActorPtr
	);

protected:
	TObjectPtr<AGuideThreadBase> GuideActorPtr = nullptr;

private:
};

UCLASS()
class PLANET_API UGameplayTask_Guide_MoveToLocation : public UGameplayTask_Guide
{
	GENERATED_BODY()

public:
	UGameplayTask_Guide_MoveToLocation(
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
		const FVector& TargetLocation,
		int32 ReachedRadius
	);

	TSubclassOf<ATargetPoint_Runtime>TargetPoint_RuntimeClass;

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
	UGameplayTask_Guide_WaitInputKey(
		const FObjectInitializer& ObjectInitializer
	);

	virtual void Activate() override;

	virtual void TickTask(
		float DeltaTime
	) override;

	FKey Key = EKeys::AnyKey;

protected:
	APlayerController* PCPtr = nullptr;
};

/**
 * NPC或Player的独白，不会阻挡输入或进入其他状态
 */
UCLASS()
class PLANET_API UGameplayTask_Guide_Monologue : public UGameplayTask_Guide
{
	GENERATED_BODY()

public:
	UGameplayTask_Guide_Monologue(
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

UCLASS()
class PLANET_API UGameplayTask_Guide_AddToTarget : public UGameplayTask_Guide
{
	GENERATED_BODY()

public:
	UGameplayTask_Guide_AddToTarget(
		const FObjectInitializer& ObjectInitializer
	);

	virtual void Activate() override;

	void SetUp(
		const TSubclassOf<AGuideInteractionBase>& InGuideInteractionActorClass,
		const TSoftObjectPtr<AHumanCharacter_AI>& InTargetCharacterPtr
	);

protected:
	void ConditionalPerformTask();

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<AGuideInteractionBase> GuideInteractionActorClass;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSoftObjectPtr<AHumanCharacter_AI> TargetCharacterPtr = nullptr;
};

/**
 * 要求Player的前往对NPC对话
 */
UCLASS()
class PLANET_API UGameplayTask_Guide_ConversationWithTarget : public UGameplayTask_Guide
{
	GENERATED_BODY()

public:
	virtual void Activate() override;

	virtual void OnDestroy(
		bool bInOwnerFinished
	) override;

	void SetUp(
		const TSoftObjectPtr<AHumanCharacter_AI>& InTargetCharacterPtr
	);

	TSubclassOf<ATargetPoint_Runtime>TargetPoint_RuntimeClass;
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
	virtual void TickTask(
		float DeltaTime
	) override;

	void SetUp(
		const FGuid& InTaskID
	);

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

	virtual void OnDestroy(
		bool bInOwnerFinished
	) override;

	void SetUp(
		const FGameplayTag& ResourceType,
		int32 Num
	);

	FTaskNodeDescript GetTaskNodeDescripton() const;

protected:
	void OnGetConsumableProxy(
		const TSharedPtr<FConsumableProxy>&,
		EProxyModifyType ProxyModifyType
	);

	void OnCoinProxyChanged(
		const TSharedPtr<FCoinProxy>&,
		EProxyModifyType ProxyModifyType,
		int32 Num
	);

	void UpdateDescription() const;

	FModifyItemProxyStrategy_Consumable::FOnConsumableProxyChanged::FCallbackHandleSPtr OnConsumableProxyChangedHandle;

	FModifyItemProxyStrategy_Coin::FOnCoinProxyChanged::FCallbackHandleSPtr OnCoinProxyChangedHandle;

	FGameplayTag ResourceType;

	int32 CurrentNum = 0;

	int32 Num = 0;
};

UCLASS()
class PLANET_API UGameplayTask_Guide_DefeatEnemy : public UGameplayTask_Guide
{
	GENERATED_BODY()

	using FMakedDamageHandle = TCallbackHandleContainer<void(
		const FOnEffectedTawrgetCallback&
	)>::FCallbackHandleSPtr;

public:
	virtual void Activate() override;

	virtual void OnDestroy(
		bool bInOwnerFinished
	) override;

	void SetUp(
		const FGameplayTag& ResourceType,
		int32 Num
	);

	FTaskNodeDescript GetTaskNodeDescripton() const;

protected:
	void OnActiveGEAddedDelegateToSelf(
		const FOnEffectedTawrgetCallback& ReceivedEventModifyDataCallback
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

	using FMakedDamageHandle = TCallbackHandleContainer<void(
		const FOnEffectedTawrgetCallback&
	)>::FCallbackHandleSPtr;

public:
	virtual void TickTask(
		float DeltaTime
	) override;

	void SetUp(
		int32 RemainTime
	);

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

	virtual void OnDestroy(
		bool bInOwnerFinished
	) override;

	TSubclassOf<UPlanetGameplayAbility> GAClass;

private:
	UFUNCTION()
	void FGenericAbilityDelegate(
		UGameplayAbility* GAPtr
	);

	FDelegateHandle DelegateHandle;
};

UCLASS()
class PLANET_API UGameplayTask_Guide_ActiveRun : public UGameplayTask_Guide
{
	GENERATED_BODY()

public:
	virtual void Activate() override;

	virtual void OnDestroy(
		bool bInOwnerFinished
	) override;

	TSubclassOf<UPlanetGameplayAbility> GAClass;

private:
	UFUNCTION()
	void FGenericAbilityDelegate(
		UGameplayAbility* GAPtr
	);

	FDelegateHandle DelegateHandle;
};

UCLASS()
class PLANET_API UGameplayTask_Guide_AttckCharacter : public UGameplayTask_Guide
{
	GENERATED_BODY()

public:
	virtual void Activate() override;

	virtual void OnDestroy(
		bool bInOwnerFinished
	) override;

	TObjectPtr<ACharacterBase> HumanCharacterAI = nullptr;

	bool bIsKill = true;
	
private:
	void OnEffectOhterCharacter(
		const FOnEffectedTawrgetCallback&
	);

	TCallbackHandleContainer<void(
		const FOnEffectedTawrgetCallback&
	)>::FCallbackHandleSPtr DelegateHandle;
};
