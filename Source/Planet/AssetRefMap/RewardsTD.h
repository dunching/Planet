// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include <GenerateTypes.h>

#include "GameplayTagContainer.h"
#include "ItemProxy_Minimal.h"

#include "RewardsTD.generated.h"

USTRUCT(BlueprintType)
struct PLANET_API FRewardsItem 
{
	GENERATED_BODY()

	bool IsValid()const;
	
#if WITH_EDITORONLY_DATA
	UPROPERTY()
	FString Description;
#endif
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TMap<FGameplayTag, int32>RewardsMap;
};

UCLASS()
class PLANET_API UPAD_RewardsItems : public UDataAsset
{
	GENERATED_BODY()

public:
	
	static UPAD_RewardsItems* GetInstance();

	FRewardsItem GetRewardsItem(const FGuid& RewardsItemID);
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TMap<FGuid, FRewardsItem> ProxyMap;
};

/**
 * 在开放世界中，击败敌人后会获得的奖励
 */
USTRUCT(BlueprintType)
struct PLANET_API FTableRow_RewardsItems_DefeatEnemy : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TMap<FGameplayTag, int32>RewardsMap;
	
};
