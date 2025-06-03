// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PlanetGenerator.h"

#include "GeneratorColony_ByInvoke.generated.h"

class UPlanetChildActorComponent;
class USplineComponent;
class ACharacterBase;

UCLASS()
class PLANET_API UFormationComponent : public USceneComponent
{
	GENERATED_BODY()

public:

	UFormationComponent(const FObjectInitializer& ObjectInitializer);

};

/**
 * 一组按固定阵型行走的NPC群体
 */
UCLASS()
class PLANET_API AGeneratorColonyWithPath : public APlanetGenerator
{
	GENERATED_BODY()

public:

	AGeneratorColonyWithPath(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds)override;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TObjectPtr<USplineComponent> SplineComponentPtr = nullptr;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TObjectPtr<UFormationComponent> FormationComponentPtr = nullptr;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 ForwardSpeed = 250;
	
	UPROPERTY(EditAnywhere, Category = Parameter)
	int32 MaxDistance = 1000;

	int32 CurrentLength = 0;

};

/**
 * NPC会沿着这条样条线巡逻
 */
UCLASS()
class PLANET_API AGeneratorNPCs_Patrol : public APlanetGenerator
{
	GENERATED_BODY()

public:

	AGeneratorNPCs_Patrol(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	bool CheckIsFarawayOriginal(ACharacterBase*TargetCharacterPtr) const;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Spline")
	TObjectPtr<USplineComponent> SplineComponentPtr;
	
	UPROPERTY(EditAnywhere, Category = Parameter)
	int32 MaxDistance = 800;

protected:

	virtual void SpawnGeneratorActor() override;

	virtual void CustomizerGroupManagger(AGroupManagger_NPC*TargetActorPtr)override;
	
};

/////////////////////////////////////////////////////////////////

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
