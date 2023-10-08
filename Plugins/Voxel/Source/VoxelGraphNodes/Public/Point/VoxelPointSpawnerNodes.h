// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelNode.h"
#include "Point/VoxelPointSpawner.h"
#include "VoxelPointSpawnerNodes.generated.h"

USTRUCT()
struct VOXELGRAPHNODES_API FVoxelRadialPointSpawner : public FVoxelPointSpawner
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

	FVoxelFloatRange Radius;
	FVoxelFloatRange RadialOffset;
	FVoxelInt32Range NumPoints;
	FVoxelSeed Seed;

	virtual TSharedPtr<const FVoxelPointSet> GeneratePoints(const FVoxelPointSet& Points) const override;
};

USTRUCT(Category = "Point")
struct VOXELGRAPHNODES_API FVoxelNode_MakeRadialPointSpawner : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelFloatRange, Radius, FVoxelFloatRange(0, 200));
	// In degrees
	VOXEL_INPUT_PIN(float, RadialOffset, 10.f);
	VOXEL_INPUT_PIN(FVoxelInt32Range, NumPoints, FVoxelInt32Range(5, 10));
	VOXEL_INPUT_PIN(FVoxelSeed, Seed, nullptr, AdvancedDisplay);
	VOXEL_OUTPUT_PIN(FVoxelPointSpawner, PointSpawner);
};