// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/MovementComponent.h"
#include <Components/ChildActorComponent.h>

#include "ChildActorComponentBase.generated.h"

class AGeneratorBase;

/**
 *
 */
UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class NPCGENERATOR_API UChildActorComponentBaseBase : public UChildActorComponent
{
	GENERATED_BODY()

public:
	using FOwnerType = AGeneratorBase;

	static FName ComponentName;

	UChildActorComponentBaseBase(
		const FObjectInitializer& ObjectInitializer
	);

	virtual void RespawnChildActor();
};
