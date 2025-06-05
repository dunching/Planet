// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/MovementComponent.h"
#include <Components/ChildActorComponent.h>

#include "ChildActorComponentBase.h"
#include "PlanetChildActorComponent.generated.h"

class APlanetGenerator;

/**
 *
 */
UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class PLANET_API UPlanetChildActorComponent : public UChildActorComponentBaseBase
{
	GENERATED_BODY()

public:
	using FOwnerType = APlanetGenerator;

	UPlanetChildActorComponent(
		const FObjectInitializer& ObjectInitializer
	);

	virtual void CreateChildActor(
		TFunction<void(
			AActor*
		)> CustomizerFunc = nullptr
	) override;

	virtual void RespawnChildActor();
};
