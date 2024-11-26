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
struct FAllocation_FASI_Container;

class UProxyProcessComponent;
class UHoldingItemsComponent;
class ACharacterBase;

// 如果直接复制这个结构 访问ProxyID时需要特殊处理一下
USTRUCT(BlueprintType)
struct FSocket_FASI : public FFastArraySerializerItem
{
	GENERATED_USTRUCT_BODY()

	void PreReplicatedRemove(const FAllocation_FASI_Container& InArraySerializer);
	
	// 注意：这里在UpdateItem时 有时也会进入这个函数？而且Items会增加，具体原因未知
	void PostReplicatedAdd(const FAllocation_FASI_Container& InArraySerializer);

	void PostReplicatedChange(const FAllocation_FASI_Container& InArraySerializer);

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	bool operator==(const FSocket_FASI& Right)const;

	FKey Key;

	FGameplayTag Socket;

	// 
	FGuid SkillProxyID;

	TSharedPtr<FBasicProxy>ProxySPtr = nullptr;

	ACharacterBase*CharacterPtr = nullptr;
	
};

template<>
struct TStructOpsTypeTraits<FSocket_FASI> :
	public TStructOpsTypeTraitsBase2<FSocket_FASI>
{
	enum
	{
		WithIdenticalViaEquality = true,
		WithNetSerializer = true,
	};
};

USTRUCT(BlueprintType)
struct PLANET_API FAllocation_FASI_Container : public FFastArraySerializer
{
	GENERATED_USTRUCT_BODY()
	
	friend ACharacterBase;

	using FItemType = FSocket_FASI;
	using FContainerType = FAllocation_FASI_Container;
	
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

	void AddItem(const TSharedPtr<FItemType>& ProxySPtr);

	void UpdateItem(const TSharedPtr<FItemType>& ProxySPtr);

	ACharacterBase*CharacterPtr = nullptr;
	
protected:
	
	UPROPERTY()
	TArray<FSocket_FASI> Items;

private:

};

template<>
struct TStructOpsTypeTraits< FAllocation_FASI_Container > :
	public TStructOpsTypeTraitsBase2< FAllocation_FASI_Container >
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};