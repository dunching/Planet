// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GenerateType.h"

#include "GeneratorBase.generated.h"

class AGroupManagger;
class AGroupManagger_NPC;
class UPlanetChildActorComponent;

/**
 *
 */
UCLASS()
class PLANET_API AGeneratorBase : public AActor
{
	GENERATED_BODY()

public:

	friend UPlanetChildActorComponent;
	
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
	
	virtual void CustomizerGroupManagger(AGroupManagger_NPC*TargetActorPtr);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETeammateOption DefaultTeammateOption = ETeammateOption::kEnemy;

private:
	
	TObjectPtr<AGroupManagger_NPC>GroupManaggerPtr = nullptr;
	
};
