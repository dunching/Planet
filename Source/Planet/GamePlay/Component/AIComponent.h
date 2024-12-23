// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"

#include "AIComponent.generated.h"

class USceneComponent;

/**
 *
 */
UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class PLANET_API UNPCComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	virtual void BeginPlay()override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> PathFollowComponentPtr = nullptr;

};

UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class PLANET_API UAIComponent : public UNPCComponent
{
	GENERATED_BODY()

public:

};
