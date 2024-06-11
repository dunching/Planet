// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>
#include <variant>

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "TalentUnit.generated.h"

UENUM(BlueprintType)
enum class EPointSkillType :uint8
{
	kNuQi,
	kYinYang,
	kDuXing,
	kFaLi,
	kGongMing,
};

UENUM(BlueprintType)
enum class EPointPropertyType :uint8
{
	kLiDao,
	kGenGu,
	kShenFa,
	kDongCha,
	kTianZi,
};

UENUM(BlueprintType)
enum class EPointType :uint8
{
	kNone,
	kSkill,
	kProperty,
};

USTRUCT(BlueprintType)
struct FTalentHelper
{
	GENERATED_USTRUCT_BODY()

	bool operator==(const FTalentHelper& RightValue)const;

	std::variant<EPointSkillType, EPointPropertyType>Type;

	EPointType PointType = EPointType::kNone;

	int32 Level = 0;

	int32 TotalLevel = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SkillSocket")
	FGameplayTag IconSocket;

};