// Zowee. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include <GenerateType.h>

#include "GameplayTagContainer.h"

#include "SceneElement.h"
#include "SceneUnitTable.h"
#include "TalentInfo.h"

#include "SceneUnitExtendInfo.generated.h"

class UDataTable;
class UPAD_Talent_Property;

struct FToolProxy;
struct FWeaponProxy;
struct FSkillProxy;
struct FCoinProxy;
struct FBasicProxy;

struct FTableRowUnit_TagExtendInfo;

UCLASS(BlueprintType, Blueprintable)
class PLANET_API USceneUnitExtendInfoMap : public UObject
{
	GENERATED_BODY()
public:

	USceneUnitExtendInfoMap();

	virtual void PostCDOContruct() override;

	static USceneUnitExtendInfoMap* GetInstance();

	FTableRowUnit* GetTableRowUnit(FGameplayTag UnitType)const;
	
	FTableRowUnit_TagExtendInfo* GetTableRowUnit_TagExtendInfo(FGameplayTag UnitType)const;
	
	FTableRowUnit_AICharacter_Allocation* GetTableRowUnit_AICharacter_Allocation(FGameplayTag UnitType)const;

	const UPAD_Talent_Property* GetTalent_Property(EPointPropertyType PointPropertyType)const;

	void InitialData();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UnitExtendInfoMap")
	TSoftObjectPtr<UDataTable> DataTable_Unit;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UnitExtendInfoMap")
	TSoftObjectPtr<UDataTable> DataTable_Unit_WeaponExtendInfo;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UnitExtendInfoMap")
	TSoftObjectPtr<UDataTable> DataTable_Unit_ActiveSkillExtendInfo;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UnitExtendInfoMap")
	TSoftObjectPtr<UDataTable> DataTable_Unit_PassiveSkillExtendInfo;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UnitExtendInfoMap")
	TSoftObjectPtr<UDataTable> DataTable_Unit_WeaponSkillExtendInfo;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UnitExtendInfoMap")
	TSoftObjectPtr<UDataTable> DataTable_Unit_CharacterInfo;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UnitExtendInfoMap")
	TSoftObjectPtr<UDataTable> DataTable_Unit_Consumable;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UnitExtendInfoMap")
	TSoftObjectPtr<UDataTable> DataTable_TagExtendInfo;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UnitExtendInfoMap")
	TSoftObjectPtr<UDataTable> DataTable_CommonCooldownInfo;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UnitExtendInfoMap")
	TSoftObjectPtr<UDataTable> DataTable_PropertyEntrys;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UnitExtendInfoMap")
	TSoftObjectPtr<UDataTable> DataTable_AICharacter_Allocation;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Talent")
	TMap<EPointPropertyType, UPAD_Talent_Property*> PAD_Talent_PropertyMap;
	
};
