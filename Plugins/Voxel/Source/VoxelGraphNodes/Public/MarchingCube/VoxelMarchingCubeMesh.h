// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VertexFactory.h"
#include "VoxelDetailTexture.h"
#include "VoxelMarchingCubeNodes.h"
#include "VoxelMarchingCubeMesh.generated.h"

class VOXELGRAPHNODES_API FVoxelMarchingCubeVertexFactoryShaderParameters : public FVertexFactoryShaderParameters
{
	DECLARE_TYPE_LAYOUT(FVoxelMarchingCubeVertexFactoryShaderParameters, NonVirtual);

public:
	void Bind(const FShaderParameterMap& ParameterMap);
	void GetElementShaderBindings(
		const FSceneInterface* Scene,
		const FSceneView* View,
		const FMeshMaterialShader* Shader,
		const EVertexInputStreamType InputStreamType,
		ERHIFeatureLevel::Type FeatureLevel,
		const FVertexFactory* VertexFactory,
		const FMeshBatchElement& BatchElement,
		FMeshDrawSingleShaderBindings& ShaderBindings,
		FVertexInputStreamArray& VertexStreams) const;

	LAYOUT_FIELD(FShaderParameter, VoxelSize);
	LAYOUT_FIELD(FShaderParameter, NumCells);
	LAYOUT_FIELD(FShaderResourceParameter, CellTextureCoordinates);
	LAYOUT_FIELD(FShaderResourceParameter, TextureSampler);

	LAYOUT_FIELD(FShaderResourceParameter, Normal_Texture);
	LAYOUT_FIELD(FShaderParameter, Normal_Texture_Size);
	LAYOUT_FIELD(FShaderParameter, Normal_Texture_InvSize);
	LAYOUT_FIELD(FShaderParameter, Normal_TextureSize);
	LAYOUT_FIELD(FShaderParameter, Normal_TextureIndex);

	LAYOUT_FIELD(FShaderResourceParameter, MaterialId_TextureA);
	LAYOUT_FIELD(FShaderResourceParameter, MaterialId_TextureB);
	LAYOUT_FIELD(FShaderParameter, MaterialId_Texture_Size);
	LAYOUT_FIELD(FShaderParameter, MaterialId_Texture_InvSize);
	LAYOUT_FIELD(FShaderParameter, MaterialId_TextureSize);
	LAYOUT_FIELD(FShaderParameter, MaterialId_TextureIndex);
};

class VOXELGRAPHNODES_API FVoxelMarchingCubeVertexFactoryBase : public FVertexFactory
{
public:
	float VoxelSize = 0;
	int32 NumCells = 0;
	FShaderResourceViewRHIRef CellTextureCoordinates;

	FTextureRHIRef Normal_Texture;
	float Normal_Texture_Size = 1;
	float Normal_Texture_InvSize = 1;
	float Normal_TextureSize = 0;
	int32 Normal_TextureIndex = 0;

	FTextureRHIRef MaterialId_TextureA;
	FTextureRHIRef MaterialId_TextureB;
	float MaterialId_Texture_Size = 1;
	float MaterialId_Texture_InvSize = 1;
	float MaterialId_TextureSize = 0;
	int32 MaterialId_TextureIndex = 0;

	FVertexStreamComponent PositionComponent;
	FVertexStreamComponent PrimitiveDataComponent;
	FVertexStreamComponent VertexNormalComponent;

	using FVertexFactory::FVertexFactory;

	VOXEL_COUNT_INSTANCES();

	static bool ShouldCompilePermutation(const FVertexFactoryShaderPermutationParameters& Parameters);
	static void ModifyCompilationEnvironment(const FVertexFactoryShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);
	static void GetPSOPrecacheVertexFetchElements(
		EVertexInputStreamType VertexInputStreamType,
		FVertexDeclarationElementList& Elements,
		bool bHasVertexNormals);

	virtual bool SupportsPositionOnlyStream() const override { return true; }
	virtual bool SupportsPositionAndNormalOnlyStream() const override { return true; }

	//~ Begin FRenderResource Interface
	virtual void InitRHI(UE_503_ONLY(FRHICommandListBase& RHICmdList)) override;
	virtual void ReleaseRHI() override;
	//~ End FRenderResource Interface
};

class VOXELGRAPHNODES_API FVoxelMarchingCubeVertexFactory_NoVertexNormals : public FVoxelMarchingCubeVertexFactoryBase
{
	DECLARE_VERTEX_FACTORY_TYPE(FVoxelMarchingCubeVertexFactory_NoVertexNormals);

public:
	using FVoxelMarchingCubeVertexFactoryBase::FVoxelMarchingCubeVertexFactoryBase;

	static void GetPSOPrecacheVertexFetchElements(
		EVertexInputStreamType VertexInputStreamType,
		FVertexDeclarationElementList& Elements)
	{
		FVoxelMarchingCubeVertexFactoryBase::GetPSOPrecacheVertexFetchElements(VertexInputStreamType, Elements, false);
	}
};

class VOXELGRAPHNODES_API FVoxelMarchingCubeVertexFactory_WithVertexNormals : public FVoxelMarchingCubeVertexFactoryBase
{
	DECLARE_VERTEX_FACTORY_TYPE(FVoxelMarchingCubeVertexFactory_WithVertexNormals);

public:
	using FVoxelMarchingCubeVertexFactoryBase::FVoxelMarchingCubeVertexFactoryBase;

	static void GetPSOPrecacheVertexFetchElements(
		EVertexInputStreamType VertexInputStreamType,
		FVertexDeclarationElementList& Elements)
	{
		FVoxelMarchingCubeVertexFactoryBase::GetPSOPrecacheVertexFetchElements(VertexInputStreamType, Elements, true);
	}
};

USTRUCT()
struct VOXELGRAPHNODES_API FVoxelMarchingCubeMesh : public FVoxelMesh
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

	int32 LOD = 0;
	FVoxelBox Bounds;
	float VoxelSize = 0.f;
	int32 ChunkSize = 0;
	int32 NumCells = 0;
	int32 NumEdgeVertices = 0;
	TSharedPtr<const FVoxelComputedMaterial> ComputedMaterial;

	bool bHasVertexNormals = false;

	TVoxelArray<int32> Indices;
	TVoxelArray<FVector3f> Vertices;
	TVoxelArray<FVector3f> VertexNormals;
	TVoxelArray<int32> CellIndices;

	using FTransitionIndex = FVoxelMarchingCubeSurface::FTransitionIndex;
	using FTransitionVertex = FVoxelMarchingCubeSurface::FTransitionVertex;

	TVoxelStaticArray<TVoxelArray<FTransitionIndex>, 6> TransitionIndices;
	TVoxelStaticArray<TVoxelArray<FTransitionVertex>, 6> TransitionVertices;
	TVoxelStaticArray<TVoxelArray<int32>, 6> TransitionCellIndices;

	TVoxelArray<uint8> CellIndexToDirection;
	TVoxelArray<FVoxelDetailTextureCoordinate> CellTextureCoordinates;

	TSharedPtr<FCardRepresentationData> CardRepresentationData;
	TSharedPtr<FDistanceFieldVolumeData> DistanceFieldVolumeData;

	void SetTransitionMask_GameThread(uint8 NewTransitionMask);
	void SetTransitionMask_RenderThread(FRHICommandList& RHICmdList, uint8 NewTransitionMask);

	virtual FVoxelBox GetBounds() const override { return Bounds; }
	virtual int64 GetAllocatedSize() const override;
	virtual int64 GetGpuAllocatedSize() const override;
	virtual TSharedPtr<FVoxelMaterialRef> GetMaterial() const override;

	virtual void Initialize_GameThread() override;

	virtual void Initialize_RenderThread(FRHICommandList& RHICmdList, ERHIFeatureLevel::Type FeatureLevel) override;
	virtual void Destroy_RenderThread() override;

	virtual bool Draw_RenderThread(const FPrimitiveSceneProxy& Proxy, FMeshBatch& MeshBatch) const override;

	virtual const FCardRepresentationData* GetCardRepresentationData() const override
	{
		return CardRepresentationData.Get();
	}
	virtual const FDistanceFieldVolumeData* GetDistanceFieldVolumeData() const override
	{
		return DistanceFieldVolumeData.Get();
	}

private:
	int32 NumIndicesToRender = 0;
	int32 NumVerticesToRender = 0;
	uint8 TransitionMask = 0;

	TSharedPtr<FIndexBuffer> IndicesBuffer;
	TSharedPtr<FVertexBuffer> VerticesBuffer;
	TSharedPtr<FVertexBuffer> PrimitivesDataBuffer;
	TSharedPtr<FVertexBuffer> VertexNormalsBuffer;

	TSharedPtr<FVertexBufferWithSRV> CellTextureCoordinatesBuffer;

	TSharedPtr<FVoxelMarchingCubeVertexFactoryBase> VertexFactory;
	TSharedPtr<FVoxelMaterialRef> Material;
};