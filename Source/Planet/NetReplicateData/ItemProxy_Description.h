// Zowee. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include <GenerateType.h>

#include "GameplayTagContainer.h"
#include "ItemProxy_Minimal.h"

#include "ItemProxy_Description.generated.h"

USTRUCT(BlueprintType)
struct PLANET_API FPerLevelValue : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

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
	 * 
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<FString> DecriptionText;

	/**
	 * 
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<FString, FPerLevelValue> Values;
};
