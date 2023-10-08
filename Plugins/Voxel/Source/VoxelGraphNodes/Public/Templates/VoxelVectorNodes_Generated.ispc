// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelISPCNodeHelpers.isph"

export void VoxelNode_VoxelNode_CrossProduct_2D_Double(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const double2 V1 = MakeDouble2(LoadDouble(Buffers[0], Index), LoadDouble(Buffers[1], Index));
		const double2 V2 = MakeDouble2(LoadDouble(Buffers[2], Index), LoadDouble(Buffers[3], Index));
		double ReturnValue;

		ReturnValue = V1.x * V2.y - V1.y * V2.x;

		StoreDouble(Buffers[4], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_CrossProduct_2D_Float(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float2 V1 = MakeFloat2(LoadFloat(Buffers[0], Index), LoadFloat(Buffers[1], Index));
		const float2 V2 = MakeFloat2(LoadFloat(Buffers[2], Index), LoadFloat(Buffers[3], Index));
		float ReturnValue;

		ReturnValue = V1.x * V2.y - V1.y * V2.x;

		StoreFloat(Buffers[4], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_CrossProduct_3D_Double(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const double3 V1 = MakeDouble3(LoadDouble(Buffers[0], Index), LoadDouble(Buffers[1], Index), LoadDouble(Buffers[2], Index));
		const double3 V2 = MakeDouble3(LoadDouble(Buffers[3], Index), LoadDouble(Buffers[4], Index), LoadDouble(Buffers[5], Index));
		double3 ReturnValue;

		ReturnValue = cross(V1, V2);

		StoreDouble(Buffers[6], Index, ReturnValue.x);
		StoreDouble(Buffers[7], Index, ReturnValue.y);
		StoreDouble(Buffers[8], Index, ReturnValue.z);
	}
}

export void VoxelNode_VoxelNode_CrossProduct_3D_Float(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float3 V1 = MakeFloat3(LoadFloat(Buffers[0], Index), LoadFloat(Buffers[1], Index), LoadFloat(Buffers[2], Index));
		const float3 V2 = MakeFloat3(LoadFloat(Buffers[3], Index), LoadFloat(Buffers[4], Index), LoadFloat(Buffers[5], Index));
		float3 ReturnValue;

		ReturnValue = cross(V1, V2);

		StoreFloat(Buffers[6], Index, ReturnValue.x);
		StoreFloat(Buffers[7], Index, ReturnValue.y);
		StoreFloat(Buffers[8], Index, ReturnValue.z);
	}
}

export void VoxelNode_VoxelNode_Distance2D_3D_Double(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const double3 V1 = MakeDouble3(LoadDouble(Buffers[0], Index), LoadDouble(Buffers[1], Index), LoadDouble(Buffers[2], Index));
		const double3 V2 = MakeDouble3(LoadDouble(Buffers[3], Index), LoadDouble(Buffers[4], Index), LoadDouble(Buffers[5], Index));
		double ReturnValue;

		ReturnValue = distance(MakeFloat2(V1.x, V1.y), MakeFloat2(V2.x, V2.y));

		StoreDouble(Buffers[6], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Distance2D_3D_Float(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float3 V1 = MakeFloat3(LoadFloat(Buffers[0], Index), LoadFloat(Buffers[1], Index), LoadFloat(Buffers[2], Index));
		const float3 V2 = MakeFloat3(LoadFloat(Buffers[3], Index), LoadFloat(Buffers[4], Index), LoadFloat(Buffers[5], Index));
		float ReturnValue;

		ReturnValue = distance(MakeFloat2(V1.x, V1.y), MakeFloat2(V2.x, V2.y));

		StoreFloat(Buffers[6], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Distance_2D_Double(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const double2 V1 = MakeDouble2(LoadDouble(Buffers[0], Index), LoadDouble(Buffers[1], Index));
		const double2 V2 = MakeDouble2(LoadDouble(Buffers[2], Index), LoadDouble(Buffers[3], Index));
		double ReturnValue;

		ReturnValue = distance(V1, V2);

		StoreDouble(Buffers[4], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Distance_2D_Float(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float2 V1 = MakeFloat2(LoadFloat(Buffers[0], Index), LoadFloat(Buffers[1], Index));
		const float2 V2 = MakeFloat2(LoadFloat(Buffers[2], Index), LoadFloat(Buffers[3], Index));
		float ReturnValue;

		ReturnValue = distance(V1, V2);

		StoreFloat(Buffers[4], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Distance_3D_Double(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const double3 V1 = MakeDouble3(LoadDouble(Buffers[0], Index), LoadDouble(Buffers[1], Index), LoadDouble(Buffers[2], Index));
		const double3 V2 = MakeDouble3(LoadDouble(Buffers[3], Index), LoadDouble(Buffers[4], Index), LoadDouble(Buffers[5], Index));
		double ReturnValue;

		ReturnValue = distance(V1, V2);

		StoreDouble(Buffers[6], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Distance_3D_Float(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float3 V1 = MakeFloat3(LoadFloat(Buffers[0], Index), LoadFloat(Buffers[1], Index), LoadFloat(Buffers[2], Index));
		const float3 V2 = MakeFloat3(LoadFloat(Buffers[3], Index), LoadFloat(Buffers[4], Index), LoadFloat(Buffers[5], Index));
		float ReturnValue;

		ReturnValue = distance(V1, V2);

		StoreFloat(Buffers[6], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_DotProduct_2D_Double(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const double2 V1 = MakeDouble2(LoadDouble(Buffers[0], Index), LoadDouble(Buffers[1], Index));
		const double2 V2 = MakeDouble2(LoadDouble(Buffers[2], Index), LoadDouble(Buffers[3], Index));
		double ReturnValue;

		ReturnValue = V1.x * V2.x + V1.y * V2.y;

		StoreDouble(Buffers[4], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_DotProduct_2D_Float(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float2 V1 = MakeFloat2(LoadFloat(Buffers[0], Index), LoadFloat(Buffers[1], Index));
		const float2 V2 = MakeFloat2(LoadFloat(Buffers[2], Index), LoadFloat(Buffers[3], Index));
		float ReturnValue;

		ReturnValue = V1.x * V2.x + V1.y * V2.y;

		StoreFloat(Buffers[4], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_DotProduct_3D_Double(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const double3 V1 = MakeDouble3(LoadDouble(Buffers[0], Index), LoadDouble(Buffers[1], Index), LoadDouble(Buffers[2], Index));
		const double3 V2 = MakeDouble3(LoadDouble(Buffers[3], Index), LoadDouble(Buffers[4], Index), LoadDouble(Buffers[5], Index));
		double ReturnValue;

		ReturnValue = V1.x * V2.x + V1.y * V2.y + V1.z * V2.z;

		StoreDouble(Buffers[6], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_DotProduct_3D_Float(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float3 V1 = MakeFloat3(LoadFloat(Buffers[0], Index), LoadFloat(Buffers[1], Index), LoadFloat(Buffers[2], Index));
		const float3 V2 = MakeFloat3(LoadFloat(Buffers[3], Index), LoadFloat(Buffers[4], Index), LoadFloat(Buffers[5], Index));
		float ReturnValue;

		ReturnValue = V1.x * V2.x + V1.y * V2.y + V1.z * V2.z;

		StoreFloat(Buffers[6], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Length_2D_Double(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const double2 Vector = MakeDouble2(LoadDouble(Buffers[0], Index), LoadDouble(Buffers[1], Index));
		double ReturnValue;

		ReturnValue = length(Vector);

		StoreDouble(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Length_2D_Float(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float2 Vector = MakeFloat2(LoadFloat(Buffers[0], Index), LoadFloat(Buffers[1], Index));
		float ReturnValue;

		ReturnValue = length(Vector);

		StoreFloat(Buffers[2], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Length_3D_Double(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const double3 Vector = MakeDouble3(LoadDouble(Buffers[0], Index), LoadDouble(Buffers[1], Index), LoadDouble(Buffers[2], Index));
		double ReturnValue;

		ReturnValue = length(Vector);

		StoreDouble(Buffers[3], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Length_3D_Float(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float3 Vector = MakeFloat3(LoadFloat(Buffers[0], Index), LoadFloat(Buffers[1], Index), LoadFloat(Buffers[2], Index));
		float ReturnValue;

		ReturnValue = length(Vector);

		StoreFloat(Buffers[3], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_LengthXY_3D_Double(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const double3 Vector = MakeDouble3(LoadDouble(Buffers[0], Index), LoadDouble(Buffers[1], Index), LoadDouble(Buffers[2], Index));
		double ReturnValue;

		ReturnValue = length(MakeFloat2(Vector.x, Vector.y));

		StoreDouble(Buffers[3], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_LengthXY_3D_Float(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float3 Vector = MakeFloat3(LoadFloat(Buffers[0], Index), LoadFloat(Buffers[1], Index), LoadFloat(Buffers[2], Index));
		float ReturnValue;

		ReturnValue = length(MakeFloat2(Vector.x, Vector.y));

		StoreFloat(Buffers[3], Index, ReturnValue);
	}
}

export void VoxelNode_VoxelNode_Normalize_2D_Double(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const double2 Vector = MakeDouble2(LoadDouble(Buffers[0], Index), LoadDouble(Buffers[1], Index));
		double2 ReturnValue;

		ReturnValue = normalize(Vector);

		StoreDouble(Buffers[2], Index, ReturnValue.x);
		StoreDouble(Buffers[3], Index, ReturnValue.y);
	}
}

export void VoxelNode_VoxelNode_Normalize_2D_Float(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float2 Vector = MakeFloat2(LoadFloat(Buffers[0], Index), LoadFloat(Buffers[1], Index));
		float2 ReturnValue;

		ReturnValue = normalize(Vector);

		StoreFloat(Buffers[2], Index, ReturnValue.x);
		StoreFloat(Buffers[3], Index, ReturnValue.y);
	}
}

export void VoxelNode_VoxelNode_Normalize_3D_Double(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const double3 Vector = MakeDouble3(LoadDouble(Buffers[0], Index), LoadDouble(Buffers[1], Index), LoadDouble(Buffers[2], Index));
		double3 ReturnValue;

		ReturnValue = normalize(Vector);

		StoreDouble(Buffers[3], Index, ReturnValue.x);
		StoreDouble(Buffers[4], Index, ReturnValue.y);
		StoreDouble(Buffers[5], Index, ReturnValue.z);
	}
}

export void VoxelNode_VoxelNode_Normalize_3D_Float(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const float3 Vector = MakeFloat3(LoadFloat(Buffers[0], Index), LoadFloat(Buffers[1], Index), LoadFloat(Buffers[2], Index));
		float3 ReturnValue;

		ReturnValue = normalize(Vector);

		StoreFloat(Buffers[3], Index, ReturnValue.x);
		StoreFloat(Buffers[4], Index, ReturnValue.y);
		StoreFloat(Buffers[5], Index, ReturnValue.z);
	}
}