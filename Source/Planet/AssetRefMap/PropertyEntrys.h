// Zowee. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include <GenerateType.h>

#include "GameplayTagContainer.h"
#include "ItemProxy.h"

#include "PropertyEntrys.generated.h"

USTRUCT(BlueprintType)
struct PLANET_API FTableRowUnit_PropertyEntrys : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString ExtendText;
	
	// 
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Addtional Element")
	TMap<ECharacterPropertyType, int32>Map;
	
};
