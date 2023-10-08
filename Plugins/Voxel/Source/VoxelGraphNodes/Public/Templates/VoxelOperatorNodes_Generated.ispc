// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelISPCNodeHelpers.isph"

export void VoxelNode_VoxelNode_Abs_Double(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const double Value = LoadDouble(Buffers[0], Index);
		double ReturnValue;

		ReturnValue = abs(Value);

		StoreDouble(Buffers[1], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Abs_Float(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float Value = LoadFloat(Buffers[0], Index);
		float ReturnValue;

		ReturnValue = abs(Value);

		StoreFloat(Buffers[1], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Abs_Int(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const int32 Value = LoadInt32(Buffers[0], Index);
		int32 ReturnValue;

		ReturnValue = abs(Value);

		StoreInt32(Buffers[1], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Add_Double(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const double A = LoadDouble(Buffers[0], Index);
		const double B = LoadDouble(Buffers[1], Index);
		double ReturnValue;

		ReturnValue = A + B;

		StoreDouble(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Add_Float(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float A = LoadFloat(Buffers[0], Index);
		const float B = LoadFloat(Buffers[1], Index);
		float ReturnValue;

		ReturnValue = A + B;

		StoreFloat(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Add_Int(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const int32 A = LoadInt32(Buffers[0], Index);
		const int32 B = LoadInt32(Buffers[1], Index);
		int32 ReturnValue;

		ReturnValue = A + B;

		StoreInt32(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Divide_Double(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const double A = LoadDouble(Buffers[0], Index);
		const double B = LoadDouble(Buffers[1], Index);
		double ReturnValue;

		ReturnValue = A / B;

		StoreDouble(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Divide_Float(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float A = LoadFloat(Buffers[0], Index);
		const float B = LoadFloat(Buffers[1], Index);
		float ReturnValue;

		ReturnValue = B != 0.f ? A / B : A > 0.f ? BIG_NUMBER : A < 0.f ? -BIG_NUMBER : 1.f;

		StoreFloat(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Divide_Int(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const int32 A = LoadInt32(Buffers[0], Index);
		const int32 B = LoadInt32(Buffers[1], Index);
		int32 ReturnValue;

		IGNORE_PERF_WARNING
		ReturnValue = B != 0 ? A / B : 0;

		StoreInt32(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Max_Double(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const double A = LoadDouble(Buffers[0], Index);
		const double B = LoadDouble(Buffers[1], Index);
		double ReturnValue;

		ReturnValue = max(A, B);

		StoreDouble(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Max_Float(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float A = LoadFloat(Buffers[0], Index);
		const float B = LoadFloat(Buffers[1], Index);
		float ReturnValue;

		ReturnValue = max(A, B);

		StoreFloat(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Max_Int(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const int32 A = LoadInt32(Buffers[0], Index);
		const int32 B = LoadInt32(Buffers[1], Index);
		int32 ReturnValue;

		ReturnValue = max(A, B);

		StoreInt32(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Min_Double(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const double A = LoadDouble(Buffers[0], Index);
		const double B = LoadDouble(Buffers[1], Index);
		double ReturnValue;

		ReturnValue = min(A, B);

		StoreDouble(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Min_Float(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float A = LoadFloat(Buffers[0], Index);
		const float B = LoadFloat(Buffers[1], Index);
		float ReturnValue;

		ReturnValue = min(A, B);

		StoreFloat(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Min_Int(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const int32 A = LoadInt32(Buffers[0], Index);
		const int32 B = LoadInt32(Buffers[1], Index);
		int32 ReturnValue;

		ReturnValue = min(A, B);

		StoreInt32(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Modulus(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const int32 A = LoadInt32(Buffers[0], Index);
		const int32 B = LoadInt32(Buffers[1], Index);
		int32 ReturnValue;

		IGNORE_PERF_WARNING
		ReturnValue = B != 0 ? A % B : 0;

		StoreInt32(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Multiply_Double(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const double A = LoadDouble(Buffers[0], Index);
		const double B = LoadDouble(Buffers[1], Index);
		double ReturnValue;

		ReturnValue = A * B;

		StoreDouble(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Multiply_Float(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float A = LoadFloat(Buffers[0], Index);
		const float B = LoadFloat(Buffers[1], Index);
		float ReturnValue;

		ReturnValue = A * B;

		StoreFloat(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Multiply_Int(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const int32 A = LoadInt32(Buffers[0], Index);
		const int32 B = LoadInt32(Buffers[1], Index);
		int32 ReturnValue;

		ReturnValue = A * B;

		StoreInt32(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_OneMinus_Double(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const double Value = LoadDouble(Buffers[0], Index);
		double ReturnValue;

		ReturnValue = 1 - Value;

		StoreDouble(Buffers[1], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_OneMinus_Float(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float Value = LoadFloat(Buffers[0], Index);
		float ReturnValue;

		ReturnValue = 1 - Value;

		StoreFloat(Buffers[1], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Sign_Double(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const double Value = LoadDouble(Buffers[0], Index);
		double ReturnValue;

		ReturnValue = Value < 0.d ? -1.d : Value > 0.f ? 1.d : 0.d;

		StoreDouble(Buffers[1], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Sign_Float(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float Value = LoadFloat(Buffers[0], Index);
		float ReturnValue;

		ReturnValue = Value < 0.f ? -1.f : Value > 0.f ? 1.f : 0.f;

		StoreFloat(Buffers[1], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Sign_Int(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const int32 Value = LoadInt32(Buffers[0], Index);
		int32 ReturnValue;

		ReturnValue = Value < 0 ? -1 : Value > 0 ? 1 : 0;

		StoreInt32(Buffers[1], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Subtract_Double(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const double A = LoadDouble(Buffers[0], Index);
		const double B = LoadDouble(Buffers[1], Index);
		double ReturnValue;

		ReturnValue = A - B;

		StoreDouble(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Subtract_Float(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float A = LoadFloat(Buffers[0], Index);
		const float B = LoadFloat(Buffers[1], Index);
		float ReturnValue;

		ReturnValue = A - B;

		StoreFloat(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Subtract_Int(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const int32 A = LoadInt32(Buffers[0], Index);
		const int32 B = LoadInt32(Buffers[1], Index);
		int32 ReturnValue;

		ReturnValue = A - B;

		StoreInt32(Buffers[2], Index, ReturnValue);
	}
}