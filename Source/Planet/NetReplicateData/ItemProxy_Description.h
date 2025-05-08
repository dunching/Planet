// Zowee. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include <GenerateType.h>

#include "GameplayTagContainer.h"
#include "ItemProxy_Minimal.h"

#include "ItemProxy_Description.generated.h"

USTRUCT(BlueprintType)
struct PLANET_API FPerLevelValue_Float : public FTableRowBase
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
struct PLANET_API FPerLevelValue_Int : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	FPerLevelValue_Int();

	FPerLevelValue_Int(
		std::initializer_list<int32> InitList
		);

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<int32> PerLevelValue;
};

/**
 * ItemProxy的数值
 * 例如药剂类的数据，持续多长时间，每隔几秒回复多少血量
 */
UCLASS()
class PLANET_API UItemProxy_Description : public UDataAsset
{
	GENERATED_BODY()

public:
	/**
	 * 简要说明
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FString Summary;

	/**
	 * 详细说明
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<FString> DecriptionText;

	/**
	 * 数值
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<FString, FPerLevelValue_Float> Values;

	/**
	 * 作为物品时的显示图片
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSoftObjectPtr<UTexture2D> DefaultIcon;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FString ProxyName = TEXT("ProxyName");

	/**
	 * 立绘，
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSoftObjectPtr<UTexture2D> RaffleIcon;
};
