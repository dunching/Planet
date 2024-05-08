// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelISPCNodeHelpers.isph"

export void VoxelNode_VoxelNode_Lerp(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float A = LoadFloat(Buffers[0], Index);
		const float B = LoadFloat(Buffers[1], Index);
		const float Alpha = LoadFloat(Buffers[2], Index);
		float ReturnValue;

		ReturnValue = lerp(A, B, Alpha);

		StoreFloat(Buffers[3], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_SafeLerp(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float A = LoadFloat(Buffers[0], Index);
		const float B = LoadFloat(Buffers[1], Index);
		const float Alpha = LoadFloat(Buffers[2], Index);
		float ReturnValue;

		ReturnValue = lerp(A, B, clamp(Alpha, 0.f, 1.f));

		StoreFloat(Buffers[3], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_SmoothStep(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float A = LoadFloat(Buffers[0], Index);
		const float B = LoadFloat(Buffers[1], Index);
		const float Alpha = LoadFloat(Buffers[2], Index);
		float ReturnValue;

		ReturnValue = SmoothStep(A, B, Alpha);

		StoreFloat(Buffers[3], Index, ReturnValue);
	}
}