// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelChunkSpawner.h"

DEFINE_UNIQUE_VOXEL_ID(FVoxelChunkId);

VOXEL_CONSOLE_VARIABLE(
	VOXELGRAPHCORE_API, int32, GVoxelChunkSpawnerMaxChunks, 1000000,
	"voxel.chunkspawner.MaxChunks",
	"");

VOXEL_CONSOLE_VARIABLE(
	VOXELGRAPHCORE_API, float, GVoxelChunkSpawnerCameraRefreshThreshold, 1.f,
	"voxel.chunkspawner.CameraRefreshThreshold",
	"");

TSharedRef<FVoxelChunkRef> FVoxelChunkSpawner::CreateChunk(
	const int32 LOD,
	const int32 ChunkSize,
	const FVoxelBox& Bounds) const
{
	const TSharedPtr<FVoxelChunkRef> ChunkRef = PrivateCreateChunkLambda(LOD, ChunkSize, Bounds);
	if (!ensure(ChunkRef))
	{
		return MakeVoxelShared<FVoxelChunkRef>(FVoxelChunkId(), MakeVoxelShared<FVoxelChunkActionQueue>());
	}
	return ChunkRef.ToSharedRef();
}