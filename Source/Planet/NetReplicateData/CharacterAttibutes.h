// Zowee. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include <GenerateType.h>

#include "ItemProxy_Description.h"

#include "CharacterAttibutes.generated.h"

/**
 * 角色成长属性变化
 */
USTRUCT(BlueprintType)
struct PLANET_API FCharacterGrowthAttribute : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 LevelExperience = 100;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Max_HP = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Max_Stamina = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Max_Mana = 100;
};