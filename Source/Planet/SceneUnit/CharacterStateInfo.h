// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>
#include <variant>

#include "CoreMinimal.h"
#include <GameplayTagContainer.h>
#include "Net/Serialization/FastArraySerializer.h"

#include "GenerateType.h"
#include "BaseData.h"

#include "CharacterStateInfo.generated.h"

class UTexture2D;
class UStateProcessorComponent;

USTRUCT()
struct PLANET_API FCharacterStateInfo 
{
	GENERATED_USTRUCT_BODY()

	using FDataChanged = TCallbackHandleContainer<void()>;

	FCharacterStateInfo();

	float GetRemainTime()const ;

	float GetRemainTimePercent()const;

	// 总时间
	float Duration = -1.f;

	// 已过去的时间
	float TotalTime = 0.f;

	// 层数 <= 0 不显示
	int32 Num = 0;

	FString Text = TEXT("");

	FGameplayTag Tag;

	TSoftObjectPtr<UTexture2D> DefaultIcon;

	FDataChanged DataChanged;

};

USTRUCT()
struct PLANET_API FCharacterStateInfo_FASI : public FFastArraySerializerItem
{
	GENERATED_USTRUCT_BODY()

	void PreReplicatedRemove(const FCharacterStateInfo_FASI_Container& InArraySerializer);

	void PostReplicatedAdd(const FCharacterStateInfo_FASI_Container& InArraySerializer);

	void PostReplicatedChange(const FCharacterStateInfo_FASI_Container& InArraySerializer);

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	TSharedPtr<FCharacterStateInfo> CharacterStateInfoSPtr = nullptr;

};

template<>
struct TStructOpsTypeTraits< FCharacterStateInfo_FASI > :
	public TStructOpsTypeTraitsBase2< FCharacterStateInfo_FASI >
{
	enum
	{
		WithNetSerializer = true,
	};
};

USTRUCT()
struct PLANET_API FCharacterStateInfo_FASI_Container : public FFastArraySerializer
{
	GENERATED_USTRUCT_BODY()

	using FItemType = FCharacterStateInfo_FASI;
	using FContainerType = FCharacterStateInfo_FASI_Container;

	UPROPERTY()
	TArray<FCharacterStateInfo_FASI> Items;

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

	void AddItem(const TSharedPtr<FCharacterStateInfo>& CharacterStateInfoSPtr);

	void UpdateItem(const TSharedPtr<FCharacterStateInfo>& CharacterStateInfoSPtr);

	void RemoveItem(const TSharedPtr<FCharacterStateInfo>& CharacterStateInfoSPtr);

	UStateProcessorComponent* StateProcessorComponent = nullptr;

protected:

};

template<>
struct TStructOpsTypeTraits< FCharacterStateInfo_FASI_Container > :
	public TStructOpsTypeTraitsBase2< FCharacterStateInfo_FASI_Container >
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};