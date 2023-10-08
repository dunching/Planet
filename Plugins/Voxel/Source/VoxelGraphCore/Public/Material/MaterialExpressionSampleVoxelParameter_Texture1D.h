// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "Material/MaterialExpressionSampleVoxelParameter.h"
#include "MaterialExpressionSampleVoxelParameter_Texture1D.generated.h"

USTRUCT()
struct FVoxelMaterialParameterData_Texture1D : public FVoxelMaterialParameterData
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

	VOXEL_ALLOCATED_SIZE_TRACKER_CUSTOM(STAT_VoxelMaterialTextureMemory, TextureMemory);

	UPROPERTY(Transient)
	TObjectPtr<UTexture2D> Texture;

	UPROPERTY(Transient)
	TArray<uint8> Data;

	virtual void CacheParameters(
		FName Name,
		FCachedParameters& InOutParameters) const override;
};

UCLASS(Abstract)
class VOXELGRAPHCORE_API UMaterialExpressionSampleVoxelParameter_Texture1D : public UMaterialExpressionSampleVoxelParameter
{
	GENERATED_BODY()

public:
	virtual EPixelFormat GetVoxelTexturePixelFormat() const VOXEL_PURE_VIRTUAL({});
	virtual void CopyVoxelInstanceData(TVoxelArrayView<uint8> OutData, const FVoxelTerminalPinValue& Value) const VOXEL_PURE_VIRTUAL();

	virtual UScriptStruct* GetVoxelParameterDataType() const final override
	{
		return FVoxelMaterialParameterData_Texture1D::StaticStruct();
	}
	virtual void UpdateVoxelParameterData(
		FName DebugName,
		const TVoxelArray<FInstance>& Instances,
		TVoxelInstancedStruct<FVoxelMaterialParameterData>& InOutParameterData) const final override;

#if WITH_EDITOR
	virtual void CompileVoxel(
		FMaterialCompiler& Compiler,
		UMaterialExpressionCustom& Custom,
		const FVoxelMaterialParameterData& ParameterData,
		FName Name,
		int32 MaterialId,
		int32 PreviewMaterialId,
		TArray<int32>& Inputs) override;

	virtual ECustomMaterialOutputType GetCustomOutputType() const VOXEL_PURE_VIRTUAL({});
	virtual FString GenerateHLSL(const FString& Value)const VOXEL_PURE_VIRTUAL({});
#endif
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

UCLASS()
class VOXELGRAPHCORE_API UMaterialExpressionGetVoxelScalarParameter : public UMaterialExpressionSampleVoxelParameter_Texture1D
{
	GENERATED_BODY()

public:
	virtual EPixelFormat GetVoxelTexturePixelFormat() const override
	{
		return PF_R32_FLOAT;
	}
	virtual void CopyVoxelInstanceData(const TVoxelArrayView<uint8> OutData, const FVoxelTerminalPinValue& Value) const override
	{
		FromByteVoxelArrayView<float>(OutData) = Value.Get<float>();
	}
	virtual FVoxelPinType GetVoxelParameterType() const override
	{
		return FVoxelPinType::Make<float>();
	}

#if WITH_EDITOR
	virtual ECustomMaterialOutputType GetCustomOutputType() const override
	{
		return CMOT_Float1;
	}
	virtual FString GenerateHLSL(const FString& Value) const override
	{
		return Value + ".r";
	}
#endif
};

UCLASS(DisplayName = "Get Voxel Vector2 Parameter")
class VOXELGRAPHCORE_API UMaterialExpressionGetVoxelVector2Parameter : public UMaterialExpressionSampleVoxelParameter_Texture1D
{
	GENERATED_BODY()

public:
	virtual EPixelFormat GetVoxelTexturePixelFormat() const override
	{
		return PF_G32R32F;
	}
	virtual void CopyVoxelInstanceData(const TVoxelArrayView<uint8> OutData, const FVoxelTerminalPinValue& Value) const override
	{
		FromByteVoxelArrayView<FVector2f>(OutData) = FVector2f(Value.Get<FVector2D>());
	}
	virtual FVoxelPinType GetVoxelParameterType() const override
	{
		return FVoxelPinType::Make<FVector2D>();
	}

#if WITH_EDITOR
	virtual ECustomMaterialOutputType GetCustomOutputType() const override
	{
		return CMOT_Float2;
	}
	virtual FString GenerateHLSL(const FString& Value) const override
	{
		return Value + ".rg";
	}
#endif
};

UCLASS(DisplayName = "Get Voxel Vector3 Parameter")
class VOXELGRAPHCORE_API UMaterialExpressionGetVoxelVector3Parameter : public UMaterialExpressionSampleVoxelParameter_Texture1D
{
	GENERATED_BODY()

public:
	virtual EPixelFormat GetVoxelTexturePixelFormat() const override
	{
		return PF_R32G32B32F;
	}
	virtual void CopyVoxelInstanceData(const TVoxelArrayView<uint8> OutData, const FVoxelTerminalPinValue& Value) const override
	{
		FromByteVoxelArrayView<FVector3f>(OutData) = FVector3f(Value.Get<FVector>());
	}
	virtual FVoxelPinType GetVoxelParameterType() const override
	{
		return FVoxelPinType::Make<FVector>();
	}

#if WITH_EDITOR
	virtual ECustomMaterialOutputType GetCustomOutputType() const override
	{
		return CMOT_Float3;
	}
	virtual FString GenerateHLSL(const FString& Value) const override
	{
		return Value + ".rgb";
	}
#endif
};

UCLASS()
class VOXELGRAPHCORE_API UMaterialExpressionGetVoxelColorParameter : public UMaterialExpressionSampleVoxelParameter_Texture1D
{
	GENERATED_BODY()

public:
	UMaterialExpressionGetVoxelColorParameter();

	virtual EPixelFormat GetVoxelTexturePixelFormat() const override
	{
		return PF_A32B32G32R32F;
	}
	virtual void CopyVoxelInstanceData(const TVoxelArrayView<uint8> OutData, const FVoxelTerminalPinValue& Value) const override
	{
		FromByteVoxelArrayView<FLinearColor>(OutData) = Value.Get<FLinearColor>();
	}
	virtual FVoxelPinType GetVoxelParameterType() const override
	{
		return FVoxelPinType::Make<FLinearColor>();
	}

#if WITH_EDITOR
	virtual ECustomMaterialOutputType GetCustomOutputType() const override
	{
		return CMOT_Float4;
	}
	virtual FString GenerateHLSL(const FString& Value) const override
	{
		return Value + ".rgba";
	}
#endif
};