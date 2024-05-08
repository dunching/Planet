// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelNode.h"
#include "VoxelSurface.h"
#include "VoxelGetSculptSurfaceNode.generated.h"

class FVoxelSculptStorageData;

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelSculptStorageQueryParameter : public FVoxelQueryParameter
{
	GENERATED_BODY()
	GENERATED_VOXEL_QUERY_PARAMETER_BODY()

	FVoxelTransformRef SurfaceToWorld;
	TSharedPtr<FVoxelSculptStorageData> Data;
	float VoxelSize = 0.f;
	TSharedPtr<const TVoxelComputeValue<FVoxelSurface>> Compute;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(Category = "Sculpt")
struct VOXELGRAPHCORE_API FVoxelNode_GetSculptSurface : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_OUTPUT_PIN(FVoxelSurface, Surface);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(meta = (Internal))
struct VOXELGRAPHCORE_API FVoxelNode_GetSculptSurface_Distance : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_CALL_PARAM(FVoxelTransformRef, SurfaceToWorld);
	VOXEL_CALL_PARAM(TSharedPtr<FVoxelSculptStorageData>, Data);
	VOXEL_CALL_PARAM(float, VoxelSize);
	VOXEL_CALL_PARAM(TSharedPtr<const FVoxelSurface>, Surface);

	VOXEL_OUTPUT_PIN(FVoxelFloatBuffer, Distance);
};