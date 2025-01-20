// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "BrainComponent.h"
#include "GameplayTagContainer.h"

#include "GuideActor.generated.h"


class UGameplayTasksComponent;

class UGuideSystemStateTreeComponent;
class UPAD_TaskNode_Guide;
class ACharacterBase;
class AHumanCharacter;
class AHumanCharacter_Player;

using FOnCurrentTaskNodeChanged = TMulticastDelegate<void(const TSoftObjectPtr<UPAD_TaskNode_Guide>&)>;

using FOnGuideInteractionEnd = TMulticastDelegate<void()>;

/**
 *	引导,执行链条或拓扑结构的系列任务
 *	包含：主线、支线、新手引导之类
 */
UCLASS(BlueprintType, Blueprintable)
class PLANET_API AGuideActor : public AInfo
{
	GENERATED_BODY()

public:
	AGuideActor(const FObjectInitializer& ObjectInitializer);

	UGameplayTasksComponent* GetGameplayTasksComponent() const;

	UGuideSystemStateTreeComponent* GetGuideSystemStateTreeComponent() const;

	FOnCurrentTaskNodeChanged OnCurrentTaskNodeChanged;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WolrdProcess)
	TObjectPtr<UGuideSystemStateTreeComponent> GuideStateTreeComponentPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WolrdProcess)
	TObjectPtr<UGameplayTasksComponent> GameplayTasksComponentPtr = nullptr;
};

/**
 *	任务
 */
UCLASS(BlueprintType, Blueprintable)
class PLANET_API AGuideThread : public AGuideActor
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient)
	TSoftObjectPtr<UPAD_TaskNode_Guide> TaskNodeRef;

	void UpdateCurrentTaskNode(const TSoftObjectPtr<UPAD_TaskNode_Guide>& InTaskNode);

	void AddEvent(const FGuid& InGuid);

	bool ConsumeEvent(const FGuid& InGuid);
	
	// 任务节点类型：支线/支线
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag TaskNodeCategory;

	// 任务名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString TaskName;

	// 任务描述
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Description;

protected:
	TSet<FGuid> EventsSet;
};

/**
 *	主线任务
 */
UCLASS(BlueprintType, Blueprintable)
class PLANET_API AGuideMainThread : public AGuideThread
{
	GENERATED_BODY()

public:
};

/**
 *	支线线任务
 */
UCLASS(BlueprintType, Blueprintable)
class PLANET_API AGuideBranchThread : public AGuideThread
{
	GENERATED_BODY()

public:
};

/**
 *	与NPC对话时的系列任务
 */
UCLASS(BlueprintType, Blueprintable)
class PLANET_API AGuideInteractionActor : public AGuideMainThread
{
	GENERATED_BODY()

public:
	AGuideInteractionActor(const FObjectInitializer& ObjectInitializer);

	// 玩家Character
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Content)
	AHumanCharacter_Player* PlayerCharacter = nullptr;

	// 这个引导所属的Character
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Content)
	AHumanCharacter* TargetCharacter = nullptr;

	FOnGuideInteractionEnd OnGuideInteractionEnd;
};
