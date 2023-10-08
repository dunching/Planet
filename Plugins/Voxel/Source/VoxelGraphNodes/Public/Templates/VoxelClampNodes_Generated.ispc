// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelISPCNodeHelpers.isph"

export void VoxelNode_VoxelNode_Clamp_Double(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const double Value = LoadDouble(Buffers[0], Index);
		const double Min = LoadDouble(Buffers[1], Index);
		const double Max = LoadDouble(Buffers[2], Index);
		double ReturnValue;

		ReturnValue = clamp(Value, Min, Max);

		StoreDouble(Buffers[3], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Clamp_Float(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float Value = LoadFloat(Buffers[0], Index);
		const float Min = LoadFloat(Buffers[1], Index);
		const float Max = LoadFloat(Buffers[2], Index);
		float ReturnValue;

		ReturnValue = clamp(Value, Min, Max);

		StoreFloat(Buffers[3], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Clamp_Int(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const int32 Value = LoadInt32(Buffers[0], Index);
		const int32 Min = LoadInt32(Buffers[1], Index);
		const int32 Max = LoadInt32(Buffers[2], Index);
		int32 ReturnValue;

		ReturnValue = clamp(Value, Min, Max);

		StoreInt32(Buffers[3], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_MappedRangeValueClamped_Double(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const double Value = LoadDouble(Buffers[0], Index);
		const double InMin = LoadDouble(Buffers[1], Index);
		const double InMax = LoadDouble(Buffers[2], Index);
		const double OutMin = LoadDouble(Buffers[3], Index);
		const double OutMax = LoadDouble(Buffers[4], Index);
		double ReturnValue;

		ReturnValue = lerp(OutMin, OutMax, clamp((Value - InMin) / (InMax - InMin), 0.d, 1.d));

		StoreDouble(Buffers[5], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_MappedRangeValueClamped_Float(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float Value = LoadFloat(Buffers[0], Index);
		const float InMin = LoadFloat(Buffers[1], Index);
		const float InMax = LoadFloat(Buffers[2], Index);
		const float OutMin = LoadFloat(Buffers[3], Index);
		const float OutMax = LoadFloat(Buffers[4], Index);
		float ReturnValue;

		ReturnValue = lerp(OutMin, OutMax, clamp((Value - InMin) / (InMax - InMin), 0.f, 1.f));

		StoreFloat(Buffers[5], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_MappedRangeValueUnclamped_Double(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const double Value = LoadDouble(Buffers[0], Index);
		const double InMin = LoadDouble(Buffers[1], Index);
		const double InMax = LoadDouble(Buffers[2], Index);
		const double OutMin = LoadDouble(Buffers[3], Index);
		const double OutMax = LoadDouble(Buffers[4], Index);
		double ReturnValue;

		ReturnValue = lerp(OutMin, OutMax, (Value - InMin) / (InMax - InMin));

		StoreDouble(Buffers[5], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_MappedRangeValueUnclamped_Float(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float Value = LoadFloat(Buffers[0], Index);
		const float InMin = LoadFloat(Buffers[1], Index);
		const float InMax = LoadFloat(Buffers[2], Index);
		const float OutMin = LoadFloat(Buffers[3], Index);
		const float OutMax = LoadFloat(Buffers[4], Index);
		float ReturnValue;

		ReturnValue = lerp(OutMin, OutMax, (Value - InMin) / (InMax - InMin));

		StoreFloat(Buffers[5], Index, ReturnValue);
	}
}