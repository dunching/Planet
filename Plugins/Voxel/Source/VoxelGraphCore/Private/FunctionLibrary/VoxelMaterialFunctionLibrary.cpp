// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "FunctionLibrary/VoxelMaterialFunctionLibrary.h"

TVoxelFutureValue<FVoxelComputedMaterialParameter> FVoxelMaterialScalarParameter::Compute(const FVoxelQuery& Query) const
{
	FVoxelComputedMaterialParameter Parameters;
	Parameters.ScalarParameters.Add(Name, Value);
	return Parameters;
}

TVoxelFutureValue<FVoxelComputedMaterialParameter> FVoxelMaterialVectorParameter::Compute(const FVoxelQuery& Query) const
{
	FVoxelComputedMaterialParameter Parameters;
	Parameters.VectorParameters.Add(Name, Value);
	return Parameters;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelMaterialScalarParameter UVoxelMaterialFunctionLibrary::MakeScalarParameter(
	const FName Name,
	const float Value) const
{
	FVoxelMaterialScalarParameter Parameter;
	Parameter.Node = GetNodeRef();
	Parameter.Name = Name;
	Parameter.Value = Value;
	return Parameter;
}

FVoxelMaterialVectorParameter UVoxelMaterialFunctionLibrary::MakeVectorParameter(
	const FName Name,
	const FVector4& Value) const
{
	FVoxelMaterialVectorParameter Parameter;
	Parameter.Node = GetNodeRef();
	Parameter.Name = Name;
	Parameter.Value = Value;
	return Parameter;
}