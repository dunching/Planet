// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"

#include "ItemProxy_Descriptions.h"

#include "PAD_ItemProxyCollection.generated.h"

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

ITEMPROXY_API const FTableRowProxy* GetTableRowProxy(const FGameplayTag &ProxyType);
	
UCLASS(BlueprintType, Blueprintable)
class ITEMPROXY_API UPAD_ItemProxyCollection : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	static const UPAD_ItemProxyCollection* GetInstance();
	
	const FTableRowProxy* GetTableRowProxy(FGameplayTag UnitType)const;

	/**
	 * 可被持有的“物品”基础信息
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UnitExtendInfoMap")
	TSoftObjectPtr<UDataTable> DataTable_Proxy;
};

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UGetItemProxyCollectionInterface : public UInterface
{
	GENERATED_BODY()
};

class ITEMPROXY_API IGetItemProxyCollectionInterface
{
	GENERATED_BODY()

public:
	virtual const UPAD_ItemProxyCollection*GetItemProxyCollection()const = 0;
	
};
