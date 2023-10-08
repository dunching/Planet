// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Rendering/VoxelMesh.h"

DEFINE_VOXEL_MEMORY_STAT(STAT_VoxelMeshMemory);
DEFINE_VOXEL_MEMORY_STAT(STAT_VoxelGpuMeshMemory);

FVoxelMesh::~FVoxelMesh()
{
	ensure(bIsDestroyed_RenderThread || !bIsInitialized_RenderThread);
}

void FVoxelMesh::MarkRenderStateDirty_GameThread() const
{
	check(IsInGameThread());

	for (const TWeakObjectPtr<UPrimitiveComponent>& Component : Components)
	{
		if (ensure(Component.IsValid()))
		{
			Component->MarkRenderStateDirty();
		}
	}
}

void FVoxelMesh::CallInitialize_GameThread() const
{
	check(IsInGameThread());

	if (bIsInitialized_GameThread)
	{
		return;
	}
	bIsInitialized_GameThread = true;

	ConstCast(this)->Initialize_GameThread();

	UpdateStats();
	UpdateGpuStats();
}

void FVoxelMesh::CallInitialize_RenderThread(FRHICommandList& RHICmdList, ERHIFeatureLevel::Type FeatureLevel) const
{
	check(IsInRenderingThread());

	if (bIsInitialized_RenderThread)
	{
		return;
	}
	bIsInitialized_RenderThread = true;

	ConstCast(this)->Initialize_RenderThread(RHICmdList, FeatureLevel);

	UpdateStats();
	UpdateGpuStats();
}

TSharedRef<FVoxelMaterialRef> FVoxelMesh::GetMaterialSafe() const
{
	TSharedPtr<FVoxelMaterialRef> Material = GetMaterial();
	if (!Material)
	{
		Material = FVoxelMaterialRef::Default();
	}
	return Material.ToSharedRef();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelMeshComponentRef::FVoxelMeshComponentRef(UPrimitiveComponent* Component)
	: Component(Component)
{
	ensure(Component);
}

FVoxelMeshComponentRef::~FVoxelMeshComponentRef()
{
	SetMesh(nullptr);
}

void FVoxelMeshComponentRef::SetMesh(const TSharedPtr<const FVoxelMesh>& NewMesh)
{
	ensure(IsInGameThread());

	if (Mesh)
	{
		ensure(IsValid(Component));
		ensure(Mesh->Components.Remove(Component));
	}

	Mesh = NewMesh;

	if (Mesh)
	{
		ensure(!Mesh->Components.Contains(Component));
		Mesh->Components.Add(Component);
	}
}