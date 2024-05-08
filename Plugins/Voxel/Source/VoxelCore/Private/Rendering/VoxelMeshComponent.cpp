// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Rendering/VoxelMeshComponent.h"
#include "Rendering/VoxelMeshSettings.h"
#include "Rendering/VoxelMeshSceneProxy.h"

VOXEL_CONSOLE_SINK(MeshComponentConsoleSink)
{
	ForEachObjectOfClass<UVoxelMeshComponent>([&](UVoxelMeshComponent* Component)
	{
		if (Component->GetMesh())
		{
			Component->MarkRenderStateDirty();
		}
	});
}

UVoxelMeshComponent::UVoxelMeshComponent()
{
	CastShadow = true;
	bUseAsOccluder = true;
}

void UVoxelMeshComponent::SetMesh(const TSharedPtr<const FVoxelMesh>& NewMesh)
{
	VOXEL_FUNCTION_COUNTER();
	check(IsInGameThread());
	ensure(IsRegistered());

	Mesh.SetMesh(NewMesh);

	if (Mesh)
	{
		Mesh->CallInitialize_GameThread();
	}

	MarkRenderStateDirty();

#if WITH_EDITOR
	FVoxelSystemUtilities::EnsureViewportIsUpToDate();
#endif
}

void UVoxelMeshComponent::ReturnToPool()
{
	VOXEL_FUNCTION_COUNTER();
	check(IsInGameThread());

	SetMesh(nullptr);
	FVoxelMeshSettings().ApplyToComponent(*this);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FPrimitiveSceneProxy* UVoxelMeshComponent::CreateSceneProxy()
{
	VOXEL_FUNCTION_COUNTER();

	if (!Mesh)
	{
		return nullptr;
	}

	return new FVoxelMeshSceneProxy(*this);
}

int32 UVoxelMeshComponent::GetNumMaterials() const
{
	return 1;
}

UMaterialInterface* UVoxelMeshComponent::GetMaterial(int32 Index) const
{
	if (!Mesh)
	{
		return nullptr;
	}

	return Mesh->GetMaterialSafe()->GetMaterial();
}

void UVoxelMeshComponent::GetUsedMaterials(TArray<UMaterialInterface*>& OutMaterials, bool bGetDebugMaterials) const
{
	if (!Mesh)
	{
		return;
	}

	OutMaterials.Add(Mesh->GetMaterialSafe()->GetMaterial());
}

FBoxSphereBounds UVoxelMeshComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	const FVoxelBox LocalBounds = Mesh ? Mesh->GetBounds().Extend(BoundsExtension) : FVoxelBox();
	ensure(LocalBounds.IsValid());
	return LocalBounds.TransformBy(LocalToWorld).ToFBox();
}

void UVoxelMeshComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	VOXEL_FUNCTION_COUNTER();

	Super::OnComponentDestroyed(bDestroyingHierarchy);

	// Clear memory
	Mesh.SetMesh(nullptr);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FMaterialRelevance UVoxelMeshComponent::GetMaterialRelevance(ERHIFeatureLevel::Type InFeatureLevel) const
{
	if (!Mesh)
	{
		return {};
	}

	const UMaterialInterface* Material = Mesh->GetMaterialSafe()->GetMaterial();
	if (!Material)
	{
		return {};
	}

	return Material->GetRelevance_Concurrent(InFeatureLevel);
}