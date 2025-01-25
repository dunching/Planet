// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>
#include <variant>

#include "CoreMinimal.h"
#include <GameplayTagContainer.h>
#include "Net/Serialization/FastArraySerializer.h"

#include "GenerateType.h"
#include "BaseData.h"

#include "ItemProxy_Container.generated.h"

class AToolProxyBase;
class AWeapon_Base;
class USkill_Consumable_Base;
class AConsumable_Base;
class IPlanetControllerInterface;

class USkill_Base;
class ACharacterBase;
class AHumanCharacter;
class UInventoryComponent;

struct FBasicProxy;
struct FTableRowProxy;

FTableRowProxy* GetTableRowProxy(const FGameplayTag &ProxyType);

USTRUCT()
struct PLANET_API FProxy_FASI : public FFastArraySerializerItem
{
	GENERATED_USTRUCT_BODY()

	void PreReplicatedRemove(const FProxy_FASI_Container& InArraySerializer);

	void PostReplicatedAdd(const FProxy_FASI_Container& InArraySerializer);

	void PostReplicatedChange(const FProxy_FASI_Container& InArraySerializer);

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	bool operator==(const FProxy_FASI& Right)const;

	TSharedPtr<FBasicProxy> ProxySPtr = nullptr;

};

 template<>
 struct TStructOpsTypeTraits< FProxy_FASI > :
 	public TStructOpsTypeTraitsBase2< FProxy_FASI >
 {
 	enum
 	{
		WithNetSerializer = true,
 	};
 };

USTRUCT()
struct PLANET_API FProxy_FASI_Container : public FFastArraySerializer
{
	GENERATED_USTRUCT_BODY()

	friend ACharacterBase;

	using FItemType = FProxy_FASI;
	using FContainerType = FProxy_FASI_Container;

	UPROPERTY()
	TArray<FProxy_FASI> Items;

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

	void AddItem(const TSharedPtr<FBasicProxy>& ProxySPtr);

	void UpdateItem(const FGuid& Proxy_ID);

	void UpdateItem(const TSharedPtr<FBasicProxy>& ProxySPtr);

	void RemoveItem(const TSharedPtr<FBasicProxy>& ProxySPtr);

	UInventoryComponent* InventoryComponentPtr = nullptr;

protected:

	TSharedPtr<FBasicProxy> GetProxyType(const FGameplayTag& ProxyType);

};

template<>
struct TStructOpsTypeTraits< FProxy_FASI_Container > :
	public TStructOpsTypeTraitsBase2< FProxy_FASI_Container >
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};