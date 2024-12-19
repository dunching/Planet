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

	FTeamConfigure();
	
	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	bool operator==(const FTeamConfigure& RightValue) const;

	void UpdateTeammateConfig(const FGuid&ID, int32 Index);

	TArray<FGuid> GetCharactersAry()const;
	
private:
	
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
