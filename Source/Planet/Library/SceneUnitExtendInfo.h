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

struct PLANET_API FSceneUnitExtendInfoBase
{
	FString Code;
	
	FGuid Guid = FGuid::NewGuid();
};

UCLASS(BlueprintType, Blueprintable)
class PLANET_API USceneUnitExtendInfoMap : public UObject
{
	GENERATED_BODY()
public:

	USceneUnitExtendInfoMap();

	static USceneUnitExtendInfoMap* GetInstance();
	
	TMap<ESkillUnitType, TSharedPtr<FSceneUnitExtendInfoBase>>SkillUnitMap;
	
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
