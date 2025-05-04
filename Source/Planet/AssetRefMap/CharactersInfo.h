// Zowee. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include <GenerateType.h>

#include "GameplayTagContainer.h"
#include "ItemProxy_Description.h"

#include "CharactersInfo.generated.h"

class AHumanCharacter_AI;

USTRUCT(BlueprintType)
struct PLANET_API FTableRowProxy_CharacterGrowthAttribute : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	
	FTableRowProxy_CharacterGrowthAttribute();

// 	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
// 	FCharacterAttributes CharacterAttributes;

	// 每级增加的属性
	// UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	// TArray<FCharacterAttributes> CharacterAttributesPerLevel;
};