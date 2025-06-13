// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"

#include "PropertyEntrys.generated.h"

USTRUCT(BlueprintType)
struct PROPERTYENTRYSYSTEM_API FGeneratedPropertyEntryInfo
{
	GENERATED_USTRUCT_BODY()

	 bool NetSerialize(
		FArchive& Ar,
		class UPackageMap* Map,
		bool& bOutSuccess
		) ;

	/**
	 * 
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTag PropertyTag;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Value = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Percent = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Level = 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsValue = true;
};

USTRUCT(BlueprintType)
struct PROPERTYENTRYSYSTEM_API FGeneratiblePropertyEntryInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 MinValue = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 MaxValue = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Weight = 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsValue = true;
};

/**
 * 
 */
USTRUCT(BlueprintType)
struct PROPERTYENTRYSYSTEM_API FTableRowProxy_GeneratiblePropertyEntrys : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString ExtendText;

	/**
	 * Key 词条的级数，> 0 ，越大提供的效果越高，可以为负数，意味着时负收益的词条
	 * Value 可生成词条的具体信息
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Addtional Element")
	TMap<int32, FGeneratiblePropertyEntryInfo> Map;
};