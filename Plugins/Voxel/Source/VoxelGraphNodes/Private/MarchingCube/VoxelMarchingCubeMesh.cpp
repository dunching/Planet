// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "MarchingCube/VoxelMarchingCubeMesh.h"
#include "Rendering/VoxelShaderHooks.h"

#include "Engine/Engine.h"
#include "Engine/Texture2D.h"
#include "Materials/Material.h"
#include "MeshBatch.h"
#include "RHIStaticStates.h"
#include "TextureResource.h"
#include "PrimitiveSceneProxy.h"
#include "MeshMaterialShader.h"
#include "MeshDrawShaderBindings.h"
#include "GlobalRenderResources.h"
#include "DataDrivenShaderPlatformInfo.h"

DEFINE_VOXEL_INSTANCE_COUNTER(FVoxelMarchingCubeVertexFactoryBase);

DEFINE_VOXEL_SHADER_HOOK(
	VoxelMarchingCubeVertexFactory,
	MaterialVertexParameters,
	R"(
	#if VOXEL_MARCHING_CUBE_VERTEX_FACTORY
		uint VoxelDetailTexture_CellIndex;
		float2 VoxelDetailTexture_Delta;
		float3 VoxelDetailTexture_MaterialIds;
		float2 VoxelDetailTexture_LerpAlphas;
	#define VERTEX_PARAMETERS_HAS_VoxelDetailTexture_CellIndex 1
	#define VERTEX_PARAMETERS_HAS_VoxelDetailTexture_Delta 1
	#define VERTEX_PARAMETERS_HAS_VoxelDetailTexture_MaterialIds_LerpAlphas 1
	#endif
	)");

DEFINE_VOXEL_SHADER_HOOK(
	VoxelMarchingCubeVertexFactory,
	MaterialPixelParameters,
	R"(
	#if VOXEL_MARCHING_CUBE_VERTEX_FACTORY
		uint VoxelDetailTexture_CellIndex;
		float2 VoxelDetailTexture_Delta;
		float3 VoxelDetailTexture_MaterialIds;
		float2 VoxelDetailTexture_LerpAlphas;
	#define PIXEL_PARAMETERS_HAS_VoxelDetailTexture_CellIndex 1
	#define PIXEL_PARAMETERS_HAS_VoxelDetailTexture_Delta 1
	#define PIXEL_PARAMETERS_HAS_VoxelDetailTexture_MaterialIds_LerpAlphas 1
	#endif
	)");

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelMarchingCubeVertexFactoryShaderParameters::Bind(const FShaderParameterMap& ParameterMap)
{
#define BIND(Name) Name.Bind(ParameterMap, TEXT(#Name))
	BIND(VoxelSize);
	BIND(NumCells);
	BIND(CellTextureCoordinates);
	BIND(TextureSampler);

	BIND(Normal_Texture);
	BIND(Normal_Texture_Size);
	BIND(Normal_Texture_InvSize);
	BIND(Normal_TextureSize);
	BIND(Normal_TextureIndex);

	BIND(MaterialId_TextureA);
	BIND(MaterialId_TextureB);
	BIND(MaterialId_Texture_Size);
	BIND(MaterialId_Texture_InvSize);
	BIND(MaterialId_TextureSize);
	BIND(MaterialId_TextureIndex);
#undef BIND
}

void FVoxelMarchingCubeVertexFactoryShaderParameters::GetElementShaderBindings(
	const FSceneInterface* Scene,
	const FSceneView* View,
	const FMeshMaterialShader* Shader,
	const EVertexInputStreamType InputStreamType,
	ERHIFeatureLevel::Type FeatureLevel,
	const FVertexFactory* VertexFactory,
	const FMeshBatchElement& BatchElement,
	FMeshDrawSingleShaderBindings& ShaderBindings,
	FVertexInputStreamArray& VertexStreams) const
{
	const FVoxelMarchingCubeVertexFactoryBase& VoxelVertexFactory = static_cast<const FVoxelMarchingCubeVertexFactoryBase&>(*VertexFactory);

	ShaderBindings.Add(VoxelSize, VoxelVertexFactory.VoxelSize);
	ShaderBindings.Add(NumCells, VoxelVertexFactory.NumCells);

	ShaderBindings.Add(
		CellTextureCoordinates,
		VoxelVertexFactory.CellTextureCoordinates ? VoxelVertexFactory.CellTextureCoordinates : GBlackTextureWithSRV->ShaderResourceViewRHI);

	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////

	ShaderBindings.AddTexture(
		Normal_Texture,
		TextureSampler,
		TStaticSamplerState<SF_Bilinear>::GetRHI(),
		VoxelVertexFactory.Normal_Texture ? VoxelVertexFactory.Normal_Texture : GBlackTexture->TextureRHI);

	ShaderBindings.Add(Normal_Texture_Size, VoxelVertexFactory.Normal_Texture_Size);
	ShaderBindings.Add(Normal_Texture_InvSize, VoxelVertexFactory.Normal_Texture_InvSize);
	ShaderBindings.Add(Normal_TextureSize, VoxelVertexFactory.Normal_TextureSize);
	ShaderBindings.Add(Normal_TextureIndex, VoxelVertexFactory.Normal_TextureIndex);

	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////

	ShaderBindings.AddTexture(
		MaterialId_TextureA,
		TextureSampler,
		TStaticSamplerState<SF_Bilinear>::GetRHI(),
		VoxelVertexFactory.MaterialId_TextureA ? VoxelVertexFactory.MaterialId_TextureA : GBlackTexture->TextureRHI);
	ShaderBindings.AddTexture(
		MaterialId_TextureB,
		TextureSampler,
		TStaticSamplerState<SF_Bilinear>::GetRHI(),
		VoxelVertexFactory.MaterialId_TextureB ? VoxelVertexFactory.MaterialId_TextureB : GBlackTexture->TextureRHI);

	ShaderBindings.Add(MaterialId_Texture_Size, VoxelVertexFactory.MaterialId_Texture_Size);
	ShaderBindings.Add(MaterialId_Texture_InvSize, VoxelVertexFactory.MaterialId_Texture_InvSize);
	ShaderBindings.Add(MaterialId_TextureSize, VoxelVertexFactory.MaterialId_TextureSize);
	ShaderBindings.Add(MaterialId_TextureIndex, VoxelVertexFactory.MaterialId_TextureIndex);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool FVoxelMarchingCubeVertexFactoryBase::ShouldCompilePermutation(const FVertexFactoryShaderPermutationParameters& Parameters)
{
	if (Parameters.MaterialParameters.bIsSpecialEngineMaterial)
	{
		return true;
	}

	return
		IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5) &&
		Parameters.MaterialParameters.MaterialDomain == MD_Surface;
}

void FVoxelMarchingCubeVertexFactoryBase::ModifyCompilationEnvironment(const FVertexFactoryShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	OutEnvironment.SetDefine(TEXT("VF_SUPPORTS_PRIMITIVE_SCENE_DATA"),
		UseGPUScene(
			Parameters.Platform,
			GetMaxSupportedFeatureLevel(Parameters.Platform)));

	OutEnvironment.SetDefine(TEXT("VOXEL_ENGINE_VERSION"), VOXEL_ENGINE_VERSION);
	OutEnvironment.SetDefine(TEXT("VOXEL_MARCHING_CUBE_VERTEX_FACTORY"), TEXT("1"));

	if (Parameters.VertexFactoryType == &FVoxelMarchingCubeVertexFactory_NoVertexNormals::StaticType)
	{
		OutEnvironment.SetDefine(TEXT("WITH_VERTEX_NORMALS"), TEXT("0"));
	}
	else
	{
		ensure(Parameters.VertexFactoryType == &FVoxelMarchingCubeVertexFactory_WithVertexNormals::StaticType);
		OutEnvironment.SetDefine(TEXT("WITH_VERTEX_NORMALS"), TEXT("1"));
	}
}

void FVoxelMarchingCubeVertexFactoryBase::GetPSOPrecacheVertexFetchElements(
	const EVertexInputStreamType VertexInputStreamType,
	FVertexDeclarationElementList& Elements,
	const bool bHasVertexNormals)
{
	Elements.Add(FVertexElement(0, 0, VET_Float3, 0, 0, false));
	Elements.Add(FVertexElement(1, 0, VET_UInt, 1, 0, false));
	Elements.Add(FVertexElement(2, 0, VET_UInt, 2, 0, true));

	if (bHasVertexNormals)
	{
		Elements.Add(FVertexElement(3, 0, VET_Float3, 3, 0, false));
	}
}

void FVoxelMarchingCubeVertexFactoryBase::InitRHI(UE_503_ONLY(FRHICommandListBase& RHICmdList))
{
	FVertexFactory::InitRHI(UE_503_ONLY(RHICmdList));

	const auto AddDeclaration = [&](const EVertexInputStreamType Type)
	{
		FVertexDeclarationElementList Elements;
		Elements.Add(AccessStreamComponent(PositionComponent, 0, Type));
		Elements.Add(AccessStreamComponent(PrimitiveDataComponent, 1, Type));

		AddPrimitiveIdStreamElement(Type, Elements, 2, 0xFF);

		if (GetType() == &FVoxelMarchingCubeVertexFactory_WithVertexNormals::StaticType)
		{
			Elements.Add(AccessStreamComponent(VertexNormalComponent, 3, Type));
		}

		InitDeclaration(Elements, Type);
	};

	AddDeclaration(EVertexInputStreamType::Default);
	AddDeclaration(EVertexInputStreamType::PositionOnly);
	AddDeclaration(EVertexInputStreamType::PositionAndNormalOnly);
}

void FVoxelMarchingCubeVertexFactoryBase::ReleaseRHI()
{
	FVertexFactory::ReleaseRHI();
}

IMPLEMENT_TYPE_LAYOUT(FVoxelMarchingCubeVertexFactoryShaderParameters);
IMPLEMENT_VERTEX_FACTORY_PARAMETER_TYPE(FVoxelMarchingCubeVertexFactory_NoVertexNormals, SF_Pixel, FVoxelMarchingCubeVertexFactoryShaderParameters);
IMPLEMENT_VERTEX_FACTORY_PARAMETER_TYPE(FVoxelMarchingCubeVertexFactory_NoVertexNormals, SF_Vertex, FVoxelMarchingCubeVertexFactoryShaderParameters);
IMPLEMENT_VERTEX_FACTORY_PARAMETER_TYPE(FVoxelMarchingCubeVertexFactory_WithVertexNormals, SF_Pixel, FVoxelMarchingCubeVertexFactoryShaderParameters);
IMPLEMENT_VERTEX_FACTORY_PARAMETER_TYPE(FVoxelMarchingCubeVertexFactory_WithVertexNormals, SF_Vertex, FVoxelMarchingCubeVertexFactoryShaderParameters);

IMPLEMENT_VERTEX_FACTORY_TYPE(FVoxelMarchingCubeVertexFactory_NoVertexNormals, "/Plugin/Voxel/VoxelMarchingCubeVertexFactory.ush",
	EVertexFactoryFlags::UsedWithMaterials |
	EVertexFactoryFlags::SupportsDynamicLighting |
	EVertexFactoryFlags::SupportsPositionOnly |
	EVertexFactoryFlags::SupportsCachingMeshDrawCommands |
	EVertexFactoryFlags::SupportsPrimitiveIdStream |
	EVertexFactoryFlags::SupportsPSOPrecaching);

IMPLEMENT_VERTEX_FACTORY_TYPE(FVoxelMarchingCubeVertexFactory_WithVertexNormals, "/Plugin/Voxel/VoxelMarchingCubeVertexFactory.ush",
	EVertexFactoryFlags::UsedWithMaterials |
	EVertexFactoryFlags::SupportsDynamicLighting |
	EVertexFactoryFlags::SupportsPositionOnly |
	EVertexFactoryFlags::SupportsCachingMeshDrawCommands |
	EVertexFactoryFlags::SupportsPrimitiveIdStream |
	EVertexFactoryFlags::SupportsPSOPrecaching);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelMarchingCubeMesh::SetTransitionMask_GameThread(uint8 NewTransitionMask)
{
	VOXEL_ENQUEUE_RENDER_COMMAND(SetTransitionMask_RenderThread)(MakeWeakPtrLambda(this, [this, NewTransitionMask](FRHICommandList& RHICmdList)
	{
		SetTransitionMask_RenderThread(RHICmdList, NewTransitionMask);
	}));

	// Clear static draws as buffers will be invalidated
	MarkRenderStateDirty_GameThread();
}

void FVoxelMarchingCubeMesh::SetTransitionMask_RenderThread(FRHICommandList& RHICmdList, const uint8 NewTransitionMask)
{
	VOXEL_FUNCTION_COUNTER();
	ensure(IsInRenderingThread());

	if (!IsInitialized_RenderThread())
	{
		TransitionMask = NewTransitionMask;
		return;
	}

	if (TransitionMask == NewTransitionMask &&
		VertexFactory->IsInitialized())
	{
		return;
	}

	TransitionMask = NewTransitionMask;

	TVoxelArray<int32> NewIndices;
	TVoxelArray<FVector3f> NewVertices;
	TVoxelArray<int32> NewCellIndices;
	TVoxelArray<FVector3f> NewVertexNormals;
	{
		int32 NumIndices = 0;
		int32 NumVertices = 0;
		for (int32 Direction = 0; Direction < 6; Direction++)
		{
			if (!(TransitionMask & (1 << Direction)))
			{
				continue;
			}

			NumIndices += TransitionIndices[Direction].Num();
			NumVertices += TransitionVertices[Direction].Num();
		}
		NewIndices.Reserve(NumIndices);
		NewVertices.Reserve(2 * NumVertices);

		ensure(NumIndices % 3 == 0);
		NewCellIndices.Reserve(NumIndices / 3);

		if (bHasVertexNormals)
		{
			NewVertexNormals.Reserve(2 * NumVertices);
		}
	}

	NewIndices.Append(Indices);
	NewVertices.Append(Vertices);
	NewCellIndices.Append(CellIndices);

	if (bHasVertexNormals)
	{
		NewVertexNormals.Append(VertexNormals);
	}

	for (int32 Direction = 0; Direction < 6; Direction++)
	{
		if (!(TransitionMask & (1 << Direction)))
		{
			continue;
		}

		const int32 Offset = NewVertices.Num();

		for (const FTransitionIndex& TransitionIndex : TransitionIndices[Direction])
		{
			int32 Index = TransitionIndex.Index;
			if (TransitionIndex.bIsRelative)
			{
				Index += Offset;
			}
			NewIndices.Add(Index);
		}

		for (const FTransitionVertex& TransitionVertex : TransitionVertices[Direction])
		{
			NewVertices.Add(TransitionVertex.Position);
		}

		NewCellIndices.Append(TransitionCellIndices[Direction]);

		if (bHasVertexNormals)
		{
			for (const FTransitionVertex& TransitionVertex : TransitionVertices[Direction])
			{
				NewVertexNormals.Add(VertexNormals[TransitionVertex.SourceVertex]);
			}
		}
	}

	if (TransitionMask != 0)
	{
		VOXEL_SCOPE_COUNTER("Translate vertices");

		const float LowerBound = 1;
		const float UpperBound = ChunkSize - 1;

		ensure(VertexNormals.Num() == NumEdgeVertices || VertexNormals.Num() == Vertices.Num());
		for (int32 Index = 0; Index < NumEdgeVertices; Index++)
		{
			FVector3f& Vertex = NewVertices[Index];

			if ((LowerBound <= Vertex.X && Vertex.X <= UpperBound) &&
				(LowerBound <= Vertex.Y && Vertex.Y <= UpperBound) &&
				(LowerBound <= Vertex.Z && Vertex.Z <= UpperBound))
			{
				// Fast exit
				continue;
			}

			constexpr uint8 XMin = 0x01;
			constexpr uint8 XMax = 0x02;
			constexpr uint8 YMin = 0x04;
			constexpr uint8 YMax = 0x08;
			constexpr uint8 ZMin = 0x10;
			constexpr uint8 ZMax = 0x20;

			if ((Vertex.X == 0.f && !(TransitionMask & XMin)) || (Vertex.X == ChunkSize && !(TransitionMask & XMax)) ||
				(Vertex.Y == 0.f && !(TransitionMask & YMin)) || (Vertex.Y == ChunkSize && !(TransitionMask & YMax)) ||
				(Vertex.Z == 0.f && !(TransitionMask & ZMin)) || (Vertex.Z == ChunkSize && !(TransitionMask & ZMax)))
			{
				// Can't translate when on a corner
				continue;
			}

			FVector3f Delta(0.f);

			if ((TransitionMask & XMin) && Vertex.X < LowerBound)
			{
				Delta.X = LowerBound - Vertex.X;
			}
			if ((TransitionMask & XMax) && Vertex.X > UpperBound)
			{
				Delta.X = UpperBound - Vertex.X;
			}
			if ((TransitionMask & YMin) && Vertex.Y < LowerBound)
			{
				Delta.Y = LowerBound - Vertex.Y;
			}
			if ((TransitionMask & YMax) && Vertex.Y > UpperBound)
			{
				Delta.Y = UpperBound - Vertex.Y;
			}
			if ((TransitionMask & ZMin) && Vertex.Z < LowerBound)
			{
				Delta.Z = LowerBound - Vertex.Z;
			}
			if ((TransitionMask & ZMax) && Vertex.Z > UpperBound)
			{
				Delta.Z = UpperBound - Vertex.Z;
			}

			Delta /= 4;

			const FVector3f Normal = VertexNormals[Index];
			ensureVoxelSlow(Normal.IsNormalized() || Normal.IsZero());

			Vertex += FVector3f(
				(1 - Normal.X * Normal.X) * Delta.X - Normal.Y * Normal.X * Delta.Y - Normal.Z * Normal.X * Delta.Z,
				-Normal.X * Normal.Y * Delta.X + (1 - Normal.Y * Normal.Y) * Delta.Y - Normal.Z * Normal.Y * Delta.Z,
				-Normal.X * Normal.Z * Delta.X - Normal.Y * Normal.Z * Delta.Y + (1 - Normal.Z * Normal.Z) * Delta.Z);
		}
	}

	NumIndicesToRender = NewIndices.Num();
	NumVerticesToRender = NewVertices.Num();

	struct FPrimitiveData
	{
		union
		{
			struct
			{
				uint32 CellIndex : 30;
				uint32 Direction : 2;
			};
			uint32 Word;
		};
	};
	checkStatic(sizeof(FPrimitiveData) == sizeof(uint32));
	TVoxelArray<FPrimitiveData> PrimitiveDatas;
	{
		VOXEL_SCOPE_COUNTER("Build PrimitiveData");
		ensure(3 * NewCellIndices.Num() == NewIndices.Num());

		PrimitiveDatas.Reserve(2 * NewVertices.Num());
		FVoxelUtilities::SetNumFast(PrimitiveDatas, NewVertices.Num());
		FVoxelUtilities::Memset(PrimitiveDatas, 0xFF);

		for (int32 TriangleIndex = 0; TriangleIndex < NewCellIndices.Num(); TriangleIndex++)
		{
			const int32 CellIndex = NewCellIndices[TriangleIndex];
			if (!ensureVoxelSlow(CellIndexToDirection.IsValidIndex(CellIndex)))
			{
				continue;
			}

			FPrimitiveData PrimitiveData;
			PrimitiveData.CellIndex = CellIndex;
			PrimitiveData.Direction = CellIndexToDirection[CellIndex];

			const int32 IndexA = NewIndices[3 * TriangleIndex + 0];
			const int32 IndexB = NewIndices[3 * TriangleIndex + 1];
			const int32 IndexC = NewIndices[3 * TriangleIndex + 2];

			// Set the data on the leading vertex
			// This will be the value used by nointerpolation attributes

			if (PrimitiveDatas[IndexA].Word == -1 ||
				PrimitiveDatas[IndexA].Word == PrimitiveData.Word)
			{
				PrimitiveDatas[IndexA] = PrimitiveData;
				continue;
			}
			if (PrimitiveDatas[IndexB].Word == -1 ||
				PrimitiveDatas[IndexB].Word == PrimitiveData.Word)
			{
				NewIndices[3 * TriangleIndex + 0] = IndexB;
				NewIndices[3 * TriangleIndex + 1] = IndexC;
				NewIndices[3 * TriangleIndex + 2] = IndexA;
				PrimitiveDatas[IndexB] = PrimitiveData;
				continue;
			}
			if (PrimitiveDatas[IndexC].Word == -1 ||
				PrimitiveDatas[IndexC].Word == PrimitiveData.Word)
			{
				NewIndices[3 * TriangleIndex + 0] = IndexC;
				NewIndices[3 * TriangleIndex + 1] = IndexA;
				NewIndices[3 * TriangleIndex + 2] = IndexB;
				PrimitiveDatas[IndexC] = PrimitiveData;
				continue;
			}

			const int32 NewIndex0 = NewVertices.Add(MakeCopy(NewVertices[IndexA]));
			const int32 NewIndex1 = PrimitiveDatas.Add(PrimitiveData);
			checkVoxelSlow(NewIndex0 == NewIndex1);

			if (bHasVertexNormals)
			{
				const int32 NewIndex2 = NewVertexNormals.Add(MakeCopy(NewVertexNormals[IndexA]));
				checkVoxelSlow(NewIndex0 == NewIndex2);
			}

			NewIndices[3 * TriangleIndex + 0] = NewIndex0;
		}

		// Make sure all PrimitiveData are valid for vertex shader detail textures
		for (int32 TriangleIndex = 0; TriangleIndex < NewCellIndices.Num(); TriangleIndex++)
		{
			const int32 CellIndex = NewCellIndices[TriangleIndex];
			if (!ensureVoxelSlow(CellIndexToDirection.IsValidIndex(CellIndex)))
			{
				continue;
			}

			FPrimitiveData PrimitiveData;
			PrimitiveData.CellIndex = CellIndex;
			PrimitiveData.Direction = CellIndexToDirection[CellIndex];

			const int32 IndexA = NewIndices[3 * TriangleIndex + 0];
			const int32 IndexB = NewIndices[3 * TriangleIndex + 1];
			const int32 IndexC = NewIndices[3 * TriangleIndex + 2];

			if (PrimitiveDatas[IndexA].Word == -1)
			{
				PrimitiveDatas[IndexA] = PrimitiveData;
			}
			if (PrimitiveDatas[IndexB].Word == -1)
			{
				PrimitiveDatas[IndexB] = PrimitiveData;
			}
			if (PrimitiveDatas[IndexC].Word == -1)
			{
				PrimitiveDatas[IndexC] = PrimitiveData;
			}
		}
	}

	// These might be used by enqueued draw, keep them alive
	FVoxelRenderUtilities::KeepAliveThisFrameAndRelease(IndicesBuffer);
	FVoxelRenderUtilities::KeepAliveThisFrameAndRelease(VerticesBuffer);
	FVoxelRenderUtilities::KeepAliveThisFrameAndRelease(VertexNormalsBuffer);
	FVoxelRenderUtilities::KeepAliveThisFrameAndRelease(PrimitivesDataBuffer);

	IndicesBuffer.Reset();
	VerticesBuffer.Reset();
	VertexNormalsBuffer.Reset();
	PrimitivesDataBuffer.Reset();

	{
		VOXEL_SCOPE_COUNTER("Indices");

		IndicesBuffer = MakeVoxelShared<FIndexBuffer>();

		FVoxelResourceArrayRef ResourceArray(NewIndices);
		FRHIResourceCreateInfo CreateInfo(TEXT("Indices"), &ResourceArray);
		IndicesBuffer->IndexBufferRHI = UE_503_SWITCH(RHICreateIndexBuffer, RHICmdList.CreateIndexBuffer)(
			sizeof(int32),
			NewIndices.Num() * sizeof(int32),
			BUF_Static,
			CreateInfo);
		IndicesBuffer->InitResource(UE_503_ONLY(RHICmdList));
	}

	{
		VOXEL_SCOPE_COUNTER("Vertices");

		VerticesBuffer = MakeVoxelShared<FVertexBuffer>();

		FVoxelResourceArrayRef ResourceArray(NewVertices);
		FRHIResourceCreateInfo CreateInfo(TEXT("Vertices"), &ResourceArray);
		VerticesBuffer->VertexBufferRHI = UE_503_SWITCH(RHICreateVertexBuffer, RHICmdList.CreateVertexBuffer)(
			NewVertices.Num() * sizeof(FVector3f),
			BUF_Static,
			CreateInfo);
		VerticesBuffer->InitResource(UE_503_ONLY(RHICmdList));

		VertexFactory->PositionComponent = FVertexStreamComponent(VerticesBuffer.Get(), 0, sizeof(FVector3f), VET_Float3);
	}

	{
		VOXEL_SCOPE_COUNTER("PrimitivesData");
		ensure(PrimitiveDatas.Num() == NewVertices.Num());

		PrimitivesDataBuffer = MakeVoxelShared<FVertexBuffer>();

		FVoxelResourceArrayRef ResourceArray(PrimitiveDatas);
		FRHIResourceCreateInfo CreateInfo(TEXT("PrimitivesData"), &ResourceArray);
		PrimitivesDataBuffer->VertexBufferRHI = UE_503_SWITCH(RHICreateVertexBuffer, RHICmdList.CreateVertexBuffer)(
			PrimitiveDatas.Num() * sizeof(uint32),
			BUF_Static,
			CreateInfo);

		PrimitivesDataBuffer->InitResource(UE_503_ONLY(RHICmdList));

		VertexFactory->PrimitiveDataComponent = FVertexStreamComponent(PrimitivesDataBuffer.Get(), 0, sizeof(uint32), VET_UInt);
	}

	if (bHasVertexNormals)
	{
		VOXEL_SCOPE_COUNTER("VertexNormals");
		ensure(NewVertexNormals.Num() == NewVertices.Num());

		VertexNormalsBuffer = MakeVoxelShared<FVertexBuffer>();

		FVoxelResourceArrayRef ResourceArray(NewVertexNormals);
		FRHIResourceCreateInfo CreateInfo(TEXT("VertexNormals"), &ResourceArray);
		VertexNormalsBuffer->VertexBufferRHI = UE_503_SWITCH(RHICreateVertexBuffer, RHICmdList.CreateVertexBuffer)(
			NewVertexNormals.Num() * sizeof(FVector3f),
			BUF_Static,
			CreateInfo);
		VertexNormalsBuffer->InitResource(UE_503_ONLY(RHICmdList));

		VertexFactory->VertexNormalComponent = FVertexStreamComponent(VertexNormalsBuffer.Get(), 0, sizeof(FVector3f), VET_Float3);
	}

	if (VertexFactory->IsInitialized())
	{
		VOXEL_INLINE_COUNTER("VertexFactory ReleaseResource", VertexFactory->ReleaseResource());
	}
	VOXEL_INLINE_COUNTER("VertexFactory InitResource", VertexFactory->InitResource(UE_503_ONLY(RHICmdList)));
}

int64 FVoxelMarchingCubeMesh::GetAllocatedSize() const
{
	int64 AllocatedSize =
		Indices.GetAllocatedSize() +
		Vertices.GetAllocatedSize() +
		VertexNormals.GetAllocatedSize();

	for (const TVoxelArray<FTransitionIndex>& Array : TransitionIndices)
	{
		AllocatedSize += Array.GetAllocatedSize();
	}
	for (const TVoxelArray<FTransitionVertex>& Array : TransitionVertices)
	{
		AllocatedSize += Array.GetAllocatedSize();
	}

	return AllocatedSize;
}

int64 FVoxelMarchingCubeMesh::GetGpuAllocatedSize() const
{
	int64 AllocatedSize = 0;
	if (IndicesBuffer && IndicesBuffer->IndexBufferRHI)
	{
		AllocatedSize += IndicesBuffer->IndexBufferRHI->GetSize();
	}
	if (VerticesBuffer && VerticesBuffer->VertexBufferRHI)
	{
		AllocatedSize += VerticesBuffer->VertexBufferRHI->GetSize();
	}
	if (VertexNormalsBuffer && VertexNormalsBuffer->VertexBufferRHI)
	{
		AllocatedSize += VertexNormalsBuffer->VertexBufferRHI->GetSize();
	}
	return AllocatedSize;
}

TSharedPtr<FVoxelMaterialRef> FVoxelMarchingCubeMesh::GetMaterial() const
{
	return Material;
}

void FVoxelMarchingCubeMesh::Initialize_GameThread()
{
	VOXEL_FUNCTION_COUNTER();
	check(IsInGameThread());
	check(ComputedMaterial);

	ensure(!Material);
	Material = ComputedMaterial->MakeMaterial_GameThread();

	if (!Material->IsInstance())
	{
		Material = FVoxelMaterialRef::MakeInstance(Material->GetMaterial());
	}

	check(!VertexFactory);
	if (bHasVertexNormals)
	{
		VertexFactory = MakeVoxelShared<FVoxelMarchingCubeVertexFactory_WithVertexNormals>(GMaxRHIFeatureLevel);
	}
	else
	{
		VertexFactory = MakeVoxelShared<FVoxelMarchingCubeVertexFactory_NoVertexNormals>(GMaxRHIFeatureLevel);
	}

	VertexFactory->VoxelSize = VoxelSize;
	VertexFactory->NumCells = NumCells;

	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////

	if (const TSharedPtr<FVoxelDynamicMaterialParameter> MaterialIdParameter = Material->FindDynamicParameter(STATIC_FNAME("MaterialId")))
	{
		const TSharedPtr<FVoxelDetailTextureDynamicMaterialParameter> DetailTexture = Cast<FVoxelDetailTextureDynamicMaterialParameter>(MaterialIdParameter);
		if (ensure(DetailTexture))
		{
			const FSimpleDelegate Delegate = MakeWeakPtrDelegate(this, MakeWeakPtrLambda(DetailTexture, [
				this,
				&DetailTexture = *DetailTexture]
			{
				check(IsInGameThread());

				// Textures will be invalided, clear static draws
				MarkRenderStateDirty_GameThread();

				if (!ensure(DetailTexture.Textures.Num() == 2))
				{
					return;
				}

				UTexture2D* TextureA = DetailTexture.Textures[0].Get();
				UTexture2D* TextureB = DetailTexture.Textures[1].Get();
				if (!ensure(TextureA) ||
					!ensure(TextureB))
				{
					return;
				}

				const FTextureResource* ResourceA = TextureA->GetResource();
				const FTextureResource* ResourceB = TextureB->GetResource();
				if (!ensure(ResourceA) ||
					!ensure(ResourceB) ||
					!ensure(VertexFactory))
				{
					return;
				}

				VOXEL_ENQUEUE_RENDER_COMMAND(UpdateTexture)([VertexFactory = VertexFactory, ResourceA, ResourceB](FRHICommandListImmediate& RHICmdList)
				{
					ensure(ResourceA->GetSizeX() == ResourceB->GetSizeX());

					VertexFactory->MaterialId_TextureA = ResourceA->GetTexture2DRHI();
					VertexFactory->MaterialId_TextureB = ResourceB->GetTexture2DRHI();
					VertexFactory->MaterialId_Texture_Size = ResourceA->GetSizeX();
					VertexFactory->MaterialId_Texture_InvSize = 1. / double(ResourceA->GetSizeX());
				});
			}));

			DetailTexture->OnChangedMulticast.Add(Delegate);
			Delegate.Execute();
		}
	}

	if (const float* MaterialId_TextureSize = Material->FindScalarParameter(STATIC_FNAME("MaterialId_TextureSize")))
	{
		VertexFactory->MaterialId_TextureSize = *MaterialId_TextureSize;
	}

	if (const float* MaterialId_TextureIndex = Material->FindScalarParameter(STATIC_FNAME("MaterialId_TextureIndex")))
	{
		VertexFactory->MaterialId_TextureIndex = FMath::RoundToInt(*MaterialId_TextureIndex);
	}

	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////

	if (bHasVertexNormals)
	{
		return;
	}

	const TSharedPtr<FVoxelDynamicMaterialParameter> NormalParameter = Material->FindDynamicParameter(STATIC_FNAME("Normal"));
	if (ensure(NormalParameter))
	{
		const TSharedPtr<FVoxelDetailTextureDynamicMaterialParameter> DetailTexture = Cast<FVoxelDetailTextureDynamicMaterialParameter>(NormalParameter);
		if (ensure(DetailTexture))
		{
			const FSimpleDelegate Delegate = MakeWeakPtrDelegate(this, MakeWeakPtrLambda(DetailTexture, [
				this,
				&DetailTexture = *DetailTexture]
			{
				check(IsInGameThread());

				// Textures will be invalided, clear static draws
				MarkRenderStateDirty_GameThread();

				if (!ensure(DetailTexture.Textures.Num() == 1))
				{
					return;
				}

				UTexture2D* Texture = DetailTexture.Textures[0].Get();
				if (!ensure(Texture))
				{
					return;
				}

				const FTextureResource* Resource = Texture->GetResource();
				if (!ensure(Resource) ||
					!ensure(VertexFactory))
				{
					return;
				}

				VOXEL_ENQUEUE_RENDER_COMMAND(UpdateTexture)([VertexFactory = VertexFactory, Resource](FRHICommandListImmediate& RHICmdList)
				{
					VertexFactory->Normal_Texture = Resource->GetTexture2DRHI();
					VertexFactory->Normal_Texture_Size = Resource->GetSizeX();
					VertexFactory->Normal_Texture_InvSize = 1. / double(Resource->GetSizeX());
				});
			}));

			DetailTexture->OnChangedMulticast.Add(Delegate);
			Delegate.Execute();
		}
	}

	const float* Normal_TextureSize = Material->FindScalarParameter(STATIC_FNAME("Normal_TextureSize"));
	if (ensure(Normal_TextureSize))
	{
		VertexFactory->Normal_TextureSize = *Normal_TextureSize;
	}

	const float* Normal_TextureIndex = Material->FindScalarParameter(STATIC_FNAME("Normal_TextureIndex"));
	if (ensure(Normal_TextureIndex))
	{
		VertexFactory->Normal_TextureIndex = FMath::RoundToInt(*Normal_TextureIndex);
	}
}

///////////////////////////////////////////////////////////////////////////////

void FVoxelMarchingCubeMesh::Initialize_RenderThread(FRHICommandList& RHICmdList, ERHIFeatureLevel::Type FeatureLevel)
{
	VOXEL_FUNCTION_COUNTER();
	check(IsInRenderingThread());

	SetTransitionMask_RenderThread(RHICmdList, TransitionMask);

	if (CellTextureCoordinates.Num() == 0)
	{
		return;
	}
	ensure(CellTextureCoordinates.Num() % NumCells == 0);

	VOXEL_SCOPE_COUNTER("CellTextureCoordinates");

	CellTextureCoordinatesBuffer = MakeVoxelShared<FVertexBufferWithSRV>();
	{
		FVoxelResourceArrayRef ResourceArray(CellTextureCoordinates);
		FRHIResourceCreateInfo CreateInfo(TEXT("CellTextureCoordinates"), &ResourceArray);
		CellTextureCoordinatesBuffer->VertexBufferRHI = UE_503_SWITCH(RHICreateVertexBuffer, RHICmdList.CreateVertexBuffer)(
			CellTextureCoordinates.Num() * sizeof(FVoxelDetailTextureCoordinate),
			BUF_Static | BUF_ShaderResource,
			CreateInfo);

		CellTextureCoordinatesBuffer->ShaderResourceViewRHI = UE_503_SWITCH(RHICreateShaderResourceView, RHICmdList.CreateShaderResourceView)(
			CellTextureCoordinatesBuffer->VertexBufferRHI,
			sizeof(FVoxelDetailTextureCoordinate),
			PF_R16G16_UINT);
	}
	CellTextureCoordinates.Empty();
	CellTextureCoordinatesBuffer->InitResource(UE_503_ONLY(RHICmdList));

	VertexFactory->CellTextureCoordinates = CellTextureCoordinatesBuffer->ShaderResourceViewRHI;
}

void FVoxelMarchingCubeMesh::Destroy_RenderThread()
{
	VOXEL_FUNCTION_COUNTER();
	check(IsInRenderingThread());

	if (IndicesBuffer)
	{
		IndicesBuffer->ReleaseResource();
		IndicesBuffer.Reset();
	}
	if (VerticesBuffer)
	{
		VerticesBuffer->ReleaseResource();
		VerticesBuffer.Reset();
	}
	if (VertexNormalsBuffer)
	{
		VertexNormalsBuffer->ReleaseResource();
		VertexNormalsBuffer.Reset();
	}
	if (PrimitivesDataBuffer)
	{
		PrimitivesDataBuffer->ReleaseResource();
		PrimitivesDataBuffer.Reset();
	}
	if (CellTextureCoordinatesBuffer)
	{
		CellTextureCoordinatesBuffer->ReleaseResource();
		CellTextureCoordinatesBuffer.Reset();
	}

	check(VertexFactory);
	VertexFactory->ReleaseResource();
	VertexFactory.Reset();
}

///////////////////////////////////////////////////////////////////////////////

bool FVoxelMarchingCubeMesh::Draw_RenderThread(const FPrimitiveSceneProxy& Proxy, FMeshBatch& MeshBatch) const
{
	if (!ensure(MeshBatch.MaterialRenderProxy) ||
		!ensure(IndicesBuffer))
	{
		return false;
	}
	ensure(NumIndicesToRender % 3 == 0);

	MeshBatch.Type = PT_TriangleList;
	MeshBatch.VertexFactory = VertexFactory.Get();

	FMeshBatchElement& BatchElement = MeshBatch.Elements[0];
	BatchElement.PrimitiveUniformBuffer = Proxy.GetUniformBuffer();
	BatchElement.IndexBuffer = IndicesBuffer.Get();
	BatchElement.FirstIndex = 0;
	BatchElement.NumPrimitives = NumIndicesToRender / 3;
	BatchElement.MinVertexIndex = 0;
	BatchElement.MaxVertexIndex = NumVerticesToRender - 1;

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	MeshBatch.VisualizeLODIndex = LOD % GEngine->LODColorationColors.Num();
#endif

	return true;
}