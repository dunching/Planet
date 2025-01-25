// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"

#include "AIComponent.generated.h"

class USceneComponent;
class ACharacterBase;
class UPAD_TaskNode;
class UPAD_TaskNode_Preset;
class UTaskNode_Temporary;
class AHumanCharacter_AI;

/**
 *
 */
UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class PLANET_API UAIComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	using FOwnerType = AHumanCharacter_AI;

	static FName ComponentName;

	UAIComponent(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	void AddTemporaryTaskNode(UTaskNode_Temporary*TaskNodePtr);

	void InitialAllocationsRowName();

	void InitialAllocationsByProxy();

	// 预置的任务，如自动播放的对话
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSoftObjectPtr<UPAD_TaskNode_Preset>> PresetTaskNodesAry;

	// 临时的任务，需要立即执行，比如多人对话时 A结束 B马上开始下一句，下一句则为临时任务被添加到此
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<UTaskNode_Temporary*> TemporaryTaskNodesAry;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> PathFollowComponentPtr = nullptr;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Pawn")
	FGameplayTag AI_Allocation_RowName = FGameplayTag::EmptyTag;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Pawn")
	FGameplayTag AI_CharacterType = FGameplayTag::EmptyTag;

};
