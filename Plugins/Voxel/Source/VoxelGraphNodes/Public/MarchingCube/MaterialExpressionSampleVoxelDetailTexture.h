// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelDetailTexture.h"
#include "Materials/MaterialExpressionCustom.h"
#include "MaterialExpressionSampleVoxelDetailTexture.generated.h"

UCLASS(Abstract)
class VOXELGRAPHNODES_API UMaterialExpressionSampleVoxelDetailTexture : public UMaterialExpression
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (RequiredInput = "false", ToolTip = "float2 between -1 and 1. Can be used to offset detail texture sample and make it look more natural"))
	FExpressionInput Offset;

	UMaterialExpressionSampleVoxelDetailTexture();

	virtual UObject* GetReferencedTexture() const override;
	virtual bool CanReferenceTexture() const override { return true; }

#if WITH_EDITOR
	virtual UVoxelDetailTexture* GetDetailTexture() const VOXEL_PURE_VIRTUAL({});
	virtual ECustomMaterialOutputType GetCustomOutputType() const VOXEL_PURE_VIRTUAL({});
	virtual void SetupCustomExpression(UMaterialExpressionCustom& Custom) const;
#endif

#if WITH_EDITOR
	//~ Begin UMaterialExpression Interface
	virtual int32 Compile(FMaterialCompiler* Compiler, int32 OutputIndex) override;
	virtual void GetCaption(TArray<FString>& OutCaptions) const override;
	//~ End UMaterialExpression Interface
#endif
};

UCLASS()
class VOXELGRAPHNODES_API UMaterialExpressionSampleVoxelFloatDetailTexture : public UMaterialExpressionSampleVoxelDetailTexture
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Config")
	TObjectPtr<UVoxelFloatDetailTexture> DetailTexture;

#if WITH_EDITOR
	virtual UVoxelDetailTexture* GetDetailTexture() const override
	{
		return DetailTexture;
	}
	virtual ECustomMaterialOutputType GetCustomOutputType() const override
	{
		return CMOT_Float1;
	}
#endif
};

UCLASS()
class VOXELGRAPHNODES_API UMaterialExpressionSampleVoxelColorDetailTexture : public UMaterialExpressionSampleVoxelDetailTexture
{
	GENERATED_BODY()

public:
	UMaterialExpressionSampleVoxelColorDetailTexture();

	UPROPERTY(EditAnywhere, Category = "Config")
	TObjectPtr<UVoxelColorDetailTexture> DetailTexture;

#if WITH_EDITOR
	virtual UVoxelDetailTexture* GetDetailTexture() const override
	{
		return DetailTexture;
	}
	virtual ECustomMaterialOutputType GetCustomOutputType() const override
	{
		return CMOT_Float4;
	}

	virtual uint32 GetOutputType(const int32 OutputIndex) override
	{
		return OutputIndex == 0 ? MCT_Float4 : MCT_Float;
	}
	virtual int32 Compile(FMaterialCompiler* Compiler, int32 OutputIndex) override;
#endif
};

UCLASS()
class VOXELGRAPHNODES_API UMaterialExpressionSampleVoxelMaterialIdDetailTexture : public UMaterialExpressionSampleVoxelDetailTexture
{
	GENERATED_BODY()

public:
	UMaterialExpressionSampleVoxelMaterialIdDetailTexture();

	UPROPERTY(EditAnywhere, Category = "Config")
	TObjectPtr<UVoxelMaterialIdDetailTexture> DetailTexture;

#if WITH_EDITOR
	virtual UVoxelDetailTexture* GetDetailTexture() const override
	{
		return DetailTexture;
	}
	virtual ECustomMaterialOutputType GetCustomOutputType() const override
	{
		return CMOT_Float3;
	}
	virtual uint32 GetOutputType(int32 OutputIndex) override
	{
		return MCT_Float;
	}

	virtual void SetupCustomExpression(UMaterialExpressionCustom& Custom) const override;
	virtual int32 Compile(FMaterialCompiler* Compiler, int32 OutputIndex) override;
#endif
};

UCLASS()
class VOXELGRAPHNODES_API UMaterialExpressionGetVoxelMaterialId : public UMaterialExpression
{
	GENERATED_BODY()

public:
	UMaterialExpressionGetVoxelMaterialId();

#if WITH_EDITOR
	//~ Begin UMaterialExpression Interface
	virtual uint32 GetOutputType(int32 OutputIndex) override
	{
		return MCT_Float;
	}
	virtual int32 Compile(FMaterialCompiler* Compiler, int32 OutputIndex) override;
	virtual void GetCaption(TArray<FString>& OutCaptions) const override;
	//~ End UMaterialExpression Interface
#endif
};