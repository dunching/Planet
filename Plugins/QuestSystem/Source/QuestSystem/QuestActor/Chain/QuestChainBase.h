// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "BrainComponent.h"
#include "GameplayTagContainer.h"
#include "Components/StateTreeComponentSchema.h"

#include "QuestsActorBase.h"
#include "QuestSystemStateTreeComponent.h"
#include "QuestChainType.h"

#include "QuestChainBase.generated.h"

class UGameplayTasksComponent;
class APlayerController;

class UQuestSubSystem;
class UQuestSystemStateTreeComponent;
class UPAD_TaskNode_Guide;

struct FTaskNodeDescript;
struct FSTT_GuideThread_Completet;

using FOnGuideInteractionEnd = TMulticastDelegate<void()>;

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UGuideThreadInterfacetion : public UInterface
{
	GENERATED_BODY()
};

class QUESTSYSTEM_API IGuideThreadInterfacetion
{
	GENERATED_BODY()

public:
	virtual EQuestChainType GetGuideThreadType() const = 0;

private:
};

#pragma region Base

/**
 *	任务链
 *	仅在 ROLE_AutonomousProxy 执行
 */
UCLASS(BlueprintType, Blueprintable)
class QUESTSYSTEM_API AQuestChainBase : public AQuestActorBase
{
	GENERATED_BODY()

	using FOnGuideThreadNameChagned = TMulticastDelegate<void(
		const FString&
		)>;

public:
	friend FSTT_GuideThread_Completet;
	friend UQuestSubSystem;

	AQuestChainBase(
		const FObjectInitializer& ObjectInitializer
		);

	void UpdateCurrentTaskNode(
		const TSoftObjectPtr<UPAD_TaskNode_Guide>& InTaskNode
		);

	void UpdateCurrentTaskNode(
		const FTaskNodeDescript& TaskNodeDescript
		);

	FTaskNodeDescript GetCurrentTaskNodeDescript() const;
	
	// 添加任务执行结果
	void AddEvent(
		const FTaskNodeResuleHelper& TaskNodeResuleHelper
		);

	FTaskNodeResuleHelper ConsumeEvent(
		const FGuid& InGuid
		);

	virtual FString GetGuideThreadTitle() const;

	// 任务节点类型：支线/支线
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag TaskNodeCategory;

	// 任务描述
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Description;

	/**
	 * 任务完成后的奖励
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FGameplayTag, int32> RewardProxysMap;

	FOnGuideThreadNameChagned OnGuideThreadNameChagned;

protected:
	// 任务名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString TaskName;

	/**
	 * 引导任务内，某些选项的结果保存在这里
	 */
	TMap<FGuid, FTaskNodeResuleHelper> EventsSet;

	FTaskNodeDescript CurrentTaskNodeDescript;

private:
	bool bIsComleted = false;
};

UCLASS()
class QUESTSYSTEM_API USTQuestChainComponentSchemaBase : public UStateTreeComponentSchema
{
	GENERATED_BODY()

public:
	using FOwnerType = AQuestChainBase;

	USTQuestChainComponentSchemaBase();

	virtual bool IsStructAllowed(
		const UScriptStruct* InScriptStruct
		) const override;

	virtual void PostLoad() override;

	static bool SetContextRequirements(
		UBrainComponent& BrainComponent,
		FStateTreeExecutionContext& Context,
		bool bLogErrors = false
		);

	UPROPERTY(EditAnywhere, Category = "Defaults", NoClear)
	TSubclassOf<APlayerController> PlayerControllerClass = nullptr;
};

/**
 *
 */
UCLASS()
class QUESTSYSTEM_API UGuideThreadSystemStateTreeComponentBase : public UQuestSystemStateTreeComponent
{
	GENERATED_BODY()

public:
	using FSchemaType = USTQuestChainComponentSchemaBase;

	virtual TSubclassOf<UStateTreeSchema> GetSchema() const override;

	virtual bool SetContextRequirements(
		FStateTreeExecutionContext& Context,
		bool bLogErrors = false
		) override;
};
#pragma endregion

#pragma region 主线
/**
 *	主线任务
 */
UCLASS(BlueprintType, Blueprintable)
class QUESTSYSTEM_API AQuestChain_MainBase : public AQuestChainBase,
                                     public IGuideThreadInterfacetion
{
	GENERATED_BODY()

public:
	AQuestChain_MainBase(
		const FObjectInitializer& ObjectInitializer
		);

	virtual EQuestChainType GetGuideThreadType() const override final;
};
#pragma endregion

#pragma region 支线
/**
 *	支线任务
 */
UCLASS(BlueprintType, Blueprintable)
class QUESTSYSTEM_API AQuestChain_BranchBase : public AQuestChainBase,
                                       public IGuideThreadInterfacetion
{
	GENERATED_BODY()

public:
	virtual EQuestChainType GetGuideThreadType() const override final;
};
#pragma endregion

#pragma region 即时任务
/**
 *	即时事件，如即时挑战
 */
UCLASS(BlueprintType, Blueprintable)
class QUESTSYSTEM_API AQuestChain_ImmediateBase : public AQuestChainBase
{
	GENERATED_BODY()

public:
};

#pragma endregion

#pragma region 区域事件
/**
 *	区域事件，进入区域后发生的引导,如世界首领、副本
 */
UCLASS(BlueprintType, Blueprintable)
class QUESTSYSTEM_API AQuestChain_AreaBase : public AQuestChain_ImmediateBase,
                                     public IGuideThreadInterfacetion
{
	GENERATED_BODY()

public:
	AQuestChain_AreaBase(
		const FObjectInitializer& ObjectInitializer
		);

	virtual EQuestChainType GetGuideThreadType() const override final;
};
#pragma endregion
