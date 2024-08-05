// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "RaffleCommon.generated.h"

UENUM(BlueprintType)
enum class ERaffleType : uint8
{
	// 常驻池
	kRafflePermanent,

	// 限定池
	kRaffleLimit,

	// 新手池
	kNovice,
};
