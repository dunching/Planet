// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelMesh.generated.h"

class FRayTracingGeometry;
class FCardRepresentationData;
class FDistanceFieldVolumeData;

DECLARE_VOXEL_MEMORY_STAT(VOXELCORE_API, STAT_VoxelMeshMemory, "Voxel Mesh Memory");
DECLARE_VOXEL_MEMORY_STAT(VOXELCORE_API, STAT_VoxelGpuMeshMemory, "Voxel Mesh Memory (GPU)");

USTRUCT()
struct VOXELCORE_API FVoxelMesh
	: public FVoxelVirtualStruct
	, public TSharedFromThis<FVoxelMesh>
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

public:
	FVoxelMesh() = default;
	virtual ~FVoxelMesh() override;
	UE_NONCOPYABLE_STRUCT(FVoxelMesh);

	VOXEL_ALLOCATED_SIZE_TRACKER(STAT_VoxelMeshMemory);
	VOXEL_ALLOCATED_SIZE_GPU_TRACKER(STAT_VoxelGpuMeshMemory);

	void MarkRenderStateDirty_GameThread() const;

	void CallInitialize_GameThread() const;
	void CallInitialize_RenderThread(FRHICommandList& RHICmdList, ERHIFeatureLevel::Type FeatureLevel) const;

	TSharedRef<FVoxelMaterialRef> GetMaterialSafe() const;

public:
	virtual FVoxelBox GetBounds() const VOXEL_PURE_VIRTUAL({});
	virtual int64 GetAllocatedSize() const VOXEL_PURE_VIRTUAL({});
	virtual int64 GetGpuAllocatedSize() const VOXEL_PURE_VIRTUAL({});
	virtual TSharedPtr<FVoxelMaterialRef> GetMaterial() const VOXEL_PURE_VIRTUAL({});

	virtual bool Draw_RenderThread(const FPrimitiveSceneProxy& Proxy, FMeshBatch& MeshBatch) const { return false; }
	virtual const FRayTracingGeometry* DrawRaytracing_RenderThread(const FPrimitiveSceneProxy& Proxy, FMeshBatch& MeshBatch) const { return nullptr; }

	virtual const FCardRepresentationData* GetCardRepresentationData() const { return nullptr; }
	virtual const FDistanceFieldVolumeData* GetDistanceFieldVolumeData() const { return nullptr; }

	virtual bool ShouldDrawVelocity() const { return true; }

protected:
	virtual void Initialize_GameThread() {}
	virtual void Initialize_RenderThread(FRHICommandList& RHICmdList, ERHIFeatureLevel::Type FeatureLevel) {}
	virtual void Destroy_RenderThread() {}

	bool IsInitialized_GameThread() const
	{
		return bIsInitialized_GameThread;
	}
	bool IsInitialized_RenderThread() const
	{
		return bIsInitialized_RenderThread;
	}

private:
	mutable bool bIsInitialized_GameThread = false;
	mutable bool bIsInitialized_RenderThread = false;
	bool bIsDestroyed_RenderThread = false;

	mutable TSet<TWeakObjectPtr<UPrimitiveComponent>> Components;

	template<typename T>
	friend TSharedRef<T> MakeVoxelMesh();

	friend class FVoxelMeshComponentRef;
};

class VOXELCORE_API FVoxelMeshComponentRef
{
public:
	explicit FVoxelMeshComponentRef(UPrimitiveComponent* Component);
	~FVoxelMeshComponentRef();
	UE_NONCOPYABLE(FVoxelMeshComponentRef);

	void SetMesh(const TSharedPtr<const FVoxelMesh>& NewMesh);

	FORCEINLINE const TSharedPtr<const FVoxelMesh>& Get() const
	{
		return Mesh;
	}
	FORCEINLINE operator bool() const
	{
		return Mesh.operator bool();
	}
	FORCEINLINE const FVoxelMesh* operator->() const
	{
		return Mesh.operator->();
	}

private:
	UPrimitiveComponent* const Component;
	TSharedPtr<const FVoxelMesh> Mesh;
};

template<typename T>
TSharedRef<T> MakeVoxelMesh()
{
	const TSharedRef<T> Mesh = TSharedPtr<T>(new (GVoxelMemory) T(), [](T* Mesh)
	{
		if (!Mesh)
		{
			return;
		}

		VOXEL_ENQUEUE_RENDER_COMMAND(DeleteVoxelMesh)([=](FRHICommandListImmediate& RHICmdList)
		{
			if (Mesh->bIsInitialized_RenderThread &&
				ensure(!Mesh->bIsDestroyed_RenderThread))
			{
				Mesh->Destroy_RenderThread();
			}
			Mesh->bIsDestroyed_RenderThread = true;

			FVoxelMemory::Delete(Mesh);
		});
	}).ToSharedRef();

	SharedPointerInternals::EnableSharedFromThis(&Mesh, &Mesh.Get(), &Mesh.Get());

	return Mesh;
}