// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "BrainComponent.h"
#include "GameplayTagContainer.h"
#include "Components/StateTreeComponentSchema.h"

#include "GuideActor.h"
#include "GuideSystemStateTreeComponent.h"

#include "GuideThreadActor.generated.h"


class UGameplayTasksComponent;

class UGuideSystemStateTreeComponent;
class UPAD_TaskNode_Guide;
class ACharacterBase;
class AHumanCharacter;
class AHumanCharacter_Player;

struct FTaskNodeDescript;

using FOnGuideInteractionEnd = TMulticastDelegate<void()>;

UCLASS()
class UStateTreeGuideThreadComponentSchema : public UStateTreeComponentSchema
{
	GENERATED_BODY()
public:
	virtual bool IsStructAllowed(const UScriptStruct* InScriptStruct) const override;

};

/**
 *
 */
UCLASS()
class PLANET_API UGuideThreadSystemStateTreeComponent : public UGuideSystemStateTreeComponent
{
	GENERATED_BODY()

public:
	virtual TSubclassOf<UStateTreeSchema>GetSchema() const override;
	
};

/**
 *	任务
 */
UCLASS(BlueprintType, Blueprintable)
class PLANET_API AGuideThread : public AGuideActor
{
	GENERATED_BODY()

public:
	AGuideThread(const FObjectInitializer& ObjectInitializer);

	void UpdateCurrentTaskNode(const TSoftObjectPtr<UPAD_TaskNode_Guide>& InTaskNode);

	void UpdateCurrentTaskNode(const FTaskNodeDescript& TaskNodeDescript);

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

	UPROPERTY(Transient)
	TSoftObjectPtr<UPAD_TaskNode_Guide> TaskNodeRef;

	FTaskNodeDescript CurrentTaskNodeDescript;
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
