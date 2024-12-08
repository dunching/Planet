// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GAEvent_Helper.h"
#include "GameplayTagContainer.h"

#include "TeamConfigure.generated.h"

USTRUCT(BlueprintType)
struct PLANET_API FTeamConfigure final
{
	GENERATED_USTRUCT_BODY()

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	bool operator==(const FTeamConfigure& RightValue) const;

	TArray<FGuid> CharactersAry;
};

template <>
struct TStructOpsTypeTraits<FTeamConfigure> :
	public TStructOpsTypeTraitsBase2<FTeamConfigure>
{
	enum
	{
		WithNetSerializer = true,
		WithIdenticalViaEquality = true,
		// WithIdentical = true,
	};
};
