// Zowee. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include <GenerateType.h>

#include "GameplayTagContainer.h"

#include "ItemProxy_Minimal.h"
#include "SceneProxyTable.h"
#include "TalentInfo.h"

#include "SceneProxyExtendInfo.generated.h"

class UDataTable;
class UPAD_Talent_Property;

struct FToolProxy;
struct FWeaponProxy;
struct FSkillProxy;
struct FCoinProxy;
struct FBasicProxy;

struct FTableRowProxy_TagExtendInfo;
struct FTableRowProxy_CharacterGrowthAttribute;
struct FTableRowProxy_AICharacter_Allocation;

UCLASS(BlueprintType, Blueprintable)
class PLANET_API USceneProxyExtendInfoMap : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:

	USceneProxyExtendInfoMap();

	virtual void PostCDOContruct() override;

	static USceneProxyExtendInfoMap* GetInstance();

	FTableRowProxy* GetTableRowProxy(FGameplayTag UnitType)const;
	
	FTableRowProxy_TagExtendInfo* GetTableRowProxy_TagExtendInfo(FGameplayTag UnitType)const;
	
	FTableRowProxy_AICharacter_Allocation* GetTableRowProxy_AICharacter_Allocation(FGameplayTag UnitType)const;

	const UPAD_Talent_Property* GetTalent_Property(EPointPropertyType PointPropertyType)const;

	void InitialData();

	// 可被持有的“物品”基础信息
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UnitExtendInfoMap")
	TSoftObjectPtr<UDataTable> DataTable_Proxy;

	// 可被持有的“武器”
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UnitExtendInfoMap")
	TSoftObjectPtr<UDataTable> DataTable_Proxy_WeaponExtendInfo;

	// 可被持有的“主动技能”
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UnitExtendInfoMap")
	TSoftObjectPtr<UDataTable> DataTable_Proxy_ActiveSkillExtendInfo;

	// 可被持有的“被动技能”
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UnitExtendInfoMap")
	TSoftObjectPtr<UDataTable> DataTable_Proxy_PassiveSkillExtendInfo;

	// 可被持有的“武器技能”
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UnitExtendInfoMap")
	TSoftObjectPtr<UDataTable> DataTable_Proxy_WeaponSkillExtendInfo;

	// 可被持有的“消耗品”
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UnitExtendInfoMap")
	TSoftObjectPtr<UDataTable> DataTable_Proxy_Consumable;
	
	// 可被持有的“队友/角色”
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UnitExtendInfoMap")
	TSoftObjectPtr<UDataTable> DataTable_Proxy_CharacterInfo;
	
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
	
	// 
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "DataLayer")
	TSoftObjectPtr<UDataTable> DataTable_Teleport;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Talent")
	TMap<EPointPropertyType, UPAD_Talent_Property*> PAD_Talent_PropertyMap;
	
};

UCLASS(BlueprintType, Blueprintable)
class PLANET_API USceneUnitExtendInfoMap : public USceneProxyExtendInfoMap
{
	GENERATED_BODY()
public:

};
