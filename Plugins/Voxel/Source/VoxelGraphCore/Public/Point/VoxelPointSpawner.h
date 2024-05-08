// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelPointSet.h"
#include "VoxelNodeHelpers.h"
#include "VoxelPointSpawner.generated.h"

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelPointSpawner
	: public FVoxelVirtualStruct
	, public IVoxelNodeInterface
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

public:
	FVoxelGraphNodeRef Node;

	//~ Begin IVoxelNodeInterface Interface
	virtual const FVoxelGraphNodeRef& GetNodeRef() const override
	{
		return Node;
	}
	//~ End IVoxelNodeInterface Interface

public:
	virtual TSharedPtr<const FVoxelPointSet> GeneratePoints(const FVoxelPointSet& Points) const VOXEL_PURE_VIRTUAL({});
};