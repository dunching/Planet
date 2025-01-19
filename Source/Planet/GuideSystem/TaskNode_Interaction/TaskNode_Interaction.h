// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"
#include "Components/StateTreeComponent.h"

#include "TaskNode.h"

#include "TaskNode_Interaction.generated.h"

class USceneComponent;
class ACharacterBase;
class UPAD_TaskNode;
class UPAD_TaskNode_Preset;
class UTaskNode_Temporary;

/*
 * 可交互的节点描述
 */
UCLASS(Blueprintable, BlueprintType)
class PLANET_API UPAD_TaskNode_Interaction : public UPAD_TaskNode_Preset
{
	GENERATED_BODY()

public:

	// 当任务完成时玩家会得到的物品
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FGameplayTag, int32> GetItemWhenComplete;

	// 执行此节点时，是否刷新上一条的描述？
	// 在执行某些H节点时，是不需要刷新的
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsFreshPreviouDescription = true;
	
};

/*
 * 对话
 */
UCLASS(Blueprintable, BlueprintType)
class PLANET_API UPAD_TaskNode_Interaction_Conversation : public UPAD_TaskNode_Interaction
{
	GENERATED_BODY()

public:

	UPAD_TaskNode_Interaction_Conversation(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<FTaskNode_Conversation_SentenceInfo> ConversationsAry;
	
};

/*
 * 选择
 */
UCLASS(Blueprintable, BlueprintType)
class PLANET_API UPAD_TaskNode_Interaction_Option : public UPAD_TaskNode_Interaction
{
	GENERATED_BODY()

public:

	UPAD_TaskNode_Interaction_Option(const FObjectInitializer& ObjectInitializer);

	// 
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<FString> OptionAry;
	
};
