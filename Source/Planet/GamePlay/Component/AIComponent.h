// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"

#include "AIComponent.generated.h"

class USceneComponent;
class ACharacterBase;
class UPAD_TaskNode;

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

	virtual void BeginPlay()override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> PathFollowComponentPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UPAD_TaskNode*> TaskNodesAry;
	
};
