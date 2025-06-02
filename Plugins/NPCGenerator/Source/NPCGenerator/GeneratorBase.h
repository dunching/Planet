// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GeneratorBase.generated.h"

class UChildActorComponentBaseBase;

/**
 *
 */
UCLASS()
class NPCGENERATOR_API AGeneratorBase : public AActor
{
	GENERATED_BODY()

public:

	friend UChildActorComponentBaseBase;
	
	AGeneratorBase(const FObjectInitializer& ObjectInitializer);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	UPROPERTY(Replicated)
	FGuid GeneratorGuid;
	
protected:

	/**
	 * 仅服务器
	 */
	virtual void SpawnGeneratorActor();

	virtual void CustomizerFunc(AActor*TargetActorPtr);
	
private:
	
};
