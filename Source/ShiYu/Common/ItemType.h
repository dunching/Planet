// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <variant>

#include "CoreMinimal.h"

#include "GenerateType.h"

#include "ItemType.generated.h"

USTRUCT(BlueprintType)
struct FItemType
{
	GENERATED_USTRUCT_BODY()

		FItemType();

	std::variant<
		EEquipmentType,
		ERawMaterialType,
		EBuildingType
	> ItemType;
	
		UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EEnumtype Enumtype = EEnumtype::kNone;

	int32 ID;

};

FORCEINLINE uint32 GetTypeHash(const FItemType& Key)
{
	switch (Key.Enumtype)
	{
	case EEnumtype::kEquipment:
	{
		return GetTypeHash(std::get<EEquipmentType>(Key.ItemType));
	}
	break;
	case EEnumtype::kBuilding:
	{
		return GetTypeHash(std::get<EBuildingType>(Key.ItemType));
	}
	break;
	case EEnumtype::kRawMaterialType:
	{
		return GetTypeHash(std::get<ERawMaterialType>(Key.ItemType));
	}
	break;
	default:
	{
		return GetTypeHash(std::get<ERawMaterialType>(Key.ItemType));
	}
	}
}

USTRUCT(BlueprintType)
struct FItemNum
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Val)
		FItemType ItemType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Val)
		int32 Num = 0; // uint32

};

bool CheckItemNumIsValid(const FItemNum& NewItemBase);

USTRUCT(BlueprintType)
struct FItemAry
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Val)
		TArray<FItemNum> ItemsAry;
};

USTRUCT(BlueprintType)
struct FCreatingMap
{
	GENERATED_USTRUCT_BODY()

		TMap<FItemType, TMap<FItemType, int32>>CreatingMap;
};

USTRUCT(BlueprintType)
struct FCanbeCreatinedSet
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Val)
		TSet<FItemType>CanbeCreatinedSet;
};

