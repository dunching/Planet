// Zowee. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include <GenerateType.h>

#include "GameplayTagContainer.h"

#include "SceneElement.h"

#include "SceneUnitTable.generated.h"

class UDataTable;

class UToolUnit;
class UWeaponUnit;
class USkillUnit;
class UCoinUnit;
class UBasicUnit; 
class AWeapon_Base; 

USTRUCT(BlueprintType)
struct PLANET_API FTableRowUnit : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName RowDescriptionText = TEXT("Row描述文字");

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ToolsIcons")
	TSoftObjectPtr<UTexture2D> DefaultIcon;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ToolsIcons")
	FString UnitName = TEXT("UnitName");

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSoftObjectPtr<UTexture2D> RaffleIcon;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<UBasicUnit>UnitClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName DescriptionText = TEXT("描述文字");

};

USTRUCT(BlueprintType)
struct PLANET_API FTableRowUnit_WeaponExtendInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ToolClass")
	TSubclassOf<AWeapon_Base> ToolActorClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ToolClass")
	FGameplayTag WeaponSkillUnitType = FGameplayTag::EmptyTag;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Skill")
	EAnimLinkClassType AnimLinkClassType = EAnimLinkClassType::kUnarmed;
	
};


USTRUCT(BlueprintType)
struct PLANET_API FTableRowUnit_ActiveSkillExtendInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Require Weapon")
	FGameplayTag RequireWeaponUnitType = FGameplayTag::EmptyTag;

};
