// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelISPCNodeHelpers.isph"

export void VoxelNode_VoxelNode_Frac(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float Value = LoadFloat(Buffers[0], Index);
		float ReturnValue;

		ReturnValue = Value - floor(Value);

		StoreFloat(Buffers[1], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Power(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float Base = LoadFloat(Buffers[0], Index);
		const float Exp = LoadFloat(Buffers[1], Index);
		float ReturnValue;

		ReturnValue = pow(abs(Base), Exp);

		StoreFloat(Buffers[2], Index, ReturnValue);
	}
}