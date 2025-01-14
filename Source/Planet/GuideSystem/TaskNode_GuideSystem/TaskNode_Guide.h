// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"

#include "TaskNode.h"

#include "TaskNode_Guide.generated.h"

enum class EStateTreeRunStatus : uint8;

class ATargetPoint;
class AHumanCharacter_Player;

/*
 * 描述任务内容
 * 比如要求玩家按下某些按键、去往某些地方、采集某些资源之类
 */
UCLASS(Blueprintable, BlueprintType)
class PLANET_API UPAD_TaskNode_Guide : public UPAD_TaskNode_Preset
{
	GENERATED_BODY()

public:

	virtual FString GetDescription() const;

	// 当任务完成时玩家会得到的物品
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FGameplayTag, int32> GetItemWhenComplete;

	// 执行此节点时，是否刷新上一条的描述？
	// 在执行某些H节点时，是不需要刷新的
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsFreshPreviouDescription = true;
	
};

/*
 * 引导玩家去往指定地点
 */
UCLASS(Blueprintable, BlueprintType)
class PLANET_API UPAD_TaskNode_Guide_MoveToLocation : public UPAD_TaskNode_Guide
{
	GENERATED_BODY()

public:
	UPAD_TaskNode_Guide_MoveToLocation(const FObjectInitializer& ObjectInitializer);

#if WITH_EDITOR
	virtual bool Modify(bool bAlwaysMarkDirty = true) override;

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
	virtual void PostLoad()override;
	
	virtual FString GetDescription() const override;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FVector TargetLocation = FVector::ZeroVector;

	// 小于此半径就判定任务完成
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 ReachedRadius = 100;

protected:

	void SetTargetPoint();	
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSoftObjectPtr<ATargetPoint> TargetPointPtr = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FString Description;
};

/*
 * 引导玩家按下某个按键
 */
UCLASS(Blueprintable, BlueprintType)
class PLANET_API UPAD_TaskNode_Guide_PressKey : public UPAD_TaskNode_Guide
{
	GENERATED_BODY()

public:
	UPAD_TaskNode_Guide_PressKey(const FObjectInitializer& ObjectInitializer);

	virtual FString GetDescription() const override;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FKey Key = EKeys::AnyKey;

};

/*
 * 玩家独白
 */
UCLASS(Blueprintable, BlueprintType)
class PLANET_API UPAD_TaskNode_Guide_Monologue : public UPAD_TaskNode_Guide
{
	GENERATED_BODY()

public:
	UPAD_TaskNode_Guide_Monologue(const FObjectInitializer& ObjectInitializer);

	virtual FString GetDescription() const override;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<FTaskNode_Conversation_SentenceInfo> ConversationsAry;
	
};

/*
 * 向目标添加可交互内容
 * 比如NPC原本只有固定对话内容，添加之后可增加新的内容，如接取支线、交付结果等
 */
UCLASS(Blueprintable, BlueprintType)
class PLANET_API UPAD_TaskNode_Guide_AddToTarget : public UPAD_TaskNode_Guide
{
	GENERATED_BODY()

public:
	UPAD_TaskNode_Guide_AddToTarget(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSoftObjectPtr<AHumanCharacter_AI>TargetCharacterPtr = nullptr;
	
};

/*
 * 引导玩家与目标对话
 */
UCLASS(Blueprintable, BlueprintType)
class PLANET_API UPAD_TaskNode_Guide_ConversationWithTarget : public UPAD_TaskNode_Guide
{
	GENERATED_BODY()

public:
	UPAD_TaskNode_Guide_ConversationWithTarget(const FObjectInitializer& ObjectInitializer);

	virtual FString GetDescription() const override;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSoftObjectPtr<AHumanCharacter_AI>TargetCharacterPtr = nullptr;
	
};
