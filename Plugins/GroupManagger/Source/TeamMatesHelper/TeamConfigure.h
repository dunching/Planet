// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "GameplayTagContainer.h"

#include "TeamConfigure.generated.h"

USTRUCT(BlueprintType)
struct GROUPMANAGGER_API FTeammate final
{
	GENERATED_USTRUCT_BODY()

	// 小组索引
	uint8 TeamIndex = 0;
	
	// 角色在小组内的索引
	uint8 IndexInTheTeam = 0;
	
	FGuid CharacterProxyID;
	
};

/**
 * 队伍配置
 */
USTRUCT(BlueprintType)
struct GROUPMANAGGER_API FTeamConfigure final
{
	GENERATED_USTRUCT_BODY()

	FTeamConfigure();
	
	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	bool operator==(const FTeamConfigure& RightValue) const;

	void UpdateTeammateConfig(const FTeammate&Teammate);

	TArray<TArray<FGuid>> GetCharactersAry()const;
	
private:

	void Check();
	
	/**
	 * 小组索引、CharacterProxy ID，如
	 * 1. Character_A、Character_B、Character_C、Character_D
	 * 2. Character_A、Character_B、Character_C、Character_D
	 * 3. Character_A、Character_B、Character_C、Character_D
	 * 4. Character_A、Character_B、Character_C、Character_D
	 */
	TArray<TArray<FGuid>> TeamConfigureAry;
	
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
