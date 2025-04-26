// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "AbilitySystemInterface.h"
#include "GroupManagger.h"

#include "GameFramework/PlayerState.h"

#include "PlanetAbilitySystemComponent.h"
#include "ItemProxy_Minimal.h"

#include "GroupManagger_NPC.generated.h"

class AGeneratorNPCs_Patrol;
class ABuildingArea;

/*
 */
UCLASS()
class PLANET_API AGroupManagger_NPC :
	public AGroupManagger
{
	GENERATED_BODY()

public:
	
	/**
	 * 既定的巡逻路线
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<AGeneratorNPCs_Patrol> GeneratorNPCs_PatrolPtr = nullptr;

	/**
	 * 营寨
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<ABuildingArea> BuildingAreaPtr = nullptr;

};
