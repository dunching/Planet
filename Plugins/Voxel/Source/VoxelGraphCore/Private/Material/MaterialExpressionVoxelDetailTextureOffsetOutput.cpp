// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Material/MaterialExpressionVoxelDetailTextureOffsetOutput.h"
#include "MaterialCompiler.h"

#if WITH_EDITOR
int32 UMaterialExpressionVoxelDetailTextureOffsetOutput::Compile(FMaterialCompiler* Compiler, const int32 OutputIndex)
{
	const int32 Value = Input.Compile(Compiler);
	if (Value == -1)
	{
		return Compiler->Constant2(0.f, 0.f);
	}
	return Compiler->CustomOutput(this, OutputIndex, Value);
}

void UMaterialExpressionVoxelDetailTextureOffsetOutput::GetCaption(TArray<FString>& OutCaptions) const
{
	OutCaptions.Add("Voxel Detail Texture Offset Output");
}
#endif