// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/MovementComponent.h"
#include <Components/ChildActorComponent.h>

#include "PlanetChildActorComponent.generated.h"

/**
 *
 */
UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class PLANET_API UPlanetChildActorComponent : public UChildActorComponent
{
	GENERATED_BODY()

public:
	virtual void CreateChildActor(TFunction<void(AActor*)> CustomizerFunc = nullptr)override;
};
