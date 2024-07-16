// Zowee. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include <GenerateType.h>

#include "GameplayTagContainer.h"
#include "SceneElement.h"

#include "StateTagExtendInfo.generated.h"

USTRUCT(BlueprintType)
struct PLANET_API FStateTagExtendInfo
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString ExtendText;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ToolsIcons")
	TSoftObjectPtr<UTexture2D> DefaultIcon;

};

UCLASS(BlueprintType, Blueprintable)
class PLANET_API UStateTagExtendInfoMap : public UObject
{
	GENERATED_BODY()
public:

	UStateTagExtendInfoMap();

	static UStateTagExtendInfoMap* GetInstance();
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GeneratorNPCClass")
	TMap<FGameplayTag, FStateTagExtendInfo>TagsExtendMap;
	
};
