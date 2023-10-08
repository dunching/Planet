// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "Materials/MaterialExpressionCustomOutput.h"
#include "MaterialExpressionVoxelDetailTextureOffsetOutput.generated.h"

UCLASS()
class VOXELGRAPHCORE_API UMaterialExpressionVoxelDetailTextureOffsetOutput : public UMaterialExpressionCustomOutput
{
	GENERATED_BODY()

	UPROPERTY(meta = (ToolTip = "float2 between -1 and 1. Can be used to offset detail texture sample and make it look more natural"))
	FExpressionInput Input;

#if WITH_EDITOR
	virtual int32 Compile(FMaterialCompiler* Compiler, int32 OutputIndex) override;
	virtual void GetCaption(TArray<FString>& OutCaptions) const override;
	virtual uint32 GetInputType(int32 InputIndex) override { return MCT_Float2; }
#endif

	virtual int32 GetNumOutputs() const override { return 1; }
	virtual FString GetFunctionName() const override { return "GetVoxelDetailTextureOffset"; }
};