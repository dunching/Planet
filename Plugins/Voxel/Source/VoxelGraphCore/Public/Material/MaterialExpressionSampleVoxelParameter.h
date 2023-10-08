// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelPinType.h"
#include "VoxelPinValue.h"
#include "VoxelMaterialDefinitionInterface.h"
#include "Materials/MaterialExpressionCustom.h"
#include "MaterialExpressionSampleVoxelParameter.generated.h"

class UVoxelMaterialDefinition;

DECLARE_VOXEL_MEMORY_STAT(VOXELGRAPHCORE_API, STAT_VoxelMaterialTextureMemory, "Voxel Material Texture Memory (GPU)");

UCLASS()
class VOXELGRAPHCORE_API UMaterialExpressionVoxelParameter : public UMaterialExpression
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Config")
	TObjectPtr<UVoxelMaterialDefinition> MaterialDefinition;

	UPROPERTY(EditAnywhere, Category = "Config")
	FGuid ParameterGuid;

public:
	UMaterialExpressionVoxelParameter();

	//~ Begin UMaterialExpression Interface
	virtual UObject* GetReferencedTexture() const override;
	virtual bool CanReferenceTexture() const override { return true; }

#if WITH_EDITOR
	virtual uint32 GetOutputType(int32 OutputIndex) override;
	virtual int32 Compile(FMaterialCompiler* Compiler, int32 OutputIndex) override;
	virtual void GetCaption(TArray<FString>& OutCaptions) const override;
#endif
	//~ End UMaterialExpression Interface
};

UCLASS(Abstract)
class VOXELGRAPHCORE_API UMaterialExpressionSampleVoxelParameter : public UMaterialExpression
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FExpressionInput ParameterInput;

	UPROPERTY()
	FExpressionInput MaterialIdInput;

public:
	UMaterialExpressionSampleVoxelParameter();

	static const UMaterialExpressionSampleVoxelParameter* GetTemplate(const FVoxelPinType& Type);

public:
	virtual FVoxelPinType GetVoxelParameterType() const VOXEL_PURE_VIRTUAL({});
	virtual UScriptStruct* GetVoxelParameterDataType() const VOXEL_PURE_VIRTUAL({});

	struct FInstance
	{
		UObject* DebugObject = nullptr;
		int32 Index = 0;
		FVoxelTerminalPinValue Value;
	};
	virtual void UpdateVoxelParameterData(
		FName DebugName,
		const TVoxelArray<FInstance>& Instances,
		TVoxelInstancedStruct<FVoxelMaterialParameterData>& InOutParameterData) const VOXEL_PURE_VIRTUAL();

#if WITH_EDITOR
	virtual void CompileVoxel(
		FMaterialCompiler& Compiler,
		UMaterialExpressionCustom& Custom,
		const FVoxelMaterialParameterData& ParameterData,
		FName Name,
		int32 MaterialId,
		int32 PreviewMaterialId,
		TArray<int32>& Inputs) VOXEL_PURE_VIRTUAL();
#endif

public:
	//~ Begin UMaterialExpression Interface
	virtual UObject* GetReferencedTexture() const final override;
	virtual bool CanReferenceTexture() const final override { return true; }

#if WITH_EDITOR
#if VOXEL_ENGINE_VERSION >= 503
	virtual TArrayView<FExpressionInput*> GetInputsView() override;
#else
	virtual const TArray<FExpressionInput*> GetInputs() override;
#endif
	virtual FExpressionInput* GetInput(int32 InputIndex) final override;
	virtual FName GetInputName(int32 InputIndex) const override;
	virtual uint32 GetInputType(int32 InputIndex) override;
	virtual bool IsInputConnectionRequired(int32 InputIndex) const override;
	virtual void GetConnectorToolTip(int32 InputIndex, int32 OutputIndex, TArray<FString>& OutToolTip) override;

	virtual int32 Compile(FMaterialCompiler* Compiler, int32 OutputIndex) override;
	virtual void GetCaption(TArray<FString>& OutCaptions) const override;
#endif
	//~ End UMaterialExpression Interface

protected:
#if VOXEL_ENGINE_VERSION < 503
#if WITH_EDITOR
	TArray<FExpressionInput*> CachedInputs;
#endif
#endif
};