// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "TeamMates_GenericType.h"
#include "GeneratorBase.h"

#include "PlanetGenerator.generated.h"

class AGroupManagger;
class AGroupManagger_NPC;
class UPlanetChildActorComponent;

/**
 *
 */
UCLASS()
class PLANET_API APlanetGenerator : public AGeneratorBase
{
	GENERATED_BODY()

public:
	friend UPlanetChildActorComponent;

	APlanetGenerator(
		const FObjectInitializer& ObjectInitializer
		);

	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps
		) const override;

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

protected:
	/**
	 * 仅服务器
	 */
	virtual void SpawnGeneratorActor() override;

	virtual void CustomizerFunc(
		AActor* TargetActorPtr
		) override;

	virtual void CustomizerGroupManagger(
		AGroupManagger_NPC* TargetActorPtr
		);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETeammateOption DefaultTeammateOption = ETeammateOption::kEnemy;

private:
	TObjectPtr<AGroupManagger_NPC> GroupManaggerPtr = nullptr;
};
