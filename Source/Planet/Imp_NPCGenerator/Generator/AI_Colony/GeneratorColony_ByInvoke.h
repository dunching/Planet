// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PlanetGenerator.h"

#include "GeneratorColony_ByInvoke.generated.h"

class UPlanetChildActorComponent;
class USplineComponent;
class ACharacterBase;

/**
 * 一组NPC群体
 * 延迟/按时间生成，
 */
UCLASS()
class PLANET_API AGeneratorColony_ByInvoke : public APlanetGenerator
{
	GENERATED_BODY()

public:

	AGeneratorColony_ByInvoke(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	
	virtual void SpawnGeneratorActor() override;

};
