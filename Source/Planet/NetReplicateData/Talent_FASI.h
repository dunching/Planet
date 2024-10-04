// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>
#include <variant>

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "GenerateType.h"

#include "Talent_FASI.generated.h"

class UTalentAllocationComponent;
struct FTalent_FASI_Container;

USTRUCT(BlueprintType)
struct FTalentHelper final
{
	GENERATED_USTRUCT_BODY()

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	bool operator==(const FTalentHelper& RightValue)const;

	std::variant<EPointSkillType, EPointPropertyType>Type = EPointPropertyType::kLiDao;

	EPointType PointType = EPointType::kNone;

	int32 Level = 0;

	int32 TotalLevel = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SkillSocket")
	FGameplayTag IconSocket;

};

template<>
struct TStructOpsTypeTraits< FTalentHelper > :
	public TStructOpsTypeTraitsBase2< FTalentHelper >
{
	enum
	{
		WithNetSerializer = true,
	};
};

USTRUCT()
struct PLANET_API FTalentItem_FASI : public FFastArraySerializerItem
{
	GENERATED_USTRUCT_BODY()

	void PostReplicatedAdd(const FTalent_FASI_Container& InArraySerializer);

	void PostReplicatedChange(const FTalent_FASI_Container& InArraySerializer);

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	FTalentHelper TalentHelper;

};

template<>
struct TStructOpsTypeTraits< FTalentItem_FASI > :
	public TStructOpsTypeTraitsBase2< FTalentItem_FASI >
{
	enum
	{
		WithNetSerializer = true,
	};
};

USTRUCT()
struct PLANET_API FTalent_FASI_Container : public FFastArraySerializer
{
	GENERATED_USTRUCT_BODY()

	using FItemType = FTalentItem_FASI;
	using FContainerType = FTalent_FASI_Container;

	UPROPERTY()
	TArray<FTalentItem_FASI> Items;

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

	void AddItem(const FTalentHelper& TalentHelper);

	void UpdateItem(const FTalentHelper& TalentHelper);

	UTalentAllocationComponent* TalentAllocationComponentPtr = nullptr;

protected:

};

template<>
struct TStructOpsTypeTraits<FTalent_FASI_Container> :
	public TStructOpsTypeTraitsBase2<FTalent_FASI_Container>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};