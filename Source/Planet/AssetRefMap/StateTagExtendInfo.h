// Zowee. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include <GenerateType.h>

#include "GameplayTagContainer.h"
#include "SceneElement.h"

#include "StateTagExtendInfo.generated.h"

USTRUCT(BlueprintType)
struct PLANET_API FTableRowUnit_TagExtendInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString ExtendText;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ToolsIcons")
	TSoftObjectPtr<UTexture2D> DefaultIcon;

};
