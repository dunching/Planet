// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelNode.h"
#include "Point/VoxelPointSet.h"
#include "VoxelPinValueInterface.h"
#include "Buffer/VoxelStaticMeshBuffer.h"
#include "VoxelMakePointsNode.generated.h"

USTRUCT(BlueprintType)
struct FVoxelMakePointPoint
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Config")
	FTransform Transform;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Config")
	TObjectPtr<UStaticMesh> Mesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Config")
	FVector Normal = FVector::UpVector;

public:
	FVoxelStaticMesh RuntimeMesh;
};

USTRUCT(BlueprintType)
struct FVoxelMakePointInitializer : public FVoxelPinValueInterface
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Config")
	TArray<FVoxelMakePointPoint> Points =
	{
		// Add a single point at origin for convenience
		{}
	};

	//~ Begin FVoxelPinValueInterface Interface
	virtual void ComputeRuntimeData() override;
	//~ End FVoxelPinValueInterface Interface
};

// Create a new point set made of a single point
// Useful to manually control point placement or for preview points
USTRUCT(Category = "Point", meta = (ShowInShortList))
struct VOXELGRAPHNODES_API FVoxelNode_MakePoints : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelMakePointInitializer, Points, nullptr);
	VOXEL_INPUT_PIN(FVoxelSeed, Seed, nullptr);
	VOXEL_OUTPUT_PIN(FVoxelPointSet, Out);

	virtual bool IsPureNode() const override
	{
		return true;
	}
};