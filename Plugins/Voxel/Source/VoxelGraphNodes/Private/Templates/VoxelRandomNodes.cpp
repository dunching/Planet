// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Templates/VoxelRandomNodes.h"
#include "VoxelFunctionNode.h"

FVoxelRandomVectorSettings UVoxelRandomNodesFunctionLibrary::MakeRandomVectorSettings(const EVoxelRandomVectorType Type, const FVoxelFloatRange& RangeX, const FVoxelFloatRange& RangeY, const FVoxelFloatRange& RangeZ) const
{
	FVoxelRandomVectorSettings Result;
	Result.Type = Type;
	Result.RangeX = RangeX;
	Result.RangeY = RangeY;
	Result.RangeZ = RangeZ;
	return Result;
}

EVoxelRandomVectorType UVoxelRandomNodesFunctionLibrary::BreakRandomVectorSettings(const FVoxelRandomVectorSettings& Value, FVoxelFloatRange& RangeX, FVoxelFloatRange& RangeY, FVoxelFloatRange& RangeZ) const
{
	RangeX = Value.RangeX;
	RangeY = Value.RangeY;
	RangeZ = Value.RangeZ;
	return Value.Type;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelTemplateNode::FPin* FVoxelTemplateNode_RandomVector::ExpandPins(FNode& Node, TArray<FPin*> Pins, const TArray<FPin*>& AllPins) const
{
	TArray<FPin*> ExpandedPins = BreakNode(Pins[0], FVoxelFunctionNode::Make(FindUFunctionChecked(UVoxelRandomNodesFunctionLibrary, BreakRandomVectorSettings)), 4);
	ExpandedPins.Add(Pins[1]);
	return Call_Single<FVoxelNode_RandomVector>(ExpandedPins);
}