// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "BrainComponent.h"
#include "GameplayTagContainer.h"
#include "GuideActor.h"

#include "GuideThreadActor.generated.h"


class UGameplayTasksComponent;

class UGuideSystemStateTreeComponent;
class UPAD_TaskNode_Guide;
class ACharacterBase;
class AHumanCharacter;
class AHumanCharacter_Player;

using FOnCurrentTaskNodeChanged = TMulticastDelegate<void(const TSoftObjectPtr<UPAD_TaskNode_Guide>&)>;

using FOnGuideInteractionEnd = TMulticastDelegate<void()>;

struct FTaskNodeResuleHelper
{
	// 任务ID
	FGuid TaskId;

	// 执行结果 (-1表示无效)
	int32 Output_1 = -1;
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

	// 添加任务执行结果
	void AddEvent(const FTaskNodeResuleHelper& TaskNodeResuleHelper);

	FTaskNodeResuleHelper ConsumeEvent(const FGuid& InGuid);
	
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
	TMap<FGuid, FTaskNodeResuleHelper> EventsSet;
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
