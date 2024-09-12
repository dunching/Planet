// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include <GameplayTagContainer.h>

#include "GenerateType.h"
#include "BaseData.h"
#include "GAEvent_Helper.h"

#include "AllocationSkills.generated.h"

struct FSkillProxy;
struct FWeaponProxy;
struct FConsumableProxy;
class UUnitProxyProcessComponent;

USTRUCT(BlueprintType)
struct FSocket_FASI : public FFastArraySerializerItem
{
	GENERATED_USTRUCT_BODY()

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	FKey Key;

	FGameplayTag Socket;

	TWeakPtr<FBasicProxy> UnitPtr = nullptr;

};

template<>
struct TStructOpsTypeTraits<FSocket_FASI> :
	public TStructOpsTypeTraitsBase2<FSocket_FASI>
{
	enum
	{
		WithNetSerializer = true,
	};
};

USTRUCT(BlueprintType)
struct PLANET_API FAllocationSkills_FASI : public FFastArraySerializer
{
	GENERATED_USTRUCT_BODY()
	
	friend ACharacterBase;

	using FItemType = FSocket_FASI;
	using FContainerType = FAllocationSkills_FASI;
	
	UPROPERTY()
	TArray<FSocket_FASI> Items;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms);

	template< typename Type, typename SerializerType >
	bool ShouldWriteFastArrayItem(const Type& Item, const bool bIsWritingOnClient)
	{
		if (bIsWritingOnClient)
		{
			return Item.ReplicationID != INDEX_NONE;
		}

		return true;
	}

	UHoldingItemsComponent* HoldingItemsComponentPtr = nullptr;

	void AddItem(const TSharedPtr<FBasicProxy>& ProxySPtr);

	void UpdateItem(const TSharedPtr<FBasicProxy>& ProxySPtr);

	UUnitProxyProcessComponent* UnitProxyProcessComponentPtr = nullptr;

private:

};

template<>
struct TStructOpsTypeTraits< FAllocationSkills_FASI > :
	public TStructOpsTypeTraitsBase2< FAllocationSkills_FASI >
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};