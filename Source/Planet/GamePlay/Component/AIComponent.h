// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"

#include "AIComponent.generated.h"

class USceneComponent;
class ACharacterBase;
class UPAD_TaskNode;
class UPAD_TaskNode_Preset;
class UTaskNode_Temporary;

/**
 *
 */
UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class PLANET_API UAIComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	static FName ComponentName;

	UAIComponent(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	void AddTemporaryTaskNode(UTaskNode_Temporary*TaskNodePtr);


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UPAD_TaskNode_Preset*> PresetTaskNodesAry;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<UTaskNode_Temporary*> TemporaryTaskNodesAry;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> PathFollowComponentPtr = nullptr;
};
