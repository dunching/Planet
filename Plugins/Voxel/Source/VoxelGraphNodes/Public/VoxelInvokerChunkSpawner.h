// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelNode.h"
#include "VoxelFastOctree.h"
#include "VoxelChunkSpawner.h"
#include "VoxelInvokerChunkSpawner.generated.h"

class FVoxelInvokerView;

USTRUCT()
struct VOXELGRAPHNODES_API FVoxelInvokerChunkSpawner : public FVoxelChunkSpawner
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

public:
	FVoxelGraphNodeRef NodeRef;
	int32 LOD = 0;
	float WorldSize = 0.f;
	int32 ChunkSize = 0;
	FName InvokerChannel;

	//~ Begin FVoxelChunkSpawnerImpl Interface
	virtual void Tick(FVoxelRuntime& Runtime) override;
	//~ End FVoxelChunkSpawnerImpl Interface

private:
	TSharedPtr<FVoxelInvokerView> InvokerView_GameThread;
	FSharedVoidPtr InvokerViewBindRef_GameThread;

	FVoxelFastCriticalSection CriticalSection;
	TVoxelIntVectorMap<TSharedPtr<FVoxelChunkRef>> ChunkRefs_RequiresLock;
};

USTRUCT(Category = "Chunk Spawner")
struct VOXELGRAPHNODES_API FVoxelNode_MakeInvokerChunkSpawner : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(int32, LOD, 0);
	VOXEL_INPUT_PIN(float, WorldSize, 1.e6f);
	VOXEL_INPUT_PIN(int32, ChunkSize, 32);
	VOXEL_INPUT_PIN(FName, InvokerChannel, "Default");

	VOXEL_OUTPUT_PIN(FVoxelChunkSpawner, Spawner);
};