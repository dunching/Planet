// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GeneratorBase.h"

#include "GeneratorColony_ByTime.generated.h"

class UPlanetChildActorComponent;
class USplineComponent;
class ACharacterBase;

/**
 * 一组NPC群体
 * 根据时间刷新
 */
UCLASS()
class PLANET_API AGeneratorColony : public AGeneratorBase
{
	GENERATED_BODY()

public:

	AGeneratorColony(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	
	/**
	 * 敌人全部死亡后，刷新的间隔
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Generator")
	int32 FreshInteraval = 10;
	
};

