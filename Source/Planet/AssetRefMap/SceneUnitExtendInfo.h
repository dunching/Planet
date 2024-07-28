// Zowee. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include <GenerateType.h>

#include "GameplayTagContainer.h"

#include "SceneElement.h"

#include "SceneUnitExtendInfo.generated.h"

class UDataTable;

class UToolUnit;
class UWeaponUnit;
class USkillUnit;
class UCoinUnit;
class UBasicUnit;

USTRUCT(BlueprintType)
struct PLANET_API FTableRowUnit : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	FTableRowUnit() { }

	virtual ~FTableRowUnit() { }

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

UCLASS(BlueprintType, Blueprintable)
class PLANET_API USceneUnitExtendInfoMap : public UObject
{
	GENERATED_BODY()
public:

	USceneUnitExtendInfoMap();

	virtual void PostCDOContruct() override;

	static USceneUnitExtendInfoMap* GetInstance();

	void InitialData();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UnitExtendInfoMap")
	TSoftObjectPtr<UDataTable> DataTable;

};
