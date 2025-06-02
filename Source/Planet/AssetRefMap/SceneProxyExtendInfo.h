// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include <GenerateTypes.h>

#include "GameplayTagContainer.h"

#include "ItemProxy_Minimal.h"
#include "SceneProxyTable.h"
#include "TalentInfo.h"
#include "CharactersInfo.h"
#include "ItemProxyCollection.h"

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
struct FTableRow_Regions;

PLANET_API FTableRowProxy_CommonCooldownInfo* GetTableRowProxy_CommonCooldownInfo(
	const FGameplayTag& CommonCooldownTag
	);

/**
 * DataTables
 */
UCLASS(BlueprintType, Blueprintable)
class PLANET_API USceneProxyExtendInfoMap : public UItemProxyCollection
{
	GENERATED_BODY()

public:
	USceneProxyExtendInfoMap();

	virtual void PostCDOContruct() override;

	static USceneProxyExtendInfoMap* GetInstance();

	template <typename ItemProxy_DescriptionType>
	TObjectPtr<ItemProxy_DescriptionType> GetTableRowProxyDescription(
		FGameplayTag UnitType
		) const;

	FTableRowProxy_TagExtendInfo* GetTableRowProxy_TagExtendInfo(
		FGameplayTag UnitType
		) const;

	const UPAD_Talent_Property* GetTalent_Property(
		EPointPropertyType PointPropertyType
		) const;

	FTableRow_Regions* GetTableRow_Region(
		FGameplayTag UnitType
		) const;

	TArray<FTableRow_Regions*> GetTableRow_AllRegions() const;

	TArray<FTableRowProxy_CharacterGrowthAttribute*> GetTableRow_CharacterGrowthAttribute() const;

	void InitialData();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UnitExtendInfoMap")
	TSoftObjectPtr<UDataTable> DataTable_TagExtendInfo;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UnitExtendInfoMap")
	TSoftObjectPtr<UDataTable> DataTable_CommonCooldownInfo;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UnitExtendInfoMap")
	TSoftObjectPtr<UDataTable> DataTable_PropertyEntrys;

	// Character的成长属性
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UnitExtendInfoMap")
	TSoftObjectPtr<UDataTable> DataTable_Character_GrowthAttribute;

	// 
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "DataLayer")
	TSoftObjectPtr<UDataTable> DataTable_Teleport;

	// 
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Regions")
	TSoftObjectPtr<UDataTable> DataTable_Regions;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Talent")
	TMap<EPointPropertyType, UPAD_Talent_Property*> PAD_Talent_PropertyMap;
};

template <typename ItemProxy_DescriptionType>
TObjectPtr<ItemProxy_DescriptionType> USceneProxyExtendInfoMap::GetTableRowProxyDescription(
	FGameplayTag UnitType
	) const
{
	auto TableRowPtr = GetTableRowProxy(UnitType);
	auto ItemProxy_Description_SkillPtr = Cast<ItemProxy_DescriptionType>(
	                                                                      TableRowPtr->ItemProxy_Description.
	                                                                      LoadSynchronous()
	                                                                     );
	return ItemProxy_Description_SkillPtr;
}

UCLASS(BlueprintType, Blueprintable)
class PLANET_API USceneUnitExtendInfoMap : public USceneProxyExtendInfoMap
{
	GENERATED_BODY()

public:
};
