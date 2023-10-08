// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelParameterNode.h"
#include "VoxelParameterValues.h"

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_Parameter, Value)
{
	const TSharedPtr<const FVoxelParameterValues>& ParameterValues = Query.GetContext().ParameterValues;
	if (!ensureVoxelSlow(ParameterValues))
	{
		VOXEL_MESSAGE(Error, "{0}: INTERNAL ERROR: Invalid parameter: {1}", this, ParameterName);
		return {};
	}

	return ParameterValues->FindParameter(
		ReturnPinType,
		Query.GetContext().ParameterPath.MakeChild(ParameterGuid),
		Query);
}