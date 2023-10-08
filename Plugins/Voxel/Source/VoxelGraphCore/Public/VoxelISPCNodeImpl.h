// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelISPCNode.h"

struct VOXELGRAPHCORE_API FVoxelISPCNodeImpl
{
	static int32 GetRegisterWidth(const FVoxelPinType& Type);
	static FVoxelPinType GetRegisterType(const FVoxelPinType& Type);

	static void FormatCode(FString& Code);
	static FString GenerateFunction(const FVoxelISPCNode& Node, TSet<FString>& OutIncludes);
};