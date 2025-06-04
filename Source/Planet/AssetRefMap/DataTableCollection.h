// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include <GenerateTypes.h>

#include "GameplayTagContainer.h"

#include "ItemProxy_Minimal.h"
#include "SceneProxyTable.h"
#include "TalentInfo.h"
#include "CharactersInfo.h"
#include "PAD_ItemProxyCollection.h"

#include "DataTableCollection.generated.h"

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
class PLANET_API UDataTableCollection : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UDataTableCollection();

	virtual void PostCDOContruct() override;

	static UDataTableCollection* GetInstance();

	template <typename ItemProxy_DescriptionType>
	const TObjectPtr<ItemProxy_DescriptionType> GetTableRowProxyDescription(
		FGameplayTag UnitType
		) const;

	const FTableRowProxy_TagExtendInfo* GetTableRowProxy_TagExtendInfo(
		FGameplayTag UnitType
		) const;

	const FTableRow_Regions* GetTableRow_Region(
		FGameplayTag UnitType
		) const;

	const FTableRow_TalenSocket* GetTableRow_TalenSocket(
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
	TSoftObjectPtr<UDataTable> TableRow_TalenSocket;
};

template <typename ItemProxy_DescriptionType>
const TObjectPtr<ItemProxy_DescriptionType> UDataTableCollection::GetTableRowProxyDescription(
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
class PLANET_API USceneUnitExtendInfoMap : public UDataTableCollection
{
	GENERATED_BODY()

public:
};
