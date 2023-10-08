// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelISPCNodeHelpers.isph"

export void VoxelNode_VoxelNode_Acos(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float Value = LoadFloat(Buffers[0], Index);
		float ReturnValue;

		ReturnValue = acos(Value);

		StoreFloat(Buffers[1], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_AcosDegrees(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float Value = LoadFloat(Buffers[0], Index);
		float ReturnValue;

		ReturnValue = 180.f / PI * acos(Value);

		StoreFloat(Buffers[1], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Asin(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float Value = LoadFloat(Buffers[0], Index);
		float ReturnValue;

		ReturnValue = asin(Value);

		StoreFloat(Buffers[1], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_AsinDegrees(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float Value = LoadFloat(Buffers[0], Index);
		float ReturnValue;

		ReturnValue = 180.f / PI * asin(Value);

		StoreFloat(Buffers[1], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Atan(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float Value = LoadFloat(Buffers[0], Index);
		float ReturnValue;

		ReturnValue = atan(Value);

		StoreFloat(Buffers[1], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Atan2(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float Y = LoadFloat(Buffers[0], Index);
		const float X = LoadFloat(Buffers[1], Index);
		float ReturnValue;

		ReturnValue = atan2(Y, X);

		StoreFloat(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Atan2Degrees(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float Y = LoadFloat(Buffers[0], Index);
		const float X = LoadFloat(Buffers[1], Index);
		float ReturnValue;

		ReturnValue = 180.f / PI * atan2(Y, X);

		StoreFloat(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_AtanDegrees(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float Value = LoadFloat(Buffers[0], Index);
		float ReturnValue;

		ReturnValue = 180.f / PI * atan(Value);

		StoreFloat(Buffers[1], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_BilinearInterpolation(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float2 Alpha = MakeFloat2(LoadFloat(Buffers[0], Index), LoadFloat(Buffers[1], Index));
		const float X0Y0 = LoadFloat(Buffers[2], Index);
		const float X1Y0 = LoadFloat(Buffers[3], Index);
		const float X0Y1 = LoadFloat(Buffers[4], Index);
		const float X1Y1 = LoadFloat(Buffers[5], Index);
		float ReturnValue;

		ReturnValue = BilinearInterpolation(X0Y0, X1Y0, X0Y1, X1Y1, Alpha.x, Alpha.y);

		StoreFloat(Buffers[6], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Bitwise_And(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const int32 A = LoadInt32(Buffers[0], Index);
		const int32 B = LoadInt32(Buffers[1], Index);
		int32 ReturnValue;

		ReturnValue = A & B;

		StoreInt32(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Bitwise_Not(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const int32 A = LoadInt32(Buffers[0], Index);
		int32 ReturnValue;

		ReturnValue = ~A;

		StoreInt32(Buffers[1], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Bitwise_Or(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const int32 A = LoadInt32(Buffers[0], Index);
		const int32 B = LoadInt32(Buffers[1], Index);
		int32 ReturnValue;

		ReturnValue = A | B;

		StoreInt32(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Bitwise_Xor(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const int32 A = LoadInt32(Buffers[0], Index);
		const int32 B = LoadInt32(Buffers[1], Index);
		int32 ReturnValue;

		ReturnValue = A ^ B;

		StoreInt32(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_BooleanNOR(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const bool A = LoadBool(Buffers[0], Index);
		const bool B = LoadBool(Buffers[1], Index);
		bool ReturnValue;

		ReturnValue = !(A || B);

		StoreBool(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_BooleanNOT(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const bool A = LoadBool(Buffers[0], Index);
		bool ReturnValue;

		ReturnValue = !A;

		StoreBool(Buffers[1], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_BooleanXOR(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
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

export void VoxelNode_VoxelNode_BreakQuaternion(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float4 Quat = MakeFloat4(LoadFloat(Buffers[0], Index), LoadFloat(Buffers[1], Index), LoadFloat(Buffers[2], Index), LoadFloat(Buffers[3], Index));
		float Roll;
		float Pitch;
		float Yaw;

		MakeEulerFromQuaternion(Quat, Pitch, Yaw, Roll);

		StoreFloat(Buffers[4], Index, Roll);
		StoreFloat(Buffers[5], Index, Pitch);
		StoreFloat(Buffers[6], Index, Yaw);
	}
}

export void VoxelNode_VoxelNode_Cos(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float Value = LoadFloat(Buffers[0], Index);
		float ReturnValue;

		ReturnValue = cos(Value);

		StoreFloat(Buffers[1], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_CosDegrees(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float Value = LoadFloat(Buffers[0], Index);
		float ReturnValue;

		ReturnValue = cos(PI / 180.f * Value);

		StoreFloat(Buffers[1], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_DegreesToRadians(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float A = LoadFloat(Buffers[0], Index);
		float ReturnValue;

		ReturnValue = A * (PI / 180.f);

		StoreFloat(Buffers[1], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_DistanceToColor(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float Distance = LoadFloat(Buffers[0], Index);
		const float Scale = LoadFloat(Buffers[1], Index);
		float4 Color;

		Color = GetDistanceFieldColor(Distance / Scale);

		StoreFloat(Buffers[2], Index, Color.x);
		StoreFloat(Buffers[3], Index, Color.y);
		StoreFloat(Buffers[4], Index, Color.z);
		StoreFloat(Buffers[5], Index, Color.w);
	}
}

export void VoxelNode_VoxelNode_GetPI(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		float ReturnValue;

		ReturnValue = PI;

		StoreFloat(Buffers[0], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_LeftShift(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const int32 Value = LoadInt32(Buffers[0], Index);
		const int32 Shift = LoadInt32(Buffers[1], Index);
		int32 ReturnValue;

		IGNORE_PERF_WARNING
		ReturnValue = Value << clamp(Shift, 0, 31);

		StoreInt32(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_MakeQuaternion(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float Roll = LoadFloat(Buffers[0], Index);
		const float Pitch = LoadFloat(Buffers[1], Index);
		const float Yaw = LoadFloat(Buffers[2], Index);
		float4 ReturnValue;

		ReturnValue = MakeQuaternionFromEuler(Pitch, Yaw, Roll);

		StoreFloat(Buffers[3], Index, ReturnValue.x);
		StoreFloat(Buffers[4], Index, ReturnValue.y);
		StoreFloat(Buffers[5], Index, ReturnValue.z);
		StoreFloat(Buffers[6], Index, ReturnValue.w);
	}
}

export void VoxelNode_VoxelNode_MakeRotationFromZ(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float3 Z = MakeFloat3(LoadFloat(Buffers[0], Index), LoadFloat(Buffers[1], Index), LoadFloat(Buffers[2], Index));
		float4 ReturnValue;

		ReturnValue = MakeQuaternionFromZ(Z);

		StoreFloat(Buffers[3], Index, ReturnValue.x);
		StoreFloat(Buffers[4], Index, ReturnValue.y);
		StoreFloat(Buffers[5], Index, ReturnValue.z);
		StoreFloat(Buffers[6], Index, ReturnValue.w);
	}
}

export void VoxelNode_VoxelNode_RadiansToDegrees(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float A = LoadFloat(Buffers[0], Index);
		float ReturnValue;

		ReturnValue = A * (180.f / PI);

		StoreFloat(Buffers[1], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_RightShift(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const int32 Value = LoadInt32(Buffers[0], Index);
		const int32 Shift = LoadInt32(Buffers[1], Index);
		int32 ReturnValue;

		IGNORE_PERF_WARNING
		ReturnValue = Value >> clamp(Shift, 0, 31);

		StoreInt32(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Sin(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float Value = LoadFloat(Buffers[0], Index);
		float ReturnValue;

		ReturnValue = sin(Value);

		StoreFloat(Buffers[1], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_SinDegrees(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float Value = LoadFloat(Buffers[0], Index);
		float ReturnValue;

		ReturnValue = sin(PI / 180.f * Value);

		StoreFloat(Buffers[1], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Tan(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float Value = LoadFloat(Buffers[0], Index);
		float ReturnValue;

		ReturnValue = tan(Value);

		StoreFloat(Buffers[1], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_TanDegrees(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float Value = LoadFloat(Buffers[0], Index);
		float ReturnValue;

		ReturnValue = tan(PI / 180.f * Value);

		StoreFloat(Buffers[1], Index, ReturnValue);
	}
}