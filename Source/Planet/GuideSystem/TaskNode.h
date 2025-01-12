// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"

#include "TaskNode.generated.h"

class USceneComponent;
class ACharacterBase;
class AHumanCharacter_AI;

UENUM()
enum class ETaskNodeType : uint8
{
	kAutomatic_Conversatin,// 自动播放的对话
	
	kConversatin,
	
	kTemporary,
	kTemporary_Conversation,// 在对话中，让对方说的话
	
	kTest,

	kWorldProcess_MoveToPoint, //
	kWorldProcess_Conversation, //
	
	kNone,
};

UENUM()
enum class ETaskNodeState : uint8
{
	kWaitExcute,
	kRunning,
	kCompleted,
};

/*
 * 用来描述任务的数据
 * 预置的任务
 */
UCLASS()
class PLANET_API UPAD_TaskNode_Preset : public UDataAsset
{
	GENERATED_BODY()

public:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	ETaskNodeType TaskNodeType = ETaskNodeType::kNone;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	ETaskNodeState TaskNodeState = ETaskNodeState::kWaitExcute;
	
};
 
/*
 * 用来描述任务的数据
 * 临时的任务
 */
UCLASS()
class PLANET_API UTaskNode_Temporary : public UObject
{
	GENERATED_BODY()

public:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	ETaskNodeType TaskNodeType = ETaskNodeType::kNone;
	
};
 