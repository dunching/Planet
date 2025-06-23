// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "ItemProxy_GenericType.generated.h"

enum class EProxyModifyType
{
	kAdd, // 新增
	kRemove, // 移除
	kPropertyChange, // 属性改变
};

UENUM()
enum class EItemProxyInteractionType : uint8
{
	kDiscard, // 丢弃
	kBreakDown, // 分解
	kUpgrade, // 升级
};

USTRUCT(BlueprintType)
struct ITEMPROXY_API FPerLevelValue_Float : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	FPerLevelValue_Float();

	FPerLevelValue_Float(
		std::initializer_list<float> InitList
		);

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<float> PerLevelValue;
};

USTRUCT(BlueprintType)
struct ITEMPROXY_API FPerLevelValue_Int : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	FPerLevelValue_Int();

	FPerLevelValue_Int(
		std::initializer_list<int32> InitList
		);

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<int32> PerLevelValue;
};
