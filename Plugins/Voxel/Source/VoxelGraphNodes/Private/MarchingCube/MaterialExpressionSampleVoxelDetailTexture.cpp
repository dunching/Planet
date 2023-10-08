// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "MarchingCube/MaterialExpressionSampleVoxelDetailTexture.h"
#include "Engine/Texture2D.h"
#include "MaterialCompiler.h"

UMaterialExpressionSampleVoxelDetailTexture::UMaterialExpressionSampleVoxelDetailTexture()
{
	bHidePreviewWindow = true;
}

UObject* UMaterialExpressionSampleVoxelDetailTexture::GetReferencedTexture() const
{
	return FVoxelTextureUtilities::GetDefaultTexture2D();
}

#if WITH_EDITOR
void UMaterialExpressionSampleVoxelDetailTexture::SetupCustomExpression(UMaterialExpressionCustom& Custom) const
{
	Custom.Code = R"(
#if \
	VOXEL_MARCHING_CUBE_VERTEX_FACTORY && \
	PIXEL_PARAMETERS_HAS_VoxelDetailTexture_CellIndex && \
	PIXEL_PARAMETERS_HAS_VoxelDetailTexture_Delta

	FLATTEN
	if (TextureIndex == -1)
	{
		return 0;
	}

	const uint2 TextureCoordinates = CellTextureCoordinates[TextureIndex * NumCells + Parameters.VoxelDetailTexture_CellIndex];
	float2 UVs = float2(TextureCoordinates.x, TextureCoordinates.y) * TextureSize;

	UVs += 1 + Parameters.VoxelDetailTexture_Delta * (TextureSize - 2);
	UVs += clamp(Offset, -1.f, 1.f) / 2.f;
	UVs *= Texture_InvSize;

	// No derivatives in raytraced shaders, need to use SampleLevel
	#if USE_FORCE_TEXTURE_MIP
		return Texture2DSampleLevel(Texture, TextureSampler, UVs, 0).SWIZZLE;
	#else
		return Texture2DSample(Texture, TextureSampler, UVs).SWIZZLE;
	#endif
#else
	return 0;
#endif
	)";

	if (GetCustomOutputType() == CMOT_Float1)
	{
		Custom.Code.ReplaceInline(TEXT("SWIZZLE"), TEXT("r"), ESearchCase::CaseSensitive);
	}
	else
	{
		ensure(GetCustomOutputType() == CMOT_Float4);
		Custom.Code.ReplaceInline(TEXT("SWIZZLE"), TEXT("rgba"), ESearchCase::CaseSensitive);
	}
}
#endif

#if WITH_EDITOR
int32 UMaterialExpressionSampleVoxelDetailTexture::Compile(FMaterialCompiler* Compiler, int32 OutputIndex)
{
	const UVoxelDetailTexture* DetailTexture = GetDetailTexture();
	if (!DetailTexture)
	{
		return Compiler->Error(TEXT("DetailTexture is null"));
	}

	const FString Guid = DetailTexture->GetGuid().ToString();

	UMaterialExpressionCustom* Custom = NewObject<UMaterialExpressionCustom>();
	Custom->Inputs.Empty();
	Custom->OutputType = GetCustomOutputType();
	SetupCustomExpression(*Custom);

	Custom->IncludeFilePaths.Add("/Plugin/Voxel/VoxelMarchingCubeVertexFactoryDefines.ush");

	TArray<int32> Inputs;
	{
		UTexture2D* DefaultTexture = FVoxelTextureUtilities::GetDefaultTexture2D();
		if (!ensure(DefaultTexture))
		{
			return -1;
		}

		Inputs.Add(Offset.GetTracedInput().Expression ? Offset.Compile(Compiler) : Compiler->Constant(0.f));
		Custom->Inputs.Add({ "Offset" });

		Inputs.Add(Compiler->TextureParameter(FName(Guid + "_Texture"), DefaultTexture, SAMPLERTYPE_Color));
		Custom->Inputs.Add({ "Texture" });

		Inputs.Add(Compiler->ScalarParameter(FName(Guid + "_Texture_Size"), 0.f));
		Custom->Inputs.Add({ "Texture_Size" });

		Inputs.Add(Compiler->ScalarParameter(FName(Guid + "_Texture_InvSize"), 0.f));
		Custom->Inputs.Add({ "Texture_InvSize" });

		Inputs.Add(Compiler->ScalarParameter(FName(Guid + "_TextureIndex"), -1));
		Custom->Inputs.Add({ "TextureIndex" });

		Inputs.Add(Compiler->ScalarParameter(FName(Guid + "_TextureSize"), 0.f));
		Custom->Inputs.Add({ "TextureSize" });
	}

	if (!ensure(!Inputs.Contains(-1)))
	{
		return -1;
	}

	return Compiler->CustomExpression(Custom, OutputIndex, Inputs);
}

void UMaterialExpressionSampleVoxelDetailTexture::GetCaption(TArray<FString>& OutCaptions) const
{
	FString Name = GetClass()->GetDisplayNameText().ToString();
	Name.RemoveFromStart("Material Expression ");
	OutCaptions.Add(Name);

	if (const UVoxelDetailTexture* DetailTexture = GetDetailTexture())
	{
		OutCaptions.Add(DetailTexture->GetName());
	}
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

UMaterialExpressionSampleVoxelColorDetailTexture::UMaterialExpressionSampleVoxelColorDetailTexture()
{
	Outputs.Reset();
	Outputs.Add(FExpressionOutput(TEXT(""), 1, 1, 1, 1, 0));
	Outputs.Add(FExpressionOutput(TEXT(""), 1, 1, 0, 0, 0));
	Outputs.Add(FExpressionOutput(TEXT(""), 1, 0, 1, 0, 0));
	Outputs.Add(FExpressionOutput(TEXT(""), 1, 0, 0, 1, 0));
	Outputs.Add(FExpressionOutput(TEXT(""), 1, 0, 0, 0, 1));
}

#if WITH_EDITOR
int32 UMaterialExpressionSampleVoxelColorDetailTexture::Compile(FMaterialCompiler* Compiler, int32 OutputIndex)
{
	return Super::Compile(Compiler, 0);
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

UMaterialExpressionSampleVoxelMaterialIdDetailTexture::UMaterialExpressionSampleVoxelMaterialIdDetailTexture()
{
	bShowOutputNameOnPin = true;

	Outputs.Reset();
	Outputs.Add({ "Layer0" });
	Outputs.Add({ "Layer1" });
	Outputs.Add({ "Layer2" });
	Outputs.Add({ "LerpAlphaA" });
	Outputs.Add({ "LerpAlphaB" });
}

#if WITH_EDITOR
void UMaterialExpressionSampleVoxelMaterialIdDetailTexture::SetupCustomExpression(UMaterialExpressionCustom& Custom) const
{
	Custom.AdditionalOutputs.Add(
	{
		"Weights",
		CMOT_Float2
	});

	Custom.Code = R"(
#if VOXEL_MARCHING_CUBE_VERTEX_FACTORY
	float Layer0 = 0;
	float Layer1 = 0;
	float Layer2 = 0;
	float LerpAlphaA = 0;
	float LerpAlphaB = 0;
	SampleVoxelMaterialId(
		Parameters,
		Texture,
		TextureSampler,
		TextureIndex,
		TextureSize,
		Texture_Size,
		Texture_InvSize,
		Offset,
		Layer0,
		Layer1,
		Layer2,
		LerpAlphaA,
		LerpAlphaB);

	Weights = float2(LerpAlphaA, LerpAlphaB);
	return float3(Layer0, Layer1, Layer2);
#else
	return 0;
#endif
	)";
}

int32 UMaterialExpressionSampleVoxelMaterialIdDetailTexture::Compile(FMaterialCompiler* Compiler, const int32 OutputIndex)
{
	const int32 Layers = Super::Compile(Compiler, 0);
	const int32 Weights = Super::Compile(Compiler, 1);

	switch (OutputIndex)
	{
	default: ensure(false);
	case 0: return Compiler->ComponentMask(Layers, true, false, false, false);
	case 1: return Compiler->ComponentMask(Layers, false, true, false, false);
	case 2: return Compiler->ComponentMask(Layers, false, false, true, false);
	case 3: return Compiler->ComponentMask(Weights, true, false, false, false);
	case 4: return Compiler->ComponentMask(Weights, false, true, false, false);
	}
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

UMaterialExpressionGetVoxelMaterialId::UMaterialExpressionGetVoxelMaterialId()
{
	bShowOutputNameOnPin = true;

	Outputs.Reset();
	Outputs.Add({ "Layer0" });
	Outputs.Add({ "Layer1" });
	Outputs.Add({ "Layer2" });
	Outputs.Add({ "LerpAlphaA" });
	Outputs.Add({ "LerpAlphaB" });
}

#if WITH_EDITOR
int32 UMaterialExpressionGetVoxelMaterialId::Compile(FMaterialCompiler* Compiler, const int32 OutputIndex)
{
	UMaterialExpressionCustom* Custom = NewObject<UMaterialExpressionCustom>();
	Custom->Inputs.Empty();
	Custom->OutputType = CMOT_Float1;

	switch (OutputIndex)
	{
	default: ensure(false);
	case 0:
	{
		Custom->Code = R"(
#if PIXEL_PARAMETERS_HAS_VoxelDetailTexture_MaterialIds_LerpAlphas
	return Parameters.VoxelDetailTexture_MaterialIds.x;
#else
	return PreviewMaterialId;
#endif)";
	}
	break;
	case 1:
	{
		Custom->Code = R"(
#if PIXEL_PARAMETERS_HAS_VoxelDetailTexture_MaterialIds_LerpAlphas
	return Parameters.VoxelDetailTexture_MaterialIds.y;
#else
	return PreviewMaterialId;
#endif)";
	}
	break;
	case 2:
	{
		Custom->Code = R"(
#if PIXEL_PARAMETERS_HAS_VoxelDetailTexture_MaterialIds_LerpAlphas
	return Parameters.VoxelDetailTexture_MaterialIds.z;
#else
	return PreviewMaterialId;
#endif)";
	}
	break;
	case 3:
	{
		Custom->Code = R"(
#if PIXEL_PARAMETERS_HAS_VoxelDetailTexture_MaterialIds_LerpAlphas
	return Parameters.VoxelDetailTexture_LerpAlphas.x;
#else
	return 0;
#endif)";
	}
	break;
	case 4:
	{
		Custom->Code = R"(
#if PIXEL_PARAMETERS_HAS_VoxelDetailTexture_MaterialIds_LerpAlphas
	return Parameters.VoxelDetailTexture_LerpAlphas.y;
#else
	return 0;
#endif)";
	}
	break;
	}

	TArray<int32> Inputs;
	Inputs.Add(Compiler->ScalarParameter("PreviewMaterialId", 0.f));
	Custom->Inputs.Add({ "PreviewMaterialId" });
	return Compiler->CustomExpression(Custom, 0, Inputs);
}

void UMaterialExpressionGetVoxelMaterialId::GetCaption(TArray<FString>& OutCaptions) const
{
	OutCaptions.Add("Get Voxel Material Id");
}
#endif