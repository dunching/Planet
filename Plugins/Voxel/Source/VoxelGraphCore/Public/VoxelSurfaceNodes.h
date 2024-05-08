// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelNode.h"
#include "VoxelSurface.h"
#include "VoxelSurfaceNodes.generated.h"

USTRUCT(Category = "Surface")
struct VOXELGRAPHCORE_API FVoxelNode_GetSurfaceDistance : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelSurface, Surface, nullptr);
	VOXEL_OUTPUT_PIN(FVoxelFloatBuffer, Distance);
};

USTRUCT(Category = "Surface")
struct VOXELGRAPHCORE_API FVoxelNode_GetSurfaceMaterial : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelSurface, Surface, nullptr);
	VOXEL_OUTPUT_PIN(FVoxelSurfaceMaterial, Material);
};

USTRUCT(Category = "Surface")
struct VOXELGRAPHCORE_API FVoxelNode_GetSurfaceAttribute : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelSurface, Surface, nullptr);
	VOXEL_INPUT_PIN(FName, Name, "MyAttribute");
	VOXEL_OUTPUT_PIN(FVoxelWildcardBuffer, Value);

#if WITH_EDITOR
	virtual FVoxelPinTypeSet GetPromotionTypes(const FVoxelPin& Pin) const override;
#endif
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// Make a surface from 2D height data
// Typically, a heightmap
USTRUCT(Category = "Surface")
struct VOXELGRAPHCORE_API FVoxelNode_MakeHeightSurface : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelFloatBuffer, Height, nullptr);
	VOXEL_OUTPUT_PIN(FVoxelSurface, Surface);
};

// Make a surface from 3D distance field data
USTRUCT(Category = "Surface", meta = (Keywords = "float surface"))
struct VOXELGRAPHCORE_API FVoxelNode_MakeVolumetricSurface : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelFloatBuffer, Distance, nullptr);
	// If not set will be infinite
	VOXEL_INPUT_PIN(FVoxelBounds, Bounds, nullptr, OptionalPin, AdvancedDisplay);
	VOXEL_OUTPUT_PIN(FVoxelSurface, Surface);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(Category = "Surface")
struct VOXELGRAPHCORE_API FVoxelNode_SetSurfaceMaterial : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelSurface, Surface, nullptr);
	VOXEL_INPUT_PIN(FVoxelSurfaceMaterial, Material, nullptr);
	VOXEL_OUTPUT_PIN(FVoxelSurface, NewSurface);
};

USTRUCT(Category = "Surface")
struct VOXELGRAPHCORE_API FVoxelNode_MakeSurfaceMaterial : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelMaterialDefinitionBuffer, Material, nullptr);
	VOXEL_OUTPUT_PIN(FVoxelSurfaceMaterial, Result);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(Category = "Surface")
struct VOXELGRAPHCORE_API FVoxelNode_SetSurfaceAttribute : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelSurface, Surface, nullptr);
	VOXEL_INPUT_PIN(FName, Name, "MyAttribute");
	VOXEL_INPUT_PIN(FVoxelWildcardBuffer, Value, nullptr);
	VOXEL_OUTPUT_PIN(FVoxelSurface, NewSurface);

#if WITH_EDITOR
	virtual FVoxelPinTypeSet GetPromotionTypes(const FVoxelPin& Pin) const override;
#endif
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// Grow a surface using a buffer as input
// Use Grow Surface Uniform if you want to grow by a constant amount
// Amount is in local space
// ie, if this brush is scaled up, Amount will increase accordingly
USTRUCT(Category = "Surface", meta = (ShowInShortList, Keywords = "shift"))
struct VOXELGRAPHCORE_API FVoxelNode_GrowSurface : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelSurface, Surface, nullptr);
	// In local space
	// ie, if this brush is scaled up, will increase accordingly
	VOXEL_INPUT_PIN(FVoxelFloatBuffer, Amount, 100.f);
	// Max amount to displace by, used to compute bounds. Amount will be clamped between -MaxAmount and +MaxAmount
	// Will increase bounds, so try to keep small
	VOXEL_INPUT_PIN(float, MaxAmount, 1000.f);
	VOXEL_OUTPUT_PIN(FVoxelSurface, NewSurface);
};

// Grow a surface by a constant amount - use Grow Surface to use a buffer as input
// Amount is in local space
// ie, if this brush is scaled up, Amount will increase accordingly
USTRUCT(Category = "Surface", meta = (ShowInShortList, Keywords = "shift"))
struct VOXELGRAPHCORE_API FVoxelNode_GrowSurfaceUniform : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelSurface, Surface, nullptr);
	// In local space
	// ie, if this brush is scaled up, will increase accordingly
	VOXEL_INPUT_PIN(float, Amount, 100.f);
	VOXEL_OUTPUT_PIN(FVoxelSurface, NewSurface);
};