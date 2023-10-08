// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelNodeHelpers.h"
#include "VoxelNode.h"

#if WITH_EDITOR
FString FVoxelNodeDefaultValueHelper::MakeObject(const FVoxelPinType& RuntimeType, const FString& Path)
{
#if VOXEL_DEBUG
	FVoxelPinValue Value(RuntimeType.GetPinDefaultValueType());
	check(Value.IsObject());

	Value.GetObject() = FSoftObjectPtr(Path).LoadSynchronous();

	check(
		Value.GetObject() &&
		Value.GetObject().IsA(Value.GetType().GetObjectClass()));
#endif
	return Path;
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelNodeHelpers::RaiseQueryError(const FVoxelGraphNodeRef& Node, const UScriptStruct* QueryType)
{
	VOXEL_MESSAGE(Error, "{0}: {1} is required but not provided by callee", Node, QueryType->GetName());
}

void FVoxelNodeHelpers::RaiseBufferError(const FVoxelGraphNodeRef& Node)
{
	VOXEL_MESSAGE(Error, "{0}: Inputs have different buffer sizes", Node);
}