// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"

#include "SceneActorInteractionComponent.generated.h"

class USceneComponent;
class ACharacterBase;
class AHumanCharacter_AI;
class UPAD_TaskNode;
class UPAD_TaskNode_Preset;
class UTaskNode_Temporary;
class UPAD_TaskNode_Interaction;
class AGuideInteraction_Actor;
class UTaskPromt;

USTRUCT(BlueprintType, Blueprintable)
struct PLANET_API FGuideInterationSetting
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AGuideInteraction_Actor> GuideInteraction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsEnable = true;

	/**
	 * 是否是可接取的任务
	 * 通常表现为NPC头上的【！】
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsTask = false;

	UPROPERTY(EditAnywhere, Category = Parameter)
	TSubclassOf<UTaskPromt> TaskPromtClass;

};

/**
 * 场景里的Actor拥有互动功能的组件
 */
UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class PLANET_API USceneActorInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	static FName ComponentName;

	USceneActorInteractionComponent(
		const FObjectInitializer& ObjectInitializer
	);

	TArray<TSubclassOf<AGuideInteraction_Actor>> GetInteractionLists() const;

	TObjectPtr<AGuideInteraction_Actor> GetCurrentInteraction() const;
	
	TArray<FGuideInterationSetting> GetGuideInteractionAry() const;

	virtual void StartInteractionItem(
		const TSubclassOf<AGuideInteraction_Actor>& Item
	);

	void StopInteractionItem();

	virtual void ChangedInterationState(
		const TSubclassOf<AGuideInteraction_Actor>& Item,
		bool bIsEnable
	);

	bool GetIsEnableInteraction()const;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Range = 200;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FGuideInterationSetting> GuideInteractionAry;

	TObjectPtr<AGuideInteraction_Actor> GuideInteractionActorPtr = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnableInteraction = true;

};
