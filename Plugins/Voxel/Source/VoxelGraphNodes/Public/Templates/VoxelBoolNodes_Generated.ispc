// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelISPCNodeHelpers.isph"

export void VoxelNode_VoxelNode_BooleanAND(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const bool A = LoadBool(Buffers[0], Index);
		const bool B = LoadBool(Buffers[1], Index);
		bool ReturnValue;

		ReturnValue = A && B;

		StoreBool(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_BooleanNAND(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const bool A = LoadBool(Buffers[0], Index);
		const bool B = LoadBool(Buffers[1], Index);
		bool ReturnValue;

		ReturnValue = !(A && B);

		StoreBool(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_BooleanOR(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const bool A = LoadBool(Buffers[0], Index);
		const bool B = LoadBool(Buffers[1], Index);
		bool ReturnValue;

		ReturnValue = A || B;

		StoreBool(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Equal_Bool(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const bool A = LoadBool(Buffers[0], Index);
		const bool B = LoadBool(Buffers[1], Index);
		bool ReturnValue;

		ReturnValue = A == B;

		StoreBool(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Equal_Byte(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const uint8 A = LoadByte(Buffers[0], Index);
		const uint8 B = LoadByte(Buffers[1], Index);
		bool ReturnValue;

		ReturnValue = A == B;

		StoreBool(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Equal_Double(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const double A = LoadDouble(Buffers[0], Index);
		const double B = LoadDouble(Buffers[1], Index);
		bool ReturnValue;

		ReturnValue = A == B;

		StoreBool(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Equal_Float(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float A = LoadFloat(Buffers[0], Index);
		const float B = LoadFloat(Buffers[1], Index);
		bool ReturnValue;

		ReturnValue = A == B;

		StoreBool(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Equal_Int(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const int32 A = LoadInt32(Buffers[0], Index);
		const int32 B = LoadInt32(Buffers[1], Index);
		bool ReturnValue;

		ReturnValue = A == B;

		StoreBool(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Greater_Double(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const double A = LoadDouble(Buffers[0], Index);
		const double B = LoadDouble(Buffers[1], Index);
		bool ReturnValue;

		ReturnValue = A > B;

		StoreBool(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Greater_Float(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float A = LoadFloat(Buffers[0], Index);
		const float B = LoadFloat(Buffers[1], Index);
		bool ReturnValue;

		ReturnValue = A > B;

		StoreBool(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Greater_Int(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const int32 A = LoadInt32(Buffers[0], Index);
		const int32 B = LoadInt32(Buffers[1], Index);
		bool ReturnValue;

		ReturnValue = A > B;

		StoreBool(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_GreaterEqual_Double(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const double A = LoadDouble(Buffers[0], Index);
		const double B = LoadDouble(Buffers[1], Index);
		bool ReturnValue;

		ReturnValue = A >= B;

		StoreBool(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_GreaterEqual_Float(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float A = LoadFloat(Buffers[0], Index);
		const float B = LoadFloat(Buffers[1], Index);
		bool ReturnValue;

		ReturnValue = A >= B;

		StoreBool(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_GreaterEqual_Int(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const int32 A = LoadInt32(Buffers[0], Index);
		const int32 B = LoadInt32(Buffers[1], Index);
		bool ReturnValue;

		ReturnValue = A >= B;

		StoreBool(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Less_Double(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const double A = LoadDouble(Buffers[0], Index);
		const double B = LoadDouble(Buffers[1], Index);
		bool ReturnValue;

		ReturnValue = A < B;

		StoreBool(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Less_Float(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float A = LoadFloat(Buffers[0], Index);
		const float B = LoadFloat(Buffers[1], Index);
		bool ReturnValue;

		ReturnValue = A < B;

		StoreBool(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Less_Int(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const int32 A = LoadInt32(Buffers[0], Index);
		const int32 B = LoadInt32(Buffers[1], Index);
		bool ReturnValue;

		ReturnValue = A < B;

		StoreBool(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_LessEqual_Double(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const double A = LoadDouble(Buffers[0], Index);
		const double B = LoadDouble(Buffers[1], Index);
		bool ReturnValue;

		ReturnValue = A <= B;

		StoreBool(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_LessEqual_Float(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float A = LoadFloat(Buffers[0], Index);
		const float B = LoadFloat(Buffers[1], Index);
		bool ReturnValue;

		ReturnValue = A <= B;

		StoreBool(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_LessEqual_Int(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const int32 A = LoadInt32(Buffers[0], Index);
		const int32 B = LoadInt32(Buffers[1], Index);
		bool ReturnValue;

		ReturnValue = A <= B;

		StoreBool(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_NearlyEqual_Double(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const double A = LoadDouble(Buffers[0], Index);
		const double B = LoadDouble(Buffers[1], Index);
		const double ErrorTolerance = LoadDouble(Buffers[2], Index);
		bool ReturnValue;

		ReturnValue = abs(A - B) <= ErrorTolerance;

		StoreBool(Buffers[3], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_NearlyEqual_Float(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float A = LoadFloat(Buffers[0], Index);
		const float B = LoadFloat(Buffers[1], Index);
		const float ErrorTolerance = LoadFloat(Buffers[2], Index);
		bool ReturnValue;

		ReturnValue = abs(A - B) <= ErrorTolerance;

		StoreBool(Buffers[3], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_NotEqual_Bool(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const bool A = LoadBool(Buffers[0], Index);
		const bool B = LoadBool(Buffers[1], Index);
		bool ReturnValue;

		ReturnValue = A != B;

		StoreBool(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_NotEqual_Byte(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const uint8 A = LoadByte(Buffers[0], Index);
		const uint8 B = LoadByte(Buffers[1], Index);
		bool ReturnValue;

		ReturnValue = A != B;

		StoreBool(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_NotEqual_Double(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const double A = LoadDouble(Buffers[0], Index);
		const double B = LoadDouble(Buffers[1], Index);
		bool ReturnValue;

		ReturnValue = A != B;

		StoreBool(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_NotEqual_Float(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float A = LoadFloat(Buffers[0], Index);
		const float B = LoadFloat(Buffers[1], Index);
		bool ReturnValue;

		ReturnValue = A != B;

		StoreBool(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_NotEqual_Int(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const int32 A = LoadInt32(Buffers[0], Index);
		const int32 B = LoadInt32(Buffers[1], Index);
		bool ReturnValue;

		ReturnValue = A != B;

		StoreBool(Buffers[2], Index, ReturnValue);
	}
}