// Zowee. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include <GenerateTypes.h>

#include "GameplayTagContainer.h"
#include "ItemProxy_Minimal.h"

#include "TalentInfo.generated.h"

UCLASS()
class PLANET_API UPAD_Talent_Property : public UDataAsset
{
	GENERATED_BODY()

public:

	// 每级 给的属性
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Addtional Element")
	TMap<ECharacterPropertyType, int32>AddValueMap;
	
	// 每级 给的 属性 结算修正
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Addtional Element")
	TMap<ECharacterPropertyType, float>ModifyValueMap;
	
};
 