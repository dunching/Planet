// Zowee. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include <GenerateType.h>

#include "GameplayTagContainer.h"

#include "ItemProxy_Minimal.h"
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
struct FTableRowUnit_CharacterGrowthAttribute;
struct FTableRowUnit_AICharacter_Allocation;

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

	// 可被持有的“物品”基础信息
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UnitExtendInfoMap")
	TSoftObjectPtr<UDataTable> DataTable_Unit;

	// 可被持有的“武器”
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UnitExtendInfoMap")
	TSoftObjectPtr<UDataTable> DataTable_Unit_WeaponExtendInfo;

	// 可被持有的“主动技能”
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UnitExtendInfoMap")
	TSoftObjectPtr<UDataTable> DataTable_Unit_ActiveSkillExtendInfo;

	// 可被持有的“被动技能”
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UnitExtendInfoMap")
	TSoftObjectPtr<UDataTable> DataTable_Unit_PassiveSkillExtendInfo;

	// 可被持有的“武器技能”
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UnitExtendInfoMap")
	TSoftObjectPtr<UDataTable> DataTable_Unit_WeaponSkillExtendInfo;

	// 可被持有的“消耗品”
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UnitExtendInfoMap")
	TSoftObjectPtr<UDataTable> DataTable_Unit_Consumable;
	
	// 可被持有的“队友/角色”
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UnitExtendInfoMap")
	TSoftObjectPtr<UDataTable> DataTable_Unit_CharacterInfo;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UnitExtendInfoMap")
	TSoftObjectPtr<UDataTable> DataTable_TagExtendInfo;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UnitExtendInfoMap")
	TSoftObjectPtr<UDataTable> DataTable_CommonCooldownInfo;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UnitExtendInfoMap")
	TSoftObjectPtr<UDataTable> DataTable_PropertyEntrys;

	// AICharacter的技能配分
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UnitExtendInfoMap")
	TSoftObjectPtr<UDataTable> DataTable_Character_Allocation;
	
	// Character的成长属性
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UnitExtendInfoMap")
	TSoftObjectPtr<UDataTable> DataTable_Character_GrowthAttribute;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Talent")
	TMap<EPointPropertyType, UPAD_Talent_Property*> PAD_Talent_PropertyMap;
	
};
