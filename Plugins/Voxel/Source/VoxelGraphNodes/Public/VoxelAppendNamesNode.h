// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelNode.h"
#include "VoxelAppendNamesNode.generated.h"

USTRUCT(Category = "Name", meta = (ShowInShortList))
struct VOXELGRAPHNODES_API FVoxelNode_AppendNames : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN_ARRAY(FName, Names, nullptr, 2);
	VOXEL_OUTPUT_PIN(FName, Name);

	//~ Begin FVoxelNode Interface
	virtual bool IsPureNode() const override
	{
		return true;
	}
	//~ End FVoxelNode Interface
};