// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include <GameplayTagContainer.h>
#include "Net/Serialization/FastArraySerializer.h"

#include "CDCaculator.generated.h"

class UCDCaculatorComponent;

struct FSkillCooldownHelper;
struct FCDItem_FASI;

USTRUCT()
struct PLANET_API FCDItem_FASI : public FFastArraySerializerItem
{
	GENERATED_USTRUCT_BODY()

	void PreReplicatedRemove(const FCD_FASI_Container& InArraySerializer);

	void PostReplicatedAdd(const FCD_FASI_Container& InArraySerializer);

	void PostReplicatedChange(const FCD_FASI_Container& InArraySerializer);

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	TSharedPtr<FSkillCooldownHelper> SkillCooldownHelper = nullptr;

};

 template<>
 struct TStructOpsTypeTraits< FCDItem_FASI > :
 	public TStructOpsTypeTraitsBase2< FCDItem_FASI >
 {
 	enum
 	{
		WithNetSerializer = true,
 	};
 };
 
USTRUCT()
struct PLANET_API FCD_FASI_Container : public FFastArraySerializer
{
	GENERATED_USTRUCT_BODY()

	using FItemType = FCDItem_FASI;
	using FContainerType = FCD_FASI_Container;

	UPROPERTY()
	TArray<FCDItem_FASI> Items;

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

	void AddItem(const TSharedPtr<FSkillCooldownHelper>& SkillCooldownHelper);

	void UpdateItem(const TSharedPtr<FSkillCooldownHelper>& SkillCooldownHelper);

	void RemoveItem(const TSharedPtr<FSkillCooldownHelper>& SkillCooldownHelper);

	UCDCaculatorComponent* CDCaculatorComponentPtr= nullptr;

protected:

};

template<>
struct TStructOpsTypeTraits< FCD_FASI_Container > :
	public TStructOpsTypeTraitsBase2< FCD_FASI_Container >
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};