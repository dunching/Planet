// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GeneratorBase.generated.h"

class AGroupManagger;
class UPlanetChildActorComponent;

/**
 *
 */
UCLASS()
class PLANET_API AGeneratorBase : public AActor
{
	GENERATED_BODY()

public:

	AGeneratorBase(const FObjectInitializer& ObjectInitializer);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	void SpawnGeneratorActor();

	bool bDirctSpawnChild = false;
	
	UPROPERTY(Replicated)
	FGuid GeneratorGuid;
	
	TObjectPtr<AGroupManagger>GroupManaggerPtr = nullptr;
	
protected:

};
