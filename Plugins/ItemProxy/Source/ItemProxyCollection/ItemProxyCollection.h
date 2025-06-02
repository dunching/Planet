// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"

#include "ItemProxy_Descriptions.h"

#include "ItemProxyCollection.generated.h"

class UDataTable;
class UPAD_Talent_Property;
class UItemDecriptionBaseWidget;
class UItemProxy_Description;

struct FToolProxy;
struct FWeaponProxyBase;
struct FSkillProxy;
struct FCoinProxyBase;
struct FBasicProxy;

struct FTableRowProxy_TagExtendInfo;
struct FTableRowProxy_CharacterGrowthAttribute;
struct FTableRow_Regions;

USTRUCT(BlueprintType)
struct ITEMPROXY_API FTableRowProxy : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	/**
	 * 这个Item使用哪个数据
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSoftObjectPtr<UItemProxy_Description> ItemProxy_Description;

	/**
	 * 这个Item使用哪个Widget进行展示
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<UItemDecriptionBaseWidget> ItemDecriptionClass;
};

/**
 * DataTables
 */
UCLASS(BlueprintType, Blueprintable)
class ITEMPROXY_API UItemProxyCollection : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/**
	 * 在项目中添加对此函数的支持
	 * @return 
	 */
	static UItemProxyCollection* GetInstance();

	const FTableRowProxy* GetTableRowProxy(
		FGameplayTag UnitType
		) const;

private:
	/**
	 * 所有的“物品”基础信息
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	TMap<FGameplayTag, FTableRowProxy> ProxysMap;
};
