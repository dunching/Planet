
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

	UGameplayTasksComponent*GetGameplayTasksComponent()const;
	
	UGuideSystemStateTreeComponent*GetGuideSystemStateTreeComponent()const;
	
	// 任务节点类型：支线/支线
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag TaskNodeCategory;
	
	// 任务名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString TaskName;
	
	// 任务描述
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Description;
	
	UPROPERTY(Transient)
	TSoftObjectPtr<UPAD_TaskNode_Guide> TaskNodeRef;

	void UpdateCurrentTaskNode(const TSoftObjectPtr<UPAD_TaskNode_Guide>& InTaskNode);

	FOnCurrentTaskNodeChanged OnCurrentTaskNodeChanged;
	
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WolrdProcess)
	TObjectPtr<UGuideSystemStateTreeComponent> GuideStateTreeComponentPtr = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WolrdProcess)
	TObjectPtr<UGameplayTasksComponent>GameplayTasksComponentPtr = nullptr;
};


/**
 *	与NPC对话时的系列任务
 */
UCLASS(BlueprintType, Blueprintable)
class PLANET_API AGuideInteractionActor : public AGuideActor
{
	GENERATED_BODY()

public:
	
	AGuideInteractionActor(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Content)
	AHumanCharacter_Player* PlayerCharacter = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Content)
	ACharacterBase* TargetCharacter = nullptr;

	FOnGuideInteractionEnd OnGuideInteractionEnd;
	
};
