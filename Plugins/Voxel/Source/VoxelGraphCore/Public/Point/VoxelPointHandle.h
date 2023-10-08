// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "Point/VoxelPointId.h"
#include "Point/VoxelPointChunkRef.h"
#include "VoxelPointHandle.generated.h"

USTRUCT(BlueprintType)
struct VOXELGRAPHCORE_API FVoxelPointHandle
{
	GENERATED_BODY()

public:
	FVoxelPointChunkRef ChunkRef;
	FVoxelPointId PointId;

public:
	FORCEINLINE UWorld* GetWorld() const
	{
		return ChunkRef.GetWorld();
	}
	FORCEINLINE bool IsValid() const
	{
		return
			ChunkRef.IsValid() &&
			PointId.IsValid();
	}
	FORCEINLINE bool operator==(const FVoxelPointHandle& Other) const
	{
		return
			ChunkRef == Other.ChunkRef &&
			PointId == Other.PointId;
	}
	FORCEINLINE friend uint32 GetTypeHash(const FVoxelPointHandle& Handle)
	{
		return
			GetTypeHash(Handle.ChunkRef) ^
			GetTypeHash(Handle.PointId);
	}

public:
	TSharedPtr<FVoxelRuntime> GetRuntime(FString* OutError = nullptr) const;

	bool GetAttributes(
		TVoxelMap<FName, FVoxelPinValue>& InOutAttributes,
		FString* OutError = nullptr) const;

	bool Serialize(FArchive& Ar);
	bool Identical(const FVoxelPointHandle* Other, uint32 PortFlags) const;
	bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess);

private:
	bool NetSerializeImpl(FArchive& Ar, UPackageMap& Map);
};

template<>
struct TStructOpsTypeTraits<FVoxelPointHandle> : TStructOpsTypeTraitsBase2<FVoxelPointHandle>
{
	enum
	{
		WithSerializer = true,
		WithIdentical = true,
		WithNetSerializer = true,
	};
};