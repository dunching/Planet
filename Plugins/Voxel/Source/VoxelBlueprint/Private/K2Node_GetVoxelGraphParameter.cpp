// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "K2Node_GetVoxelGraphParameter.h"
#include "FunctionLibrary/VoxelParameterFunctionLibrary.h"

UK2Node_GetVoxelGraphParameter::UK2Node_GetVoxelGraphParameter()
{
	FunctionReference.SetExternalMember(GET_FUNCTION_NAME_CHECKED(UVoxelParameterFunctionLibrary, K2_GetVoxelParameter), UVoxelParameterFunctionLibrary::StaticClass());
}

bool UK2Node_GetVoxelGraphParameter::IsPinWildcard(const UEdGraphPin& Pin) const
{
	return Pin.PinName == STATIC_FNAME("Value");
}

UEdGraphPin* UK2Node_GetVoxelGraphParameter::GetParameterNamePin() const
{
	return FindPin(STATIC_FNAME("Name"));
}