// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "Rendering/VoxelMesh.h"
#include "VoxelPhysicalMaterial.h"
#include "Material/VoxelMaterial.h"
#include "Collision/VoxelCollider.h"
#include "Buffer/VoxelFloatBuffers.h"
#include "GameFramework/DefaultPhysicsVolume.h"
#include "VoxelMarchingCubeNodes.generated.h"

struct FVoxelMarchingCubeCell
{
	uint8 X = 0;
	uint8 Y = 0;
	uint8 Z = 0;
	uint8 NumTriangles = 0;
	int32 FirstTriangle = 0;
};
checkStatic(sizeof(FVoxelMarchingCubeCell) == 8);

USTRUCT()
struct VOXELGRAPHNODES_API FVoxelMarchingCubeSurface
{
	GENERATED_BODY()

	int32 LOD = 0;
	int32 ChunkSize = 0;
	float ScaledVoxelSize = 0.f;
	FVoxelBox ChunkBounds;

	int32 NumEdgeVertices = 0;

	TVoxelArray<FVoxelMarchingCubeCell> Cells;
	TVoxelArray<int32> Indices;
	TVoxelArray<FVector3f> Vertices;
	TVoxelArray<int32> CellIndices;

	struct FTransitionIndex
	{
		uint32 bIsRelative : 1;
		uint32 Index : 31;
	};
	TVoxelStaticArray<TVoxelArray<FTransitionIndex>, 6> TransitionIndices;

	struct FTransitionVertex
	{
		FVector3f Position;
		int32 SourceVertex = 0;
	};
	TVoxelStaticArray<TVoxelArray<FTransitionVertex>, 6> TransitionVertices;

	TVoxelStaticArray<TVoxelArray<int32>, 6> TransitionCellIndices;
};

USTRUCT(meta = (Internal))
struct VOXELGRAPHNODES_API FVoxelNode_GenerateMarchingCubeSurface : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelFloatBuffer, Distance, nullptr);
	VOXEL_INPUT_PIN(float, VoxelSize, nullptr);
	VOXEL_INPUT_PIN(int32, ChunkSize, nullptr);
	VOXEL_INPUT_PIN(FVoxelBox, Bounds, nullptr);
	VOXEL_INPUT_PIN(bool, EnableTransitions, nullptr);
	VOXEL_INPUT_PIN(bool, PerfectTransitions, nullptr);
	VOXEL_INPUT_PIN(bool, EnableDistanceChecks, nullptr);
	VOXEL_INPUT_PIN(float, DistanceChecksTolerance, nullptr);
	VOXEL_OUTPUT_PIN(FVoxelMarchingCubeSurface, Surface);
};

USTRUCT(meta = (Internal))
struct VOXELGRAPHNODES_API FVoxelNode_CreateMarchingCubeCollider : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelMarchingCubeSurface, Surface, nullptr);
	VOXEL_INPUT_PIN(FVoxelPhysicalMaterialBuffer, PhysicalMaterial, nullptr);
	VOXEL_OUTPUT_PIN(FVoxelCollider, Collider);
};

USTRUCT(meta = (Internal))
struct VOXELGRAPHNODES_API FVoxelNode_CreateMarchingCubeMesh : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelMarchingCubeSurface, Surface, nullptr);
	VOXEL_INPUT_PIN(FVoxelMaterial, Material, nullptr);
	VOXEL_INPUT_PIN(FVoxelFloatBuffer, Distance, nullptr);
	VOXEL_INPUT_PIN(bool, GenerateDistanceField, nullptr);
	VOXEL_INPUT_PIN(float, DistanceFieldBias, nullptr);
	VOXEL_OUTPUT_PIN(FVoxelMesh, Mesh);
};