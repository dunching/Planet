// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelPinValueInterface.generated.h"

struct FVoxelParameterPath;

USTRUCT()
struct FVoxelPinValueInterface : public FVoxelVirtualStruct
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

	virtual void Fixup(UObject* Outer) {}
	virtual void ComputeRuntimeData() {}
	virtual void ComputeExposedData() {}
};