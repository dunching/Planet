// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Material/VoxelMaterialNodes.h"

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_MakeMaterial, OutMaterial)
{
	const TValue<FVoxelMaterial> Material = Get(MaterialPin, Query);
	const TVoxelArray<TValue<FVoxelMaterialParameter>> Parameters = Get(ParameterPins, Query);

	return VOXEL_ON_COMPLETE(Material, Parameters)
	{
		FVoxelMaterial Result;
		Result.ParentMaterial = Material->ParentMaterial;
		Result.Parameters = Material->Parameters;

		for (const TSharedRef<const FVoxelMaterialParameter>& Parameter : Parameters)
		{
			if (Parameter->GetStruct() == FVoxelMaterialParameter::StaticStruct())
			{
				continue;
			}
			Result.Parameters.Add(Parameter);
		}

		return Result;
	};
}