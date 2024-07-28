// Zowee. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "SceneElement.h"

#include "SceneUnitTable.generated.h"

class UBasicUnit;

USTRUCT(BlueprintType)
struct PLANET_API FSceneUnitExtendInfoBase : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	FSceneUnitExtendInfoBase() { }

	virtual ~FSceneUnitExtendInfoBase() { }

	// 用于复制到RowName
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FGuid UnitGuid = FGuid::NewGuid();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName RowDescriptionText = TEXT("Row描述文字");

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	ESceneToolsType SceneToolsType = ESceneToolsType::kCoin;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	EToolUnitType ToolUnitType = EToolUnitType::kNone;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	EWeaponUnitType WeaponUnitType = EWeaponUnitType::kNone;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	EConsumableUnitType ConsumableUnitType = EConsumableUnitType::kNone;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	ESkillUnitType SkillUnitType = ESkillUnitType::kNone;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	ECoinUnitType CoinUnitType = ECoinUnitType::kNone;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSoftObjectPtr<UTexture2D> RaffleIcon;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<UBasicUnit>UnitClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName DescriptionText = TEXT("描述文字");

};
