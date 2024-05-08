// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelNode.h"
#include "Point/VoxelChunkedPointSet.h"
#include "VoxelMakeChunkedPointsNode.generated.h"

USTRUCT(Category = "Point", meta = (ShowInShortList))
struct VOXELGRAPHCORE_API FVoxelNode_MakeChunkedPoints : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelPointSet, In, nullptr, VirtualPin);
	VOXEL_INPUT_PIN(int32, ChunkSize, 10000);
	VOXEL_OUTPUT_PIN(FVoxelChunkedPointSet, Out);
};