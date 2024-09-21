// Zowee. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include <GenerateType.h>

#include "GameplayTagContainer.h"
#include "SceneElement.h"

#include "PropertyEntrys.generated.h"

USTRUCT(BlueprintType)
struct PLANET_API FTableRowUnit_PropertyEntrys : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString ExtendText;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ToolsIcons")
	ECharacterPropertyType CharacterPropertyType = ECharacterPropertyType::AD;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ToolsIcons")
	int32 Value = 0;

};
