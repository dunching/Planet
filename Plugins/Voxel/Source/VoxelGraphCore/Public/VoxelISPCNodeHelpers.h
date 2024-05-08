// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"

namespace ispc
{
#define __ISPC_STRUCT_FVoxelBuffer__
	struct FVoxelBuffer
	{
		void* Data = nullptr;
		bool bIsConstant = false;
	};
}

using FVoxelNodeISPCPtr = void (*)(const ispc::FVoxelBuffer* Buffers, int32 Num);

extern VOXELGRAPHCORE_API TMap<FName, FVoxelNodeISPCPtr> GVoxelNodeISPCPtrs;

#define REGISTER_VOXEL_NODE_ISPC(HeaderName, Name) \
	namespace ispc \
	{ \
		extern "C" \
		{ \
			extern void VoxelNode_ ## Name(const FVoxelBuffer* Buffers, const int32 Num); \
		} \
	} \
	VOXEL_RUN_ON_STARTUP_GAME(Register_ ## Name) \
	{ \
		check(!GVoxelNodeISPCPtrs.Contains(#Name)); \
		GVoxelNodeISPCPtrs.Add(#Name, ispc::VoxelNode_ ## Name); \
	}