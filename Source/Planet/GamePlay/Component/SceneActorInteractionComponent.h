// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"
#include "Components/StateTreeComponent.h"

#include "TaskNode.h"

#include "SceneActorInteractionComponent.generated.h"

class USceneComponent;
class ACharacterBase;
class UPAD_TaskNode;
class UPAD_TaskNode_Preset;
class UTaskNode_Temporary;
class UPAD_TaskNode_Interaction;
class AGuideInteractionActor;

/**
 *
 */
UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class PLANET_API USceneActorInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	static FName ComponentName;

	USceneActorInteractionComponent(const FObjectInitializer& ObjectInitializer);

	TArray<TSubclassOf<AGuideInteractionActor>> GetTaskNodes() const;

	void AddGuideActor(const TSubclassOf<AGuideInteractionActor>& GuideActorClass);

	void RemoveGuideActor(const TSubclassOf<AGuideInteractionActor>& GuideActorClass);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Range = 200;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<AGuideInteractionActor>> GuideInteractionAry;

	TArray<TSubclassOf<AGuideInteractionActor>> TemporaryGuideInteractionAry;
};
