// Zowee. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include <GenerateType.h>

#include "GameplayTagContainer.h"
#include "CharacterAttibutes.h"

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
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TArray<FCharacterAttributes> CharacterAttributesPerLevel;
};

USTRUCT(BlueprintType)
struct PLANET_API FTableRowProxy_CharacterType : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<AHumanCharacter_AI>CharacterClass;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FString Title;
};

// NPC分配的技能
USTRUCT(BlueprintType)
struct PLANET_API FTableRowProxy_AICharacter_Allocation : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FGameplayTag FirstWeaponSocketInfo;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FGameplayTag SecondWeaponSocketInfo;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FGameplayTag ActiveSkillSet_1;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FGameplayTag ActiveSkillSet_2;

};
