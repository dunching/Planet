// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include <GameplayTagContainer.h>

#include "GenerateTypes.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "OnEffectedTawrgetCallback.generated.h"

class UConversationComponent;
class ACharacterBase;


/**
 * 对目标造成影响后的回执
 */
USTRUCT()
struct PLANET_API FOnEffectedTawrgetCallback
{
	GENERATED_USTRUCT_BODY()

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	ACharacterBase* InstigatorCharacterPtr = nullptr;

	ACharacterBase* TargetCharacterPtr = nullptr;

	EElementalType ElementalType = EElementalType::kMetal;
	
	bool bIsDeath = false;

	bool bIsEvade = false;

	bool bIsCritical = false;

	FGameplayTagContainer AllAssetTags;

	TMap<FGameplayTag, float> SetByCallerTagMagnitudes;

	/**
	 * 伤害值
	 */
	int32 Damage = 0;
	
	/**
	 * 治疗量
	 */
	int32 TherapeuticalDose = 0;
};

template <>
struct TStructOpsTypeTraits<FOnEffectedTawrgetCallback> :
	public TStructOpsTypeTraitsBase2<FOnEffectedTawrgetCallback>
{
	enum
	{
		WithNetSerializer = true,
	};
};
