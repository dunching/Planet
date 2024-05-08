// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelMeshVoxelizerLibrary.h"
#include "VoxelMeshVoxelizer.h"
#include "VoxelDistanceFieldUtilities_Old.h"
#include "Engine/StaticMesh.h"
#include "StaticMeshResources.h"

FVoxelMeshVoxelizerInputData UVoxelMeshVoxelizerLibrary::CreateMeshDataFromStaticMesh(const UStaticMesh& StaticMesh)
{
	VOXEL_FUNCTION_COUNTER();

	const FStaticMeshRenderData* RenderData = StaticMesh.GetRenderData();
	if (!ensure(RenderData) ||
		!ensure(RenderData->LODResources.IsValidIndex(0)))
	{
		return {};
	}

	const FStaticMeshLODResources& LODResources = RenderData->LODResources[0];
	const FRawStaticIndexBuffer& IndexBuffer = LODResources.IndexBuffer;
	const FPositionVertexBuffer& PositionVertexBuffer = LODResources.VertexBuffers.PositionVertexBuffer;
	const FStaticMeshVertexBuffer& StaticMeshVertexBuffer = LODResources.VertexBuffers.StaticMeshVertexBuffer;

	FVoxelMeshVoxelizerInputData Data;
	FVoxelUtilities::SetNumFast(Data.Vertices, PositionVertexBuffer.GetNumVertices());
	FVoxelUtilities::SetNumFast(Data.Indices, IndexBuffer.GetNumIndices());
	FVoxelUtilities::SetNumFast(Data.VertexNormals, StaticMeshVertexBuffer.GetNumVertices());

	if (!FPlatformProperties::RequiresCookedData() ||
		StaticMesh.bAllowCPUAccess ||
		UE_SERVER)
	{
		// Read from CPU

		VOXEL_SCOPE_COUNTER("Copy CPU data");

		for (uint32 Index = 0; Index < PositionVertexBuffer.GetNumVertices(); Index++)
		{
			Data.Vertices[Index] = PositionVertexBuffer.VertexPosition(Index);
		}
		for (int32 Index = 0; Index < IndexBuffer.GetNumIndices(); Index++)
		{
			Data.Indices[Index] = IndexBuffer.GetIndex(Index);
		}
		for (uint32 Index = 0; Index < StaticMeshVertexBuffer.GetNumVertices(); Index++)
		{
			Data.VertexNormals[Index] = StaticMeshVertexBuffer.VertexTangentZ(Index);
		}

		return Data;
	}

	VOXEL_SCOPE_COUNTER("Copy GPU data");

	LOG_VOXEL(Log, "Extracting mesh data from GPU for %s", *StaticMesh.GetName());

	VOXEL_ENQUEUE_RENDER_COMMAND(GetMergedSectionFromStaticMesh)([&](FRHICommandListImmediate& RHICmdList)
	{
		{
			VOXEL_SCOPE_COUNTER("Copy Vertices from GPU");
			const int32 NumBytes = PositionVertexBuffer.GetNumVertices() * PositionVertexBuffer.GetStride();

			const void* BufferData = RHICmdList.LockBuffer(PositionVertexBuffer.VertexBufferRHI, 0, NumBytes, RLM_ReadOnly);
			FMemory::Memcpy(Data.Vertices.GetData(), BufferData, NumBytes);
			RHICmdList.UnlockBuffer(PositionVertexBuffer.VertexBufferRHI);
		}

		{
			VOXEL_SCOPE_COUNTER("Copy Triangles from GPU");

			const bool bIs32Bit = IndexBuffer.Is32Bit();
			const int32 NumBytes = IndexBuffer.GetNumIndices() * (bIs32Bit ? sizeof(uint32) : sizeof(uint16));

			const void* BufferData = RHICmdList.LockBuffer(IndexBuffer.IndexBufferRHI, 0, NumBytes, EResourceLockMode::RLM_ReadOnly);
			if (bIs32Bit)
			{
				FMemory::Memcpy(Data.Indices.GetData(), BufferData, NumBytes);
			}
			else
			{
				TVoxelArray<uint16> Indices16;
				Indices16.SetNumUninitialized(IndexBuffer.GetNumIndices());
				FMemory::Memcpy(Indices16.GetData(), BufferData, NumBytes);
				for (int32 Index = 0; Index < Indices16.Num(); Index++)
				{
					Data.Indices[Index] = Indices16[Index];
				}
			}
			RHICmdList.UnlockBuffer(IndexBuffer.IndexBufferRHI);
		}

		{
			VOXEL_SCOPE_COUNTER("Copy Vertex Normals from GPU");

			if (StaticMeshVertexBuffer.GetUseHighPrecisionTangentBasis())
			{
				using FTangentType =
					TStaticMeshVertexTangentDatum<
					TStaticMeshVertexTangentTypeSelector<
					EStaticMeshVertexTangentBasisType::HighPrecision>::TangentTypeT>;

				const void* BufferData = RHICmdList.LockBuffer(
					StaticMeshVertexBuffer.TangentsVertexBuffer.VertexBufferRHI,
					0,
					StaticMeshVertexBuffer.GetNumVertices() * sizeof(FTangentType),
					RLM_ReadOnly);

				for (uint32 Index = 0; Index < StaticMeshVertexBuffer.GetNumVertices(); Index++)
				{
					Data.VertexNormals[Index] = static_cast<const FTangentType*>(BufferData)[Index].GetTangentZ();
				}

				RHICmdList.UnlockBuffer(PositionVertexBuffer.VertexBufferRHI);
			}
			else
			{
				using FTangentType =
					TStaticMeshVertexTangentDatum<
					TStaticMeshVertexTangentTypeSelector<
					EStaticMeshVertexTangentBasisType::Default>::TangentTypeT>;

				const void* BufferData = RHICmdList.LockBuffer(
					StaticMeshVertexBuffer.TangentsVertexBuffer.VertexBufferRHI,
					0,
					StaticMeshVertexBuffer.GetNumVertices() * sizeof(FTangentType),
					RLM_ReadOnly);

				for (uint32 Index = 0; Index < StaticMeshVertexBuffer.GetNumVertices(); Index++)
				{
					Data.VertexNormals[Index] = static_cast<const FTangentType*>(BufferData)[Index].GetTangentZ();
				}

				RHICmdList.UnlockBuffer(PositionVertexBuffer.VertexBufferRHI);
			}
		}
	});

	FRenderCommandFence Fence;
	Fence.BeginFence();
	Fence.Wait();

	return Data;
}

bool UVoxelMeshVoxelizerLibrary::VoxelizeMesh(
	const FVoxelMeshVoxelizerInputData& Mesh,
	const FTransform& Transform,
	const FVoxelMeshVoxelizerSettings& Settings,
	// Needed if we want a smooth import, in voxels
	const float BoxExtension,
	TVoxelArray<float>& OutDistances,
	TVoxelArray<FVector3f>& OutSurfacePositions,
	FIntVector& OutSize,
	FIntVector& OutOffset,
	int32& OutNumLeaks,
	const bool bMultiThreaded)
{
	VOXEL_FUNCTION_COUNTER();

	TVoxelArray<FVector3f> Vertices;
	Vertices.Reserve(Mesh.Vertices.Num());
	FBox3f Box(ForceInit);
	for (const FVector3f& Vertex : Mesh.Vertices)
	{
		const FVector3f NewVertex = FTransform3f(Transform).TransformPosition(Vertex);
		Vertices.Add(NewVertex);
		Box += NewVertex;
	}
	Box = Box.ExpandBy(BoxExtension);

	const FVector3f SizeFloat = Box.GetSize();
	const FIntVector Size(FVoxelUtilities::CeilToInt(SizeFloat));
	const FVector3f Origin = Box.Min;

	if (int64(Size.X) * int64(Size.Y) * int64(Size.Z) >= MAX_int32)
	{
		VOXEL_MESSAGE(Error, "Voxelized mesh would have more than 2B voxels! Aborting");
		return false;
	}
	if (Size.X * Size.Y * Size.Z == 0)
	{
		VOXEL_MESSAGE(Error, "Size = 0! Aborting");
		return false;
	}

	Voxel::MeshVoxelizer::Voxelize(
		Settings,
		Vertices,
		Mesh.Indices,
		Origin,
		Size,
		OutDistances,
		OutSurfacePositions,
		nullptr,
		nullptr,
		&OutNumLeaks);

	OutSize = Size;
	OutOffset = FVoxelUtilities::RoundToInt(Box.Min);

	// Propagate distances
	FVoxelDistanceFieldUtilities::JumpFlood(Size, OutSurfacePositions, bMultiThreaded);
	FVoxelDistanceFieldUtilities::GetDistancesFromSurfacePositions(Size, OutSurfacePositions, OutDistances);

	return true;
}