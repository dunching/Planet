// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include <GenerateTypes.h>

#include "GameplayTagContainer.h"
#include "ItemProxy_Description.h"

#include "Regions.generated.h"

class AHumanCharacter_AI;

USTRUCT(BlueprintType)
struct PLANET_API FTableRow_Regions : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FGameplayTag RegionTag;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FString Name;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FString Description;
	
	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category=Collision,
		meta = (AllowPrivateAccess = true)
	)
	TSoftObjectPtr<USoundWave> RegionBGM = nullptr;
};