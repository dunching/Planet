// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Abilities/GameplayAbilityTargetTypes.h"

#include "PlanetGameplayAbilityTargetTypes.generated.h"

/** Target data with a source location and a list of targeted actors, makes sense for AOE attacks */
USTRUCT(BlueprintType)
struct PLANET_API FGameplayAbilityTargetData_MyActorArray : public FGameplayAbilityTargetData_ActorArray
{
	GENERATED_USTRUCT_BODY()

	virtual bool HasHitResult() const override
	{
		return true;
	}

	virtual const FHitResult* GetHitResult() const override
	{
		return &HitResult;
	}

	bool NetSerialize(
		FArchive& Ar,
		class UPackageMap* Map,
		bool& bOutSuccess
		);

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FGameplayAbilityTargetData_MyActorArray::StaticStruct();
	}

	/** Hit result that stores data */
	UPROPERTY()
	FHitResult HitResult;
};

template <>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_MyActorArray> : public TStructOpsTypeTraitsBase2<
		FGameplayAbilityTargetData_MyActorArray>
{
	enum
	{
		WithNetSerializer = true
		// For now this is REQUIRED for FGameplayAbilityTargetDataHandle net serialization to work
	};
};
