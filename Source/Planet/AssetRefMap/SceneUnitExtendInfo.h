// Zowee. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include <GenerateType.h>

#include "GameplayTagContainer.h"
#include "SceneElement.h"

#include "SceneUnitExtendInfo.generated.h"

class UToolUnit;
class UWeaponUnit;
class USkillUnit;
class UCoinUnit;

USTRUCT(BlueprintType)
struct PLANET_API FSceneUnitExtendInfoBase
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Code")
	FString Code;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Code")
	FGuid Guid = FGuid::NewGuid();
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ToolsIcons")
	TSoftObjectPtr<UTexture2D> DefaultIcon;

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
	TMap<ESkillUnitType, FSceneUnitExtendInfoBase>SkillUnitMap;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UnitExtendInfoMap")
	TMap<EToolUnitType, FSceneUnitExtendInfoBase>ToolUnitMap;
	
#pragma region SceneTools
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "SceneTools")
	TMap<EToolUnitType, TSubclassOf<UToolUnit>>EquipmentToolsMap;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "SceneTools")
	TMap<EWeaponUnitType, TSubclassOf<UWeaponUnit>>WeaponToolsMap;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "SceneTools")
	TMap<ESkillUnitType, TSubclassOf<USkillUnit>>SkillToolsMap;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "SceneTools")
	TMap<EConsumableUnitType, TSubclassOf<UConsumableUnit>>ConsumableToolMap;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "SceneTools")
	TMap<ECoinUnitType, TSubclassOf<UCoinUnit>>CoinToolMap;
#pragma endregion 

};
