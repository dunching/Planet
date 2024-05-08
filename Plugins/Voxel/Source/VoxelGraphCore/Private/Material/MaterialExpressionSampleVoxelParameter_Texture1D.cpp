// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Material/MaterialExpressionSampleVoxelParameter_Texture1D.h"
#include "TextureResource.h"
#include "MaterialCompiler.h"
#include "Engine/Texture2D.h"

void FVoxelMaterialParameterData_Texture1D::CacheParameters(
	const FName Name,
	FCachedParameters& InOutParameters) const
{
	InOutParameters.TextureParameters.Add(Name, Texture);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void UMaterialExpressionSampleVoxelParameter_Texture1D::UpdateVoxelParameterData(
	const FName DebugName,
	const TVoxelArray<FInstance>& Instances,
	TVoxelInstancedStruct<FVoxelMaterialParameterData>& InOutParameterData) const
{
	VOXEL_FUNCTION_COUNTER();

	FVoxelMaterialParameterData_Texture1D& ParameterData = InOutParameterData.Get<FVoxelMaterialParameterData_Texture1D>();
	const TArray<uint8> PreviousData = ParameterData.Data;

	int32 SizeX = 0;
	for (const FInstance& Instance : Instances)
	{
		SizeX = FMath::Max(SizeX, Instance.Index + 1);
	}
	SizeX = FMath::Max<uint32>(16, FMath::RoundUpToPowerOfTwo(SizeX));

	const EPixelFormat PixelFormat = GetVoxelTexturePixelFormat();
	check(GPixelFormats[PixelFormat].BlockSizeX == 1);
	check(GPixelFormats[PixelFormat].BlockSizeY == 1);

	const int32 TypeSize = GPixelFormats[PixelFormat].BlockBytes;

	ParameterData.Data.Reset();
	{
		VOXEL_SCOPE_COUNTER("Build data");
		FVoxelUtilities::SetNumZeroed(ParameterData.Data, SizeX * TypeSize);
		for (const FInstance& Instance : Instances)
		{
			CopyVoxelInstanceData(
				MakeVoxelArrayView(ParameterData.Data).Slice(Instance.Index * TypeSize, TypeSize),
				Instance.Value);
		}
	}

	if (ParameterData.Data == PreviousData)
	{
		return;
	}

	if (!ParameterData.Texture ||
		ParameterData.Texture->GetSizeX() != SizeX ||
		ParameterData.Texture->GetPixelFormat() != PixelFormat)
	{
		GVoxelMaterialDefinitionManager->QueueMaterialRefresh();

		const int64 NumBytes = SizeX * TypeSize;
		const FName TextureName = MakeUniqueObjectName(
			GetTransientPackage(),
			UTexture2D::StaticClass(),
			"VoxelMaterialTexture_" + DebugName);

		ParameterData.Texture = NewObject<UTexture2D>(GetTransientPackage(), TextureName, RF_Transient);
		ParameterData.TextureMemory = NumBytes;

		FTexturePlatformData* PlatformData = new FTexturePlatformData();
		PlatformData->SizeX = SizeX;
		PlatformData->SizeY = 1;
		PlatformData->PixelFormat = PixelFormat;

		FTexture2DMipMap* Mip = new FTexture2DMipMap();
		PlatformData->Mips.Add(Mip);
		Mip->SizeX = SizeX;
		Mip->SizeY = 1;
		Mip->BulkData.Lock(LOCK_READ_WRITE);
		{
			VOXEL_SCOPE_COUNTER("Memcpy");
			FVoxelUtilities::Memcpy(
				MakeVoxelArrayView<uint8>(Mip->BulkData.Realloc(NumBytes), NumBytes),
				ParameterData.Data);
		}
		Mip->BulkData.Unlock();

		ParameterData.Texture->SetPlatformData(PlatformData);

		VOXEL_SCOPE_COUNTER("UpdateResource");
		ParameterData.Texture->UpdateResource();

		return;
	}

	FTextureResource* Resource = ParameterData.Texture->GetResource();
	VOXEL_ENQUEUE_RENDER_COMMAND(UpdateTexture)([DebugName, SizeX, Data = ParameterData.Data, Resource](FRHICommandListImmediate& RHICmdList)
	{
		FRHITexture* RHI = Resource->GetTextureRHI();
		if (!ensure(RHI))
		{
			return;
		}

		VOXEL_SCOPE_COUNTER_FORMAT("RHIUpdateTexture2D %s", *DebugName.ToString());

		const FUpdateTextureRegion2D UpdateRegion(
			0,
			0,
			0,
			0,
			SizeX,
			1);

		RHIUpdateTexture2D_Safe(
			RHI,
			0,
			UpdateRegion,
			Data.Num(),
			Data);
		});
}

#if WITH_EDITOR
void UMaterialExpressionSampleVoxelParameter_Texture1D::CompileVoxel(
	FMaterialCompiler& Compiler,
	UMaterialExpressionCustom& Custom,
	const FVoxelMaterialParameterData& ParameterData,
	const FName Name,
	const int32 MaterialId,
	const int32 PreviewMaterialId,
	TArray<int32>& Inputs)
{
	Custom.OutputType = GetCustomOutputType();

	if (MaterialId == -1)
	{
		Custom.Code = FString() +
			"#if PIXEL_PARAMETERS_HAS_VoxelDetailTexture_MaterialIds_LerpAlphas\n" +
			"return lerp(lerp(" +
			GenerateHLSL("Texture[uint2(Parameters.VoxelDetailTexture_MaterialIds.x, 0)]") + "," +
			GenerateHLSL("Texture[uint2(Parameters.VoxelDetailTexture_MaterialIds.y, 0)]") + ", Parameters.VoxelDetailTexture_LerpAlphas.x)," +
			GenerateHLSL("Texture[uint2(Parameters.VoxelDetailTexture_MaterialIds.z, 0)]") + ", Parameters.VoxelDetailTexture_LerpAlphas.y);\n" +
			"#else\n" +
			"return " + GenerateHLSL("Texture[uint2(PreviewMaterialId, 0)]") + ";\n" +
			"#endif";
	}
	else
	{
		Custom.Code = "return " + GenerateHLSL("Texture[uint2(MaterialId, 0)]") + ";";

		Inputs.Add(MaterialId);
		Custom.Inputs.Add({ "MaterialId" });
	}

	Inputs.Add(PreviewMaterialId);
	Custom.Inputs.Add({ "PreviewMaterialId" });

	Inputs.Add(Compiler.TextureParameter(Name, FVoxelTextureUtilities::GetDefaultTexture2D(), SAMPLERTYPE_Color, SSM_Wrap_WorldGroupSettings));
	Custom.Inputs.Add({ "Texture" });
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

UMaterialExpressionGetVoxelColorParameter::UMaterialExpressionGetVoxelColorParameter()
{
	Outputs.Reset();
	Outputs.Add(FExpressionOutput(TEXT(""), 1, 1, 1, 1, 0));
	Outputs.Add(FExpressionOutput(TEXT(""), 1, 1, 0, 0, 0));
	Outputs.Add(FExpressionOutput(TEXT(""), 1, 0, 1, 0, 0));
	Outputs.Add(FExpressionOutput(TEXT(""), 1, 0, 0, 1, 0));
	Outputs.Add(FExpressionOutput(TEXT(""), 1, 0, 0, 0, 1));
}