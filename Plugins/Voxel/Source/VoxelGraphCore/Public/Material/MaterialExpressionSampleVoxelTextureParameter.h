// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "Engine/Texture2D.h"
#include "Material/MaterialExpressionSampleVoxelParameter.h"
#include "MaterialExpressionSampleVoxelTextureParameter.generated.h"

UENUM()
enum class EVoxelTextureArrayCompression : uint8
{
	DXT1,
	DXT5,
	BC5,
};

USTRUCT()
struct FVoxelMaterialParameterData_TextureArray : public FVoxelMaterialParameterData
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Config")
	int32 TextureSize = 1024;

	UPROPERTY(EditAnywhere, Category = "Config")
	int32 LastMipTextureSize = 16;

	UPROPERTY(EditAnywhere, Category = "Config")
	EVoxelTextureArrayCompression Compression = EVoxelTextureArrayCompression::DXT1;

	int32 GetNumMips() const
	{
		ensureVoxelSlow(TextureSize % LastMipTextureSize == 0);
		return 1 + FVoxelUtilities::ExactLog2(TextureSize / LastMipTextureSize);
	}

	virtual void Fixup() override;

public:
	UPROPERTY(Transient)
	TObjectPtr<UTexture2D> IndirectionTexture;

	UPROPERTY(Transient)
	TObjectPtr<UTexture2DArray> TextureArray;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UTexture2D>> TextureArrayValues;

	virtual void CacheParameters(
		FName Name,
		FCachedParameters& InOutParameters) const override;
};

UCLASS()
class VOXELGRAPHCORE_API UMaterialExpressionSampleVoxelTextureParameter : public UMaterialExpressionSampleVoxelParameter
{
	GENERATED_BODY()

public:
	UMaterialExpressionSampleVoxelTextureParameter();

	UPROPERTY()
	FExpressionInput UVsInput;

	virtual FVoxelPinType GetVoxelParameterType() const override
	{
		return FVoxelPinType::Make<UTexture2D>();
	}
	virtual UScriptStruct* GetVoxelParameterDataType() const override
	{
		return FVoxelMaterialParameterData_TextureArray::StaticStruct();
	}
	virtual void UpdateVoxelParameterData(
		FName DebugName,
		const TVoxelArray<FInstance>& Instances,
		TVoxelInstancedStruct<FVoxelMaterialParameterData>& InOutParameterData) const override;

#if WITH_EDITOR
	virtual void CompileVoxel(
		FMaterialCompiler& Compiler,
		UMaterialExpressionCustom& Custom,
		const FVoxelMaterialParameterData& ParameterData,
		FName Name,
		int32 MaterialId,
		int32 PreviewMaterialId,
		TArray<int32>& Inputs) override;
#endif

	//~ Begin UMaterialExpression Interface
#if WITH_EDITOR
	virtual FName GetInputName(int32 InputIndex) const override;
	virtual uint32 GetInputType(int32 InputIndex) override;
	virtual bool IsInputConnectionRequired(int32 InputIndex) const override;
	virtual void GetConnectorToolTip(int32 InputIndex, int32 OutputIndex, TArray<FString>& OutToolTip) override;
#endif
	//~ End UMaterialExpression Interface
};