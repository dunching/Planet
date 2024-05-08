// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Buffer/VoxelStaticMeshBuffer.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"

FVoxelFastCriticalSection GVoxelStaticMesh_CriticalSection;
TMap<FObjectKey, FVoxelStaticMesh::FMeshInfo> GVoxelStaticMesh_MeshToMeshInfo;

FVoxelStaticMesh::FMeshInfo FVoxelStaticMesh::GetMeshInfo() const
{
	if (StaticMesh.IsExplicitlyNull())
	{
		return {};
	}

	VOXEL_SCOPE_LOCK(GVoxelStaticMesh_CriticalSection);

	const FMeshInfo* Info = GVoxelStaticMesh_MeshToMeshInfo.Find(MakeObjectKey(StaticMesh));
	if (!ensure(Info))
	{
		return {};
	}
	return *Info;
}

// See UHierarchicalInstancedStaticMeshComponent::DesiredInstancesPerLeaf
static int32 GetDesiredInstancesPerLeaf(const UStaticMesh& StaticMesh)
{
	ensure(IsInGameThread());

	if (!StaticMesh.HasValidRenderData(true, 0))
	{
		return 16;
	}

	static IConsoleVariable* CVarMinVertsToSplitNode = IConsoleManager::Get().FindConsoleVariable(TEXT("foliage.MinVertsToSplitNode"));
	check(CVarMinVertsToSplitNode);

	return FMath::Clamp(CVarMinVertsToSplitNode->GetInt() / StaticMesh.GetNumVertices(0), 1, 1024);
}

FVoxelStaticMesh FVoxelStaticMesh::Make(UStaticMesh* Mesh)
{
	FVoxelStaticMesh Result;
	Result.StaticMesh = Mesh;

	if (Mesh)
	{
		FMeshInfo MeshInfo;
		MeshInfo.MeshBox = Mesh->GetBounds().GetBox();
		MeshInfo.DesiredInstancesPerLeaf = GetDesiredInstancesPerLeaf(*Mesh);

		VOXEL_SCOPE_LOCK(GVoxelStaticMesh_CriticalSection);
		GVoxelStaticMesh_MeshToMeshInfo.FindOrAdd(Mesh) = MeshInfo;
	}

	return Result;
}