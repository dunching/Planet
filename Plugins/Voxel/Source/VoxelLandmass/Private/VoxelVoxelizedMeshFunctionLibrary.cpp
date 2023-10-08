// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelVoxelizedMeshFunctionLibrary.h"
#include "VoxelVoxelizedMeshFunctionLibraryImpl.ispc.generated.h"
#include "VoxelPositionQueryParameter.h"

FVoxelVoxelizedMesh UVoxelVoxelizedMeshFunctionLibrary::MakeVoxelizedMeshFromStaticMesh(const FVoxelVoxelizedMeshStaticMesh& StaticMesh) const
{
	if (!StaticMesh.VoxelizedMesh.Data)
	{
		VOXEL_MESSAGE(Error, "{0}: mesh is null", this);
		return {};
	}

	return StaticMesh.VoxelizedMesh;
}

FVoxelSurface UVoxelVoxelizedMeshFunctionLibrary::CreateVoxelizedMeshSurface(
	const FVoxelVoxelizedMesh& Mesh,
	const bool bHermiteInterpolation) const
{
	const TSharedPtr<const FVoxelVoxelizedMeshData> MeshData = Mesh.Data;
	if (!MeshData)
	{
		VOXEL_MESSAGE(Error, "{0}: mesh is null", this);
		return {};
	}

	FVoxelSurface Surface = FVoxelSurface::MakeWithLocalBounds(
		GetNodeRef(),
		GetQuery(),
		MeshData->MeshBounds
		.Extend(1)
		.Scale(MeshData->VoxelSize));

	Surface.SetLocalDistance(GetQuery(), GetNodeRef(), [=, NodeRef = GetNodeRef()](const FVoxelQuery& Query)
	{
		return MakeVoxelFunctionCaller<UVoxelVoxelizedMeshFunctionLibrary>(NodeRef, Query)->CreateVoxelizedMeshSurface_Distance(MeshData.ToSharedRef(), bHermiteInterpolation);
	});

	return Surface;
}

FVoxelFloatBuffer UVoxelVoxelizedMeshFunctionLibrary::CreateVoxelizedMeshSurface_Distance(
	const TSharedRef<const FVoxelVoxelizedMeshData>& MeshData,
	const bool bHermiteInterpolation) const
{
	VOXEL_FUNCTION_COUNTER();
	FindVoxelQueryParameter_Function(FVoxelPositionQueryParameter, PositionQueryParameter);

	const FVoxelVectorBuffer Positions = PositionQueryParameter->GetPositions();
	const FIntVector Size = MeshData->Size;
	check(MeshData->DistanceField.Num() == Size.X * Size.Y * Size.Z);
	check(MeshData->Normals.Num() == Size.X * Size.Y * Size.Z);

	FVoxelFloatBufferStorage Distance;
	Distance.Allocate(Positions.Num());

	ForeachVoxelBufferChunk(Positions.Num(), [&](const FVoxelBufferIterator& Iterator)
	{
		ispc::VoxelVoxelizedMeshFunctionLibrary_SampleVoxelizedMesh(
			Positions.X.GetData(Iterator),
			Positions.Y.GetData(Iterator),
			Positions.Z.GetData(Iterator),
			Positions.X.IsConstant(),
			Positions.Y.IsConstant(),
			Positions.Z.IsConstant(),
			Iterator.Num(),
			GetISPCValue(Size),
			MeshData->DistanceField.GetData(),
			MeshData->Normals.GetData(),
			GetISPCValue(MeshData->Origin),
			MeshData->VoxelSize,
			bHermiteInterpolation,
			Distance.GetData(Iterator));
	});

	return FVoxelFloatBuffer::Make(Distance);
}