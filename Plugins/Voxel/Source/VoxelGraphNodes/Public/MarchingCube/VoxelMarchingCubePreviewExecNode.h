// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelSurface.h"
#include "VoxelExecNode.h"
#include "Material/VoxelMaterial.h"
#include "VoxelMarchingCubePreviewExecNode.generated.h"

struct FVoxelMesh;
struct FVoxelCollider;
class UVoxelMeshComponent;
class UVoxelCollisionComponent;

USTRUCT()
struct VOXELGRAPHNODES_API FVoxelMarchingCubeBrushPreviewMesh
{
	GENERATED_BODY()

	FVoxelBox Bounds;
	bool bOnlyDrawIfSelected = false;
	TSharedPtr<const FVoxelMesh> Mesh;
	TSharedPtr<const FBodyInstance> BodyInstance;
	TSharedPtr<const FVoxelCollider> Collider;
};

USTRUCT(DisplayName = "Create Marching Cube Preview Mesh")
struct VOXELGRAPHNODES_API FVoxelMarchingCubePreviewExecNode : public FVoxelExecNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelSurface, Surface, nullptr, VirtualPin);
	// Disable if this is a sculpt tool
	VOXEL_INPUT_PIN(bool, OnlyDrawIfSelected, true, VirtualPin);
	// If not set will be automatically computed from Surface
	VOXEL_INPUT_PIN(FVoxelBounds, Bounds, nullptr, VirtualPin, AdvancedDisplay);
	// The size of the mesh to create in voxels
	VOXEL_INPUT_PIN(int32, Size, 32, VirtualPin, AdvancedDisplay);
	VOXEL_INPUT_PIN(FVoxelMaterial, Material, "/Voxel/EditorAssets/M_BrushPreview.M_BrushPreview", VirtualPin, AdvancedDisplay);
	// Body instance for the collision, ignored in editor worlds
	VOXEL_INPUT_PIN(FBodyInstance, BodyInstance, nullptr, VirtualPin, AdvancedDisplay);
	// If true the mesh will only be drawn if it's selected in the editor

	TValue<FVoxelMarchingCubeBrushPreviewMesh> CreateMesh(
		const FVoxelQuery& Query,
		float VoxelSize,
		int32 ChunkSize,
		const FVoxelBox& Bounds) const;
	virtual TVoxelUniquePtr<FVoxelExecNodeRuntime> CreateExecRuntime(const TSharedRef<const FVoxelExecNode>& SharedThis) const override;
};

class VOXELGRAPHNODES_API FVoxelMarchingCubePreviewExecNodeRuntime : public TVoxelExecNodeRuntime<FVoxelMarchingCubePreviewExecNode>
{
public:
	using Super::Super;

	//~ Begin FVoxelExecNodeRuntime Interface
	virtual void Create() override;
	virtual void Destroy() override;
	virtual FVoxelOptionalBox GetBounds() const override;
	//~ End FVoxelExecNodeRuntime Interface

private:
	TVoxelDynamicValue<FVoxelMarchingCubeBrushPreviewMesh> MeshValue;
	TWeakObjectPtr<UVoxelMeshComponent> WeakMeshComponent;
	TWeakObjectPtr<UVoxelCollisionComponent> WeakCollisionComponent;
};