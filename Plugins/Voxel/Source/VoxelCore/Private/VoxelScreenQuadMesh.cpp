// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Rendering/VoxelScreenQuadMesh.h"
#include "LocalVertexFactory.h"
#include "StaticMeshResources.h"
#include "CommonRenderResources.h"

class FVoxelQuadMeshVertexBuffer : public FRenderResource
{
public:
	FStaticMeshVertexBuffers Buffers;

	FVoxelQuadMeshVertexBuffer()
	{
		TArray<FVector2f> UVs;
		UVs.Add(FVector2f(0.0f, 0.0f));
		UVs.Add(FVector2f(0.0f, 1.0f));
		UVs.Add(FVector2f(1.0f, 0.0f));
		UVs.Add(FVector2f(1.0f, 1.0f));

		Buffers.PositionVertexBuffer.Init(4);
		Buffers.StaticMeshVertexBuffer.Init(4, 1);

		for (int32 Index = 0; Index < 4; Index++)
		{
			Buffers.PositionVertexBuffer.VertexPosition(Index) = FVector3f::ZeroVector;
			Buffers.StaticMeshVertexBuffer.SetVertexTangents(Index, FVector3f::XAxisVector, FVector3f::YAxisVector, FVector3f::ZAxisVector);
			Buffers.StaticMeshVertexBuffer.SetVertexUV(Index, 0, UVs[Index]);
		}
	}

	virtual void InitRHI(UE_503_ONLY(FRHICommandListBase& RHICmdList)) override
	{
		Buffers.PositionVertexBuffer.InitResource(UE_503_ONLY(RHICmdList));
		Buffers.StaticMeshVertexBuffer.InitResource(UE_503_ONLY(RHICmdList));
	}

	virtual void ReleaseRHI() override
	{
		Buffers.PositionVertexBuffer.ReleaseResource();
		Buffers.StaticMeshVertexBuffer.ReleaseResource();
	}
};
TGlobalResource<FVoxelQuadMeshVertexBuffer> GVoxelQuadMeshVertexBuffer;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelBox FVoxelScreenQuadMesh::GetBounds() const
{
	return FVoxelBox(FVector(-1e6), FVector(1e6));
}

int64 FVoxelScreenQuadMesh::GetAllocatedSize() const
{
	return sizeof(*this);
}

int64 FVoxelScreenQuadMesh::GetGpuAllocatedSize() const
{
	return 0;
}

TSharedPtr<FVoxelMaterialRef> FVoxelScreenQuadMesh::GetMaterial() const
{
	return Material;
}

void FVoxelScreenQuadMesh::Initialize_RenderThread(FRHICommandList& RHICmdList, ERHIFeatureLevel::Type FeatureLevel)
{
	VOXEL_FUNCTION_COUNTER();
	check(IsInRenderingThread());

	check(!VertexFactory);
	VertexFactory = MakeVoxelShared<FLocalVertexFactory>(FeatureLevel, "FVoxelScreenQuadMesh");

	FLocalVertexFactory::FDataType Data;
	GVoxelQuadMeshVertexBuffer.Buffers.PositionVertexBuffer.BindPositionVertexBuffer(VertexFactory.Get(), Data);
	GVoxelQuadMeshVertexBuffer.Buffers.StaticMeshVertexBuffer.BindTangentVertexBuffer(VertexFactory.Get(), Data);
	GVoxelQuadMeshVertexBuffer.Buffers.StaticMeshVertexBuffer.BindPackedTexCoordVertexBuffer(VertexFactory.Get(), Data);
	GVoxelQuadMeshVertexBuffer.Buffers.ColorVertexBuffer.BindColorVertexBuffer(VertexFactory.Get(), Data);

	VertexFactory->SetData(Data);
	VOXEL_INLINE_COUNTER("VertexFactory", VertexFactory->InitResource(UE_503_ONLY(RHICmdList)));
}

void FVoxelScreenQuadMesh::Destroy_RenderThread()
{
	VOXEL_FUNCTION_COUNTER();
	check(IsInRenderingThread());

	check(VertexFactory);
	VertexFactory->ReleaseResource();
	VertexFactory.Reset();
}

bool FVoxelScreenQuadMesh::Draw_RenderThread(const FPrimitiveSceneProxy& Proxy, FMeshBatch& MeshBatch) const
{
	MeshBatch.CastShadow = 0;
	MeshBatch.VertexFactory = VertexFactory.Get();

	FMeshBatchElement& BatchElement = MeshBatch.Elements[0];
	BatchElement.IndexBuffer = &GScreenRectangleIndexBuffer;
	BatchElement.FirstIndex = 0;
	BatchElement.NumPrimitives = 2;
	BatchElement.MinVertexIndex = 0;
	BatchElement.MaxVertexIndex = 4;

	return true;
}