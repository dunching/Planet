// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelMinimal.isph"

export uniform bool BaseUtilities_AllEqual_uint8(
	const uniform uint8 Constant,
	const uniform uint8 Data[],
	const uniform int32 Num)
{
	const uniform int32 AlignedNum = programCount * (Num / programCount);

	for (uniform int32 Index = 0; Index < AlignedNum; Index += programCount)
	{
		if (any(Data[Index + programIndex] != Constant))
		{
			return false;
		}
	}

	for (uniform int32 Index = AlignedNum; Index < Num; Index++)
	{
		if (Data[Index] != Constant)
		{
			return false;
		}
	}

	return true;
}

export uniform bool BaseUtilities_AllEqual_uint16(
	const uniform uint16 Constant,
	const uniform uint16 Data[],
	const uniform int32 Num)
{
	const uniform int32 AlignedNum = programCount * (Num / programCount);

	for (uniform int32 Index = 0; Index < AlignedNum; Index += programCount)
	{
		if (any(Data[Index + programIndex] != Constant))
		{
			return false;
		}
	}

	for (uniform int32 Index = AlignedNum; Index < Num; Index++)
	{
		if (Data[Index] != Constant)
		{
			return false;
		}
	}

	return true;
}

export uniform bool BaseUtilities_AllEqual_uint32(
	const uniform uint32 Constant,
	const uniform uint32 Data[],
	const uniform int32 Num)
{
	const uniform int32 AlignedNum = programCount * (Num / programCount);

	for (uniform int32 Index = 0; Index < AlignedNum; Index += programCount)
	{
		if (any(Data[Index + programIndex] != Constant))
		{
			return false;
		}
	}

	for (uniform int32 Index = AlignedNum; Index < Num; Index++)
	{
		if (Data[Index] != Constant)
		{
			return false;
		}
	}

	return true;
}

export uniform bool BaseUtilities_AllEqual_uint64(
	const uniform uint64 Constant,
	const uniform uint64 Data[],
	const uniform int32 Num)
{
	const uniform int32 AlignedNum = programCount * (Num / programCount);

	for (uniform int32 Index = 0; Index < AlignedNum; Index += programCount)
	{
		if (any(Data[Index + programIndex] != Constant))
		{
			return false;
		}
	}

	for (uniform int32 Index = AlignedNum; Index < Num; Index++)
	{
		if (Data[Index] != Constant)
		{
			return false;
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

export uniform float BaseUtilities_GetMin(
	const uniform float Values[],
	const uniform int32 Num)
{
	const uniform int32 AlignedNum = programCount * (Num / programCount);

	uniform float Result;
	if (AlignedNum != 0)
	{
		varying float Value = Values[0];
		for (uniform int32 Index = 0; Index < AlignedNum; Index += programCount)
		{
			Value = min(Value, Values[Index + programIndex]);
		}
		Result = reduce_min(Value);
	}
	else
	{
		Result = Values[0];
	}

	for (uniform int32 Index = AlignedNum; Index < Num; Index++)
	{
		Result = min(Result, Values[Index]);
	}
	return Result;
}

export uniform float BaseUtilities_GetMax(
	const uniform float Values[],
	const uniform int32 Num)
{
	const uniform int32 AlignedNum = programCount * (Num / programCount);

	uniform float Result;
	if (AlignedNum != 0)
	{
		varying float Value = Values[0];
		for (uniform int32 Index = 0; Index < AlignedNum; Index += programCount)
		{
			Value = max(Value, Values[Index + programIndex]);
		}
		Result = reduce_max(Value);
	}
	else
	{
		Result = Values[0];
	}

	for (uniform int32 Index = AlignedNum; Index < Num; Index++)
	{
		Result = max(Result, Values[Index]);
	}
	return Result;
}

export void BaseUtilities_GetMinMax(
	const uniform float Values[],
	const uniform int32 Num,
	float* uniform OutMin,
	float* uniform OutMax)
{
	const uniform int32 AlignedNum = programCount * (Num / programCount);

	uniform float MinResult;
	uniform float MaxResult;
	if (AlignedNum != 0)
	{
		varying float MinValue = Values[0];
		varying float MaxValue = Values[0];
		for (uniform int32 Index = 0; Index < AlignedNum; Index += programCount)
		{
			const varying float Value = Values[Index + programIndex];
			MinValue = min(MinValue, Value);
			MaxValue = max(MaxValue, Value);
		}
		MinResult = reduce_min(MinValue);
		MaxResult = reduce_max(MaxValue);
	}
	else
	{
		MinResult = Values[0];
		MaxResult = Values[0];
	}

	for (uniform int32 Index = AlignedNum; Index < Num; Index++)
	{
		const uniform float Value = Values[Index];
		MinResult = min(MinResult, Value);
		MaxResult = max(MaxResult, Value);
	}

	*OutMin = MinResult;
	*OutMax = MaxResult;
}

export void BaseUtilities_GetMinMaxSafe(
	const uniform float Values[],
	const uniform int32 Num,
	float* uniform OutMin,
	float* uniform OutMax)
{
	const uniform int32 AlignedNum = programCount * (Num / programCount);

	uniform float MinResult;
	uniform float MaxResult;
	if (AlignedNum != 0)
	{
		varying float MinValue = MAX_flt;
		varying float MaxValue = -MAX_flt;
		for (uniform int32 Index = 0; Index < AlignedNum; Index += programCount)
		{
			const varying float Value = Values[Index + programIndex];
			if (!IsFinite(Value))
			{
				continue;
			}

			MinValue = min(MinValue, Value);
			MaxValue = max(MaxValue, Value);
		}
		MinResult = reduce_min(MinValue);
		MaxResult = reduce_max(MaxValue);
	}
	else
	{
		MinResult = MAX_flt;
		MaxResult = -MAX_flt;
	}

	for (uniform int32 Index = AlignedNum; Index < Num; Index++)
	{
		const uniform float Value = Values[Index];
		if (!IsFinite(Value))
		{
			continue;
		}

		MinResult = min(MinResult, Value);
		MaxResult = max(MaxResult, Value);
	}

	*OutMin = MinResult;
	*OutMax = MaxResult;
}