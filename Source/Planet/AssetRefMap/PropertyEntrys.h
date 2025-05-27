// Zowee. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include <GenerateTypes.h>

#include "GameplayTagContainer.h"
#include "ItemProxy_Minimal.h"

#include "PropertyEntrys.generated.h"

USTRUCT(BlueprintType)
struct PLANET_API FTableRowProxy_PropertyEntrys : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString ExtendText;
	
	// 
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Addtional Element")
	TMap<ECharacterPropertyType, int32>Map;
	
};
