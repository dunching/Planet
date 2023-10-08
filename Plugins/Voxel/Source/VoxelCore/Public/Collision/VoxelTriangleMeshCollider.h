// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "Collision/VoxelCollider.h"
#include "Chaos/TriangleMeshImplicitObject.h"
#include "RawIndexBuffer.h"
#include "Rendering/ColorVertexBuffer.h"
#include "Rendering/PositionVertexBuffer.h"
#include "Rendering/StaticMeshVertexBuffer.h"
#include "VoxelTriangleMeshCollider.generated.h"

USTRUCT()
struct VOXELCORE_API FVoxelTriangleMeshCollider : public FVoxelCollider
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

	FVector Offset = FVector::ZeroVector;
	FVoxelBox LocalBounds;
	TSharedPtr<Chaos::FTriangleMeshImplicitObject> TriangleMesh;
	TArray<TWeakObjectPtr<UPhysicalMaterial>> PhysicalMaterials;

	virtual FVector GetOffset() const override { return Offset; }
	virtual FVoxelBox GetLocalBounds() const override { return LocalBounds.Extend(0.0001); }
	virtual int64 GetAllocatedSize() const override;
	virtual void AddToBodySetup(UBodySetup& BodySetup) const override;
	virtual TSharedPtr<const FVoxelNavmesh> GetNavmesh() const override;
	virtual TSharedPtr<IVoxelColliderRenderData> GetRenderData() const override;
	virtual TArray<TWeakObjectPtr<UPhysicalMaterial>> GetPhysicalMaterials() const override;

	virtual void BulkRaycast(
		bool bDebug,
		const TConstVoxelArrayView<FVector3f> LocalRayPositions,
		const TConstVoxelArrayView<FVector3f> LocalRayDirections,
		TVoxelArray<FVector3f>& OutLocalHitPositions,
		TVoxelArray<FVector3f>& OutLocalHitNormals) const override;
};

class VOXELCORE_API FVoxelTriangleMeshCollider_RenderData : public IVoxelColliderRenderData
{
public:
	explicit FVoxelTriangleMeshCollider_RenderData(const FVoxelTriangleMeshCollider& Collider);
	virtual ~FVoxelTriangleMeshCollider_RenderData() override;

	//~ Begin IVoxelColliderRenderData Interface
	virtual void Draw_RenderThread(const FPrimitiveSceneProxy& Proxy, FMeshBatch& MeshBatch) override;
	//~ End IVoxelColliderRenderData Interface

private:
	FRawStaticIndexBuffer IndexBuffer{ false };
	FPositionVertexBuffer PositionVertexBuffer;
	FStaticMeshVertexBuffer StaticMeshVertexBuffer;
	FColorVertexBuffer ColorVertexBuffer;
	TUniquePtr<FLocalVertexFactory> VertexFactory;
};