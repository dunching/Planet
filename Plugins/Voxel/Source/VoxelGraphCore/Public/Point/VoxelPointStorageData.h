// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelBufferBuilder.h"
#include "Point/VoxelPointHandle.h"

class FVoxelDependency;

class VOXELGRAPHCORE_API FVoxelPointStorageChunkData
{
public:
	const TSharedRef<FVoxelDependency> Dependency;
	mutable FVoxelFastCriticalSection CriticalSection;

	struct FAttribute
	{
		TVoxelMap<FVoxelPointId, int32> PointIdToIndex;
		TSharedPtr<FVoxelBufferBuilder> Buffer;
	};
	TVoxelMap<FName, TSharedPtr<FAttribute>> NameToAttributeOverride;

	explicit FVoxelPointStorageChunkData(const TSharedRef<FVoxelDependency>& Dependency)
		: Dependency(Dependency)
	{
	}
};

class VOXELGRAPHCORE_API FVoxelPointStorageData : public TSharedFromThis<FVoxelPointStorageData>
{
public:
	const FName AssetName;
	const TSharedRef<FVoxelDependency> Dependency;

	explicit FVoxelPointStorageData(FName AssetName);

	void ClearData();
	void Serialize(FArchive& Ar);

	TSharedRef<FVoxelPointStorageChunkData> FindOrAddChunkData(const FVoxelPointChunkRef& ChunkRef);

	bool SetPointAttribute(
		const FVoxelPointHandle& Handle,
		FName Name,
		const FVoxelPinType& Type,
		const FVoxelPinValue& Value,
		FString* OutError = nullptr);

private:
	mutable FVoxelFastCriticalSection CriticalSection;
	TVoxelMap<FVoxelPointChunkRef, TSharedPtr<FVoxelPointStorageChunkData>> ChunkRefToChunkData_RequiresLock;
};