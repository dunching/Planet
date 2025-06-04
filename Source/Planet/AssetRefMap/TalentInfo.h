// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include <GenerateTypes.h>

#include "GameplayTagContainer.h"
#include "ItemProxy_Minimal.h"

#include "TalentInfo.generated.h"

UENUM(BlueprintType, Blueprintable)
enum class ETalentType : uint8
{
	kData,
	kPercent,
};

USTRUCT(BlueprintType)
struct PLANET_API FTableRow_TalenSocket : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	/**
	 * 属性Tag
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SkillSocket")
	FGameplayTag ModifyDataTypeTag;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SkillSocket")
	ETalentType TalentType = ETalentType::kData;

	/**
	 * 
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SkillSocket")
	int32 Value = 1;
};
 