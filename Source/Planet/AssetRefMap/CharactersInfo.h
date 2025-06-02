// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include <GenerateTypes.h>

#include "CharacterAttibutes.h"
#include "ItemProxy_Description.h"

#include "CharactersInfo.generated.h"

class AHumanCharacter_AI;

/**
 * 角色成长属性变化
 */
USTRUCT(BlueprintType)
struct PLANET_API FTableRowProxy_CharacterGrowthAttribute : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	
	FTableRowProxy_CharacterGrowthAttribute();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 LevelExperience = 100;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Max_HP = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Max_Stamina = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Max_Mana = 100;
};