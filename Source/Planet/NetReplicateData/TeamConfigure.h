// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Net/Serialization/FastArraySerializer.h"

#include "GenerateType.h"

#include "TeamConfigure.generated.h"

class ACharacterBase;
class APlanetPlayerState;
class UGroupMnaggerComponent;

struct FAllocation_FASI_Container;
struct FCharacterProxy;

USTRUCT(BlueprintType)
struct PLANET_API FTeamConfigure final
{
	GENERATED_USTRUCT_BODY()

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	bool Identical(const FTeamConfigure* Other, uint32 PortFlags) const;

	FGuid Guid = FGuid();

	TWeakObjectPtr<ACharacterBase>ForceKnowCharater;

	TArray<TPair<TWeakObjectPtr<ACharacterBase>, int32>>KnowCharatersSet;

	ETeammateOption TeammateOption = ETeammateOption::kEnemy;

private:

	bool bIsNotChanged = true;

};

template<>
struct TStructOpsTypeTraits< FTeamConfigure > :
	public TStructOpsTypeTraitsBase2< FTeamConfigure >
{
	enum
	{
		WithNetSerializer = true,
//		WithIdenticalViaEquality = true,
		WithIdentical = true,
	};
};

USTRUCT(BlueprintType)
struct FTeammate_FASI : public FFastArraySerializerItem
{
	GENERATED_USTRUCT_BODY()

	void PreReplicatedRemove(const FTeammate_FASI_Container& InArraySerializer);
	
	void PostReplicatedAdd(const FTeammate_FASI_Container& InArraySerializer);

	void PostReplicatedChange(const FTeammate_FASI_Container& InArraySerializer);

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	// 这个“角色”在队伍里面的位置
	int32 Index = 0;

	FGuid CharacterProxyID;
	
	FGuid PreviousCharacterProxyID;

	ACharacterBase* OwnerCharacterPtr = nullptr;

};

template<>
struct TStructOpsTypeTraits<FTeammate_FASI> :
	public TStructOpsTypeTraitsBase2<FTeammate_FASI>
{
	enum
	{
		WithNetSerializer = true,
	};
};

USTRUCT(BlueprintType)
struct PLANET_API FTeammate_FASI_Container : public FFastArraySerializer
{
	GENERATED_USTRUCT_BODY()
	
	friend APlanetPlayerState;

	using FItemType = FTeammate_FASI;
	using FContainerType = FTeammate_FASI_Container;
	
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

	void UpdateItem(const FItemType& Item);

	APlanetPlayerState* OwnerPtr = nullptr;
	
	UPROPERTY()
	TArray<FTeammate_FASI> Items;

protected:
	
private:
	
};

template<>
struct TStructOpsTypeTraits< FTeammate_FASI_Container > :
	public TStructOpsTypeTraitsBase2< FTeammate_FASI_Container >
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};