// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelCollider.generated.h"

struct FVoxelNavmesh;

DECLARE_VOXEL_MEMORY_STAT(VOXELCORE_API, STAT_VoxelColliderMemory, "Voxel Collider Memory");

class IVoxelColliderRenderData
{
public:
	IVoxelColliderRenderData() = default;
	virtual ~IVoxelColliderRenderData() = default;

	virtual void Draw_RenderThread(const FPrimitiveSceneProxy& Proxy, FMeshBatch& MeshBatch) = 0;
};

USTRUCT()
struct VOXELCORE_API FVoxelCollider : public FVoxelVirtualStruct
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

	VOXEL_ALLOCATED_SIZE_TRACKER(STAT_VoxelColliderMemory);

	virtual FVector GetOffset() const { return FVector::ZeroVector; }
	virtual FVoxelBox GetLocalBounds() const VOXEL_PURE_VIRTUAL({});
	virtual int64 GetAllocatedSize() const;
	virtual void AddToBodySetup(UBodySetup& BodySetup) const VOXEL_PURE_VIRTUAL();
	virtual TSharedPtr<const FVoxelNavmesh> GetNavmesh() const { return nullptr; }
	virtual TSharedPtr<IVoxelColliderRenderData> GetRenderData() const { return nullptr; }
	virtual TArray<TWeakObjectPtr<UPhysicalMaterial>> GetPhysicalMaterials() const { return {}; }

	virtual void BulkRaycast(
		bool bDebug,
		const TConstVoxelArrayView<FVector3f> LocalRayPositions,
		const TConstVoxelArrayView<FVector3f> LocalRayDirections,
		TVoxelArray<FVector3f>& OutLocalHitPositions,
		TVoxelArray<FVector3f>& OutLocalHitNormals) const VOXEL_PURE_VIRTUAL();
};