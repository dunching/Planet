// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Material/MaterialExpressionSampleVoxelTextureParameter.h"
#include "TextureResource.h"
#include "MaterialCompiler.h"
#include "Engine/Texture2DArray.h"

void FVoxelMaterialParameterData_TextureArray::Fixup()
{
	TextureSize = FMath::RoundUpToPowerOfTwo(FMath::Clamp(TextureSize, 16, 8192));
	LastMipTextureSize = FMath::RoundUpToPowerOfTwo(FMath::Clamp(LastMipTextureSize, 16, TextureSize));
}

void FVoxelMaterialParameterData_TextureArray::CacheParameters(
	const FName Name,
	FCachedParameters& InOutParameters) const
{
	InOutParameters.TextureParameters.Add(Name + "_IndirectionTexture", IndirectionTexture);
	InOutParameters.TextureParameters.Add(Name + "_TextureArray", TextureArray);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

UMaterialExpressionSampleVoxelTextureParameter::UMaterialExpressionSampleVoxelTextureParameter()
{
	// Mitigate crash in UTexture::PostLoad
	static ConstructorHelpers::FObjectFinder<UTexture2DArray> DefaultTextureArray(
		TEXT("/Voxel/Default/DefaultTextureArray.DefaultTextureArray"));
	ensure(DefaultTextureArray.Object);

	Outputs.Reset();
	Outputs.Add(FExpressionOutput(TEXT(""), 1, 1, 1, 1, 0));
	Outputs.Add(FExpressionOutput(TEXT(""), 1, 1, 0, 0, 0));
	Outputs.Add(FExpressionOutput(TEXT(""), 1, 0, 1, 0, 0));
	Outputs.Add(FExpressionOutput(TEXT(""), 1, 0, 0, 1, 0));
	Outputs.Add(FExpressionOutput(TEXT(""), 1, 0, 0, 0, 1));

#if WITH_EDITOR
	CachedInputs.Add(&UVsInput);
#endif
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void UMaterialExpressionSampleVoxelTextureParameter::UpdateVoxelParameterData(
	const FName DebugName,
	const TVoxelArray<FInstance>& Instances,
	TVoxelInstancedStruct<FVoxelMaterialParameterData>& InOutParameterData) const
{
	VOXEL_FUNCTION_COUNTER();

	FVoxelMaterialParameterData_TextureArray& ParameterData = InOutParameterData.Get<FVoxelMaterialParameterData_TextureArray>();

	const TArray<TObjectPtr<UTexture2D>> PreviousTextureArrayValues = ParameterData.TextureArrayValues;

	TVoxelArray<int32> IndirectionData;
	{
		int32 Num = 0;
		for (const FInstance& Instance : Instances)
		{
			Num = FMath::Max(Num, Instance.Index + 1);
		}
		Num = FMath::Max<uint32>(16, FMath::RoundUpToPowerOfTwo(Num));

		FVoxelUtilities::SetNumFast(IndirectionData, Num);
		FVoxelUtilities::SetAll(IndirectionData, -1);

		TVoxelAddOnlyMap<UTexture2D*, int32> TextureToIndex;
		TVoxelArray<int32> FreeTextureIndices;
		{
			TSet<UTexture2D*> UsedTextures;
			for (const FInstance& Instance : Instances)
			{
				UsedTextures.Add(Instance.Value.Get<UTexture2D>());
			}

			if (UsedTextures.Num() < ParameterData.TextureArrayValues.Num())
			{
				ParameterData.TextureArrayValues.SetNum(UsedTextures.Num());
			}

			// Try to reuse already allocated textures if possible
			for (int32 Index = 0; Index < ParameterData.TextureArrayValues.Num(); Index++)
			{
				TObjectPtr<UTexture2D>& Texture = ParameterData.TextureArrayValues[Index];
				if (UsedTextures.Contains(Texture))
				{
					TextureToIndex.Add_CheckNew(Texture, Index);
				}
				else
				{
					Texture = nullptr;
					FreeTextureIndices.Add(Index);
				}
			}

			for (int32 Index = ParameterData.TextureArrayValues.Num(); Index < UsedTextures.Num(); Index++)
			{
				ParameterData.TextureArrayValues.Add(nullptr);
				FreeTextureIndices.Add(Index);
			}
		}

		for (const FInstance& Instance : Instances)
		{
			UTexture2D* Texture = Instance.Value.Get<UTexture2D>();

			if (!TextureToIndex.Contains(Texture))
			{
				if (!ensure(FreeTextureIndices.Num() > 0))
				{
					return;
				}
				const int32 Index = FreeTextureIndices.Pop(false);
				ParameterData.TextureArrayValues[Index] = Texture;
				TextureToIndex.Add_CheckNew(Texture, Index);
			}

			IndirectionData[Instance.Index] = TextureToIndex[Texture];
		}
	}

	if (!ParameterData.IndirectionTexture ||
		ParameterData.IndirectionTexture->GetSizeX() != IndirectionData.Num())
	{
		VOXEL_SCOPE_COUNTER("Update indirection");

		GVoxelMaterialDefinitionManager->QueueMaterialRefresh();

		const int64 NumBytes = IndirectionData.Num() * IndirectionData.GetTypeSize();
		const FName TextureName = MakeUniqueObjectName(
			GetTransientPackage(),
			UTexture2D::StaticClass(),
			"VoxelMaterialTexture_" + DebugName + "_Indirection");

		ParameterData.IndirectionTexture = NewObject<UTexture2D>(GetTransientPackage(), TextureName, RF_Transient);

		FTexturePlatformData* PlatformData = new FTexturePlatformData();
		PlatformData->SizeX = IndirectionData.Num();
		PlatformData->SizeY = 1;
		PlatformData->PixelFormat = PF_R32_SINT;

		FTexture2DMipMap* Mip = new FTexture2DMipMap();
		PlatformData->Mips.Add(Mip);
		Mip->SizeX = IndirectionData.Num();
		Mip->SizeY = 1;
		Mip->BulkData.Lock(LOCK_READ_WRITE);
		{
			VOXEL_SCOPE_COUNTER("Memcpy");
			FVoxelUtilities::Memcpy(
				MakeVoxelArrayView<uint8>(Mip->BulkData.Realloc(NumBytes), NumBytes),
				ReinterpretCastVoxelArrayView<uint8>(MakeVoxelArrayView(IndirectionData)));
		}
		Mip->BulkData.Unlock();

		ParameterData.IndirectionTexture->SetPlatformData(PlatformData);

		VOXEL_SCOPE_COUNTER("UpdateResource");
		ParameterData.IndirectionTexture->UpdateResource();
	}
	else
	{
		VOXEL_ENQUEUE_RENDER_COMMAND(UpdateIndirectionTexture)([
			DebugName,
			IndirectionData = MoveTemp(IndirectionData),
			Resource = ParameterData.IndirectionTexture->GetResource()](FRHICommandListImmediate& RHICmdList)
		{
			FRHITexture* RHI = Resource->GetTextureRHI();
			if (!ensure(RHI))
			{
				return;
			}

			VOXEL_SCOPE_COUNTER_FORMAT("RHIUpdateTexture2D IndirectionTexture %s", *DebugName.ToString());

			const FUpdateTextureRegion2D UpdateRegion(
				0,
				0,
				0,
				0,
				IndirectionData.Num(),
				1);

			RHIUpdateTexture2D_Safe(
				RHI,
				0,
				UpdateRegion,
				IndirectionData.Num() * IndirectionData.GetTypeSize(),
				MakeByteVoxelArrayView(IndirectionData));
		});
	}

	const int32 NumMips = ParameterData.GetNumMips();
	const EPixelFormat PixelFormat = INLINE_LAMBDA
	{
		switch (ParameterData.Compression)
		{
		default: ensure(false);
		case EVoxelTextureArrayCompression::DXT1: return PF_DXT1;
		case EVoxelTextureArrayCompression::DXT5: return PF_DXT5;
		case EVoxelTextureArrayCompression::BC5: return PF_BC5;
		}
	};

	const auto GetMipData = [&](UTexture2D* Texture) -> TArray<void*>
	{
		const auto GetDebugObject = [&]() -> UObject*
		{
			for (const FInstance& Instance : Instances)
			{
				if (Texture == Instance.Value.Get<UTexture2D>())
				{
					return Instance.DebugObject;
				}
			}
			return nullptr;
		};

		if (!Texture)
		{
			return {};
		}

		FVoxelTextureUtilities::FullyLoadTexture(Texture);

		if (Texture->GetSizeX() != Texture->GetSizeY())
		{
			VOXEL_MESSAGE(Error, "{0}: Texture {1} is not a square", GetDebugObject(), Texture);
			return {};
		}

		const int32 Size = Texture->GetSizeX();
		if (!FMath::IsPowerOfTwo(Size))
		{
			VOXEL_MESSAGE(Error, "{0}: Texture {1} size is not a power of two", GetDebugObject(), Texture);
			return {};
		}

		if (ParameterData.TextureSize > Size)
		{
			VOXEL_MESSAGE(Error, "{0}: Texture {1} has size {2} which is less than required texture size {3}", GetDebugObject(), Texture, Size, ParameterData.TextureSize);
			return {};
		}

		const FTexturePlatformData* PlatformData = Texture->GetPlatformData();
		if (!ensure(PlatformData))
		{
			VOXEL_MESSAGE(Error, "Invalid platform data");
			return {};
		}

		if (PlatformData->PixelFormat != PixelFormat)
		{
			VOXEL_MESSAGE(Error, "{0}: Texture {1} should have compression set to {2}, current format is {3}",
				GetDebugObject(),
				Texture,
				GPixelFormats[PixelFormat].Name,
				GPixelFormats[PlatformData->PixelFormat].Name);
			return {};
		}

		const int32 MipOffset = FVoxelUtilities::ExactLog2(Size / ParameterData.TextureSize);
		if (!ensure(MipOffset + NumMips <= PlatformData->Mips.Num()))
		{
			VOXEL_MESSAGE(Error, "{0}: Texture {1} is missing mips",
				GetDebugObject(),
				Texture);
			return {};
		}

		VOXEL_SCOPE_COUNTER_FORMAT("ReadMips %s", *Texture->GetPathName());

		TArray<void*> MipData;
		MipData.SetNumZeroed(PlatformData->Mips.Num() - MipOffset);
		Texture->GetMipData(MipOffset, MipData.GetData());
		return MipData;
	};

	if (!ParameterData.TextureArray ||
		ParameterData.TextureArray->GetSizeX() != ParameterData.TextureSize ||
		ParameterData.TextureArray->GetSizeY() != ParameterData.TextureSize ||
		ParameterData.TextureArray->GetArraySize() != ParameterData.TextureArrayValues.Num() ||
		ParameterData.TextureArray->GetNumMips() != NumMips)
	{
		VOXEL_SCOPE_COUNTER("Update texture array");

		GVoxelMaterialDefinitionManager->QueueMaterialRefresh();

		TVoxelMap<UTexture2D*, TArray<void*>> TextureToMipDatas;
		ON_SCOPE_EXIT
		{
			for (const auto& It : TextureToMipDatas)
			{
				for (void* Data : It.Value)
				{
					FMemory::Free(Data);
				}
			}
			TextureToMipDatas.Empty();
		};

		for (UTexture2D* Texture : ParameterData.TextureArrayValues)
		{
			TextureToMipDatas.Add_CheckNew(Texture, GetMipData(Texture));
		}

		FTexturePlatformData* PlatformData = new FTexturePlatformData();
		PlatformData->SizeX = ParameterData.TextureSize;
		PlatformData->SizeY = ParameterData.TextureSize;
		PlatformData->SetNumSlices(ParameterData.TextureArrayValues.Num());
		PlatformData->PixelFormat = PixelFormat;

		for (int32 MipIndex = 0; MipIndex < NumMips; MipIndex++)
		{
			const int32 MipTextureSize = ParameterData.TextureSize >> MipIndex;
			ensure(MipTextureSize >= 16);

			FTexture2DMipMap* Mip = new FTexture2DMipMap();
			Mip->SizeX = MipTextureSize;
			Mip->SizeY = MipTextureSize;
			Mip->SizeZ = ParameterData.TextureArrayValues.Num();
			PlatformData->Mips.Add(Mip);

			Mip->BulkData.Lock(LOCK_READ_WRITE);
			ON_SCOPE_EXIT
			{
				Mip->BulkData.Unlock();
			};

			VOXEL_SCOPE_COUNTER_FORMAT("%s: Allocate Mip %d", *DebugName.ToString(), MipIndex);

			const FPixelFormatInfo Info = GPixelFormats[PixelFormat];
			ensure(MipTextureSize % Info.BlockSizeX == 0);
			ensure(MipTextureSize % Info.BlockSizeY == 0);
			ensure(Info.BlockSizeZ == 1);

			const int32 NumBlocksX = MipTextureSize / Info.BlockSizeX;
			const int32 NumBlocksY = MipTextureSize / Info.BlockSizeY;

			const int64 LayerNumBytes = NumBlocksX * NumBlocksY * Info.BlockBytes;
			const int64 TotalNumBytes = LayerNumBytes * ParameterData.TextureArrayValues.Num();
			void* TextureArrayData = Mip->BulkData.Realloc(TotalNumBytes);

			for (int32 Index = 0; Index < ParameterData.TextureArrayValues.Num(); Index++)
			{
				void* WriteData = static_cast<uint8*>(TextureArrayData) + Index * LayerNumBytes;
				UTexture2D* Texture = ParameterData.TextureArrayValues[Index];

				const TArray<void*>& MipData = TextureToMipDatas[Texture];
				if (MipData.Num() == 0)
				{
					VOXEL_SCOPE_COUNTER("Memzero");
					FMemory::Memzero(WriteData, LayerNumBytes);
					continue;
				}

				const void* ReadData = MipData[MipIndex];
				if (!ensure(ReadData))
				{
					VOXEL_SCOPE_COUNTER("Memzero");
					FMemory::Memzero(WriteData, LayerNumBytes);
					continue;
				}

				VOXEL_SCOPE_COUNTER("Memcpy");
				FMemory::Memcpy(WriteData, ReadData, LayerNumBytes);
			}
		}

		const FName TextureName = MakeUniqueObjectName(
			GetTransientPackage(),
			UTexture2DArray::StaticClass(),
			"VoxelMaterialTexture_" + DebugName + "_TextureArray");

		ParameterData.TextureArray = NewObject<UTexture2DArray>(GetTransientPackage(), TextureName, RF_Transient);

		ParameterData.TextureArray->SRGB = INLINE_LAMBDA
		{
			switch (ParameterData.Compression)
			{
			default: ensure(false);
			case EVoxelTextureArrayCompression::DXT1: return true;
			case EVoxelTextureArrayCompression::DXT5: return true;
			case EVoxelTextureArrayCompression::BC5: return false;
			}
		};

		ParameterData.TextureArray->SetPlatformData(PlatformData);

		{
			VOXEL_SCOPE_COUNTER("UpdateResource");
			ParameterData.TextureArray->UpdateResource();
		}

		for (FTexture2DMipMap& Mip : PlatformData->Mips)
		{
			VOXEL_SCOPE_COUNTER("RemoveBulkData");
			Mip.BulkData.RemoveBulkData();
		}

		return;
	}
	check(PreviousTextureArrayValues.Num() == ParameterData.TextureArrayValues.Num());

	for (int32 TextureIndex = 0; TextureIndex < ParameterData.TextureArrayValues.Num(); TextureIndex++)
	{
		UTexture2D* Texture = ParameterData.TextureArrayValues[TextureIndex];
		if (Texture == PreviousTextureArrayValues[TextureIndex])
		{
			continue;
		}

		VOXEL_ENQUEUE_RENDER_COMMAND(UpdateTextureArray)([
			DebugName,
			NumMips,
			PixelFormat,
			TextureIndex,
			MipData = GetMipData(Texture),
			TextureSize = ParameterData.TextureSize,
			Resource = ParameterData.TextureArray->GetResource()](FRHICommandListImmediate& RHICmdList)
		{
			FRHITexture* RHI = Resource->GetTextureRHI();
			if (!ensure(RHI))
			{
				return;
			}

			for (int32 MipIndex = 0; MipIndex < NumMips; MipIndex++)
			{
				VOXEL_SCOPE_COUNTER_FORMAT("LockTexture2DArray %s Mip=%d Texture=%d", *DebugName.ToString(), MipIndex, TextureIndex);

				const int32 MipTextureSize = TextureSize >> MipIndex;
				ensure(MipTextureSize >= 16);

				const FPixelFormatInfo Info = GPixelFormats[PixelFormat];
				ensure(MipTextureSize % Info.BlockSizeX == 0);
				ensure(MipTextureSize % Info.BlockSizeY == 0);
				ensure(Info.BlockSizeZ == 1);

				const int32 NumBlocksX = MipTextureSize / Info.BlockSizeX;
				const int32 NumBlocksY = MipTextureSize / Info.BlockSizeY;

				uint32 Stride = 0;
				void* Data = RHICmdList.LockTexture2DArray(RHI, TextureIndex, MipIndex, RLM_WriteOnly, Stride, false);
				ON_SCOPE_EXIT
				{
					RHICmdList.UnlockTexture2DArray(RHI, TextureIndex, MipIndex, false);
				};

				VOXEL_SCOPE_COUNTER("Memcpy");

				void* SourceData = MipData.Num() == 0 ? nullptr : MipData[MipIndex];
				for (int32 Y = 0; Y < NumBlocksY; Y++)
				{
					const int64 NumBytes = NumBlocksX * Info.BlockBytes;
					ensure(NumBytes <= Stride);

					if (SourceData)
					{
						FMemory::Memcpy(
							static_cast<uint8*>(Data) + Y * Stride,
							static_cast<uint8*>(SourceData) + Y * NumBytes,
							NumBytes);
					}
					else
					{
						FMemory::Memzero(
							static_cast<uint8*>(Data) + Y * Stride,
							NumBytes);
					}
				}
			}

			for (void* Data : MipData)
			{
				FMemory::Free(Data);
			}
		});
	}
}

#if WITH_EDITOR
void UMaterialExpressionSampleVoxelTextureParameter::CompileVoxel(
	FMaterialCompiler& Compiler,
	UMaterialExpressionCustom& Custom,
	const FVoxelMaterialParameterData& ParameterData,
	const FName Name,
	const int32 MaterialId,
	const int32 PreviewMaterialId,
	TArray<int32>& Inputs)
{
	if (!ensure(ParameterData.IsA<FVoxelMaterialParameterData_TextureArray>()))
	{
		Compiler.Error(TEXT("Invalid parameter data"));
		return;
	}

	const FString SampleFunction = INLINE_LAMBDA
	{
		switch (CastChecked<FVoxelMaterialParameterData_TextureArray>(ParameterData).Compression)
		{
		default: ensure(false);
		case EVoxelTextureArrayCompression::DXT1:
		case EVoxelTextureArrayCompression::DXT5:
		{
			return "ProcessMaterialColorTextureLookup";
		}
		case EVoxelTextureArrayCompression::BC5:
		{
			return "UnpackNormalMap";
		}
		}
	};

	Custom.OutputType = CMOT_Float4;

	if (MaterialId == -1)
	{
		Custom.Code = R"(
#if PIXEL_PARAMETERS_HAS_VoxelDetailTexture_MaterialIds_LerpAlphas
	const uint IndexA = asuint(IndirectionTexture[uint2(Parameters.VoxelDetailTexture_MaterialIds.x, 0)].r);
	const uint IndexB = asuint(IndirectionTexture[uint2(Parameters.VoxelDetailTexture_MaterialIds.y, 0)].r);
	const uint IndexC = asuint(IndirectionTexture[uint2(Parameters.VoxelDetailTexture_MaterialIds.z, 0)].r);

	// No derivatives in raytraced shaders, need to use SampleLevel
	#if USE_FORCE_TEXTURE_MIP
		return lerp(lerp(
			UNPACKSAMPLE(TextureArray.SampleLevel(TextureArraySampler, float3(UVs, IndexA), 0)),
			UNPACKSAMPLE(TextureArray.SampleLevel(TextureArraySampler, float3(UVs, IndexB), 0)), Parameters.VoxelDetailTexture_LerpAlphas.x),
			UNPACKSAMPLE(TextureArray.SampleLevel(TextureArraySampler, float3(UVs, IndexC), 0)), Parameters.VoxelDetailTexture_LerpAlphas.y);
	#else
		return lerp(lerp(
			UNPACKSAMPLE(TextureArray.Sample(TextureArraySampler, float3(UVs, IndexA))),
			UNPACKSAMPLE(TextureArray.Sample(TextureArraySampler, float3(UVs, IndexB))), Parameters.VoxelDetailTexture_LerpAlphas.x),
			UNPACKSAMPLE(TextureArray.Sample(TextureArraySampler, float3(UVs, IndexC))), Parameters.VoxelDetailTexture_LerpAlphas.y);
	#endif
#else
	// No derivatives in raytraced shaders, need to use SampleLevel
	#if USE_FORCE_TEXTURE_MIP
		const uint Index = asuint(IndirectionTexture[uint2(PreviewMaterialId, 0)].r);
		return UNPACKSAMPLE(TextureArray.SampleLevel(TextureArraySampler, float3(UVs, Index), 0));
	#else
		const uint Index = asuint(IndirectionTexture[uint2(PreviewMaterialId, 0)].r);
		return UNPACKSAMPLE(TextureArray.Sample(TextureArraySampler, float3(UVs, Index)));
	#endif
#endif
)";
	}
	else
	{
		Custom.Code = FString() + R"(
// No derivatives in raytraced shaders, need to use SampleLevel
#if USE_FORCE_TEXTURE_MIP
	const uint Index = asuint(IndirectionTexture[uint2(MaterialId, 0)].r);
	return UNPACKSAMPLE(TextureArray.SampleLevel(TextureArraySampler, float3(UVs, Index), 0));
#else
	const uint Index = asuint(IndirectionTexture[uint2(MaterialId, 0)].r);
	return UNPACKSAMPLE(TextureArray.Sample(TextureArraySampler, float3(UVs, Index)));
#endif
)";

		Inputs.Add(MaterialId);
		Custom.Inputs.Add({ "MaterialId" });
	}

	Custom.Code.ReplaceInline(TEXT("UNPACKSAMPLE"), *SampleFunction);

	Inputs.Add(PreviewMaterialId);
	Custom.Inputs.Add({ "PreviewMaterialId" });

	Inputs.Add(Compiler.TextureParameter(Name + "_IndirectionTexture", FVoxelTextureUtilities::GetDefaultTexture2D(), SAMPLERTYPE_Color, SSM_Wrap_WorldGroupSettings));
	Custom.Inputs.Add({ "IndirectionTexture" });

	Inputs.Add(Compiler.TextureParameter(Name + "_TextureArray", FVoxelTextureUtilities::GetDefaultTexture2DArray(), SAMPLERTYPE_Color, SSM_Wrap_WorldGroupSettings));
	Custom.Inputs.Add({ "TextureArray" });

	int32 UVs = UVsInput.Compile(&Compiler);
	if (UVs == -1)
	{
		UVs = Compiler.ComponentMask(
			Compiler.Div(
				Compiler.WorldPosition(WPT_Default),
				Compiler.Constant(100.f)),
			true,
			true,
			false,
			false);
	}

	Inputs.Add(UVs);
	Custom.Inputs.Add({ "UVs" });
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#if WITH_EDITOR
FName UMaterialExpressionSampleVoxelTextureParameter::GetInputName(const int32 InputIndex) const
{
	if (InputIndex == 2)
	{
		return "UVs";
	}
	else
	{
		return Super::GetInputName(InputIndex);
	}
}

uint32 UMaterialExpressionSampleVoxelTextureParameter::GetInputType(const int32 InputIndex)
{
	if (InputIndex == 2)
	{
		return MCT_Float2;
	}
	else
	{
		return Super::GetInputType(InputIndex);
	}
}

bool UMaterialExpressionSampleVoxelTextureParameter::IsInputConnectionRequired(const int32 InputIndex) const
{
	if (InputIndex == 2)
	{
		return true;
	}
	else
	{
		return Super::IsInputConnectionRequired(InputIndex);
	}
}

void UMaterialExpressionSampleVoxelTextureParameter::GetConnectorToolTip(
	const int32 InputIndex,
	const int32 OutputIndex,
	TArray<FString>& OutToolTip)
{
	if (InputIndex == 2)
	{
		OutToolTip.Add("UVs to sample the texture at, will use WorldPosition.XY / 100 if not set");
	}
	else
	{
		Super::GetConnectorToolTip(InputIndex, OutputIndex, OutToolTip);
	}
}
#endif