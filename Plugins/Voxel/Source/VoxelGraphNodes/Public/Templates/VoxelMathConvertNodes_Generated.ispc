// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelISPCNodeHelpers.isph"

export void VoxelNode_VoxelNode_Ceil_DoubleToDouble(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const double Value = LoadDouble(Buffers[0], Index);
		double ReturnValue;

		ReturnValue = ceil(Value);

		StoreDouble(Buffers[1], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Ceil_DoubleToInt(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const double Value = LoadDouble(Buffers[0], Index);
		int32 ReturnValue;

		ReturnValue = (int)ceil(Value);

		StoreInt32(Buffers[1], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Ceil_FloatToFloat(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float Value = LoadFloat(Buffers[0], Index);
		float ReturnValue;

		ReturnValue = ceil(Value);

		StoreFloat(Buffers[1], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Ceil_FloatToInt(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float Value = LoadFloat(Buffers[0], Index);
		int32 ReturnValue;

		ReturnValue = (int)ceil(Value);

		StoreInt32(Buffers[1], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Floor_DoubleToDouble(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const double Value = LoadDouble(Buffers[0], Index);
		double ReturnValue;

		ReturnValue = floor(Value);

		StoreDouble(Buffers[1], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Floor_DoubleToInt(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const double Value = LoadDouble(Buffers[0], Index);
		int32 ReturnValue;

		ReturnValue = (int)floor(Value);

		StoreInt32(Buffers[1], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Floor_FloatToFloat(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float Value = LoadFloat(Buffers[0], Index);
		float ReturnValue;

		ReturnValue = floor(Value);

		StoreFloat(Buffers[1], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Floor_FloatToInt(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float Value = LoadFloat(Buffers[0], Index);
		int32 ReturnValue;

		ReturnValue = (int)floor(Value);

		StoreInt32(Buffers[1], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Round_DoubleToDouble(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const double Value = LoadDouble(Buffers[0], Index);
		double ReturnValue;

		ReturnValue = round(Value);

		StoreDouble(Buffers[1], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Round_DoubleToInt(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const double Value = LoadDouble(Buffers[0], Index);
		int32 ReturnValue;

		ReturnValue = (int)round(Value);

		StoreInt32(Buffers[1], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Round_FloatToFloat(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float Value = LoadFloat(Buffers[0], Index);
		float ReturnValue;

		ReturnValue = round(Value);

		StoreFloat(Buffers[1], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Round_FloatToInt(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float Value = LoadFloat(Buffers[0], Index);
		int32 ReturnValue;

		ReturnValue = (int)round(Value);

		StoreInt32(Buffers[1], Index, ReturnValue);
	}
}