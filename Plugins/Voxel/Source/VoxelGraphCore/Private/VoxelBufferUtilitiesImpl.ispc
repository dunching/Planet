// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelMinimal.isph"

// ReSharper disable CppCStyleCast

export void VoxelBufferUtilities_ApplyTransform(
	const uniform float PositionX[], const uniform bool bConstPositionX,
	const uniform float PositionY[], const uniform bool bConstPositionY,
	const uniform float PositionZ[], const uniform bool bConstPositionZ,
	const uniform int32 Num,
	const uniform float3& Translation,
	const uniform float4& Rotation,
	const uniform float3& Scale,
	uniform float OutPositionsX[],
	uniform float OutPositionsY[],
	uniform float OutPositionsZ[])
{
	if (IsIdentity(Rotation))
	{
		FOREACH(Index, 0, Num)
		{
			const varying float3 OldPosition = MakeFloat3(
				bConstPositionX ? PositionX[0] : PositionX[Index],
				bConstPositionY ? PositionY[0] : PositionY[Index],
				bConstPositionZ ? PositionZ[0] : PositionZ[Index]);

			const varying float3 NewPosition = OldPosition * Scale + Translation;

			OutPositionsX[Index] = NewPosition.x;
			OutPositionsY[Index] = NewPosition.y;
			OutPositionsZ[Index] = NewPosition.z;
		}
	}
	else
	{
		FOREACH(Index, 0, Num)
		{
			const varying float3 OldPosition = MakeFloat3(
				bConstPositionX ? PositionX[0] : PositionX[Index],
				bConstPositionY ? PositionY[0] : PositionY[Index],
				bConstPositionZ ? PositionZ[0] : PositionZ[Index]);

			const varying float3 NewPosition = RotateVector(OldPosition * Scale, Rotation) + Translation;

			OutPositionsX[Index] = NewPosition.x;
			OutPositionsY[Index] = NewPosition.y;
			OutPositionsZ[Index] = NewPosition.z;
		}
	}
}

export void VoxelBufferUtilities_ApplyInverseTransform(
	const uniform float PositionX[], const uniform bool bConstPositionX,
	const uniform float PositionY[], const uniform bool bConstPositionY,
	const uniform float PositionZ[], const uniform bool bConstPositionZ,
	const uniform int32 Num,
	const uniform float3& Translation,
	const uniform float4& Rotation,
	const uniform float3& Scale,
	uniform float OutPositionsX[],
	uniform float OutPositionsY[],
	uniform float OutPositionsZ[])
{
	uniform float3 InvScale;
	InvScale.x = Scale.x != 0 ? 1.f / Scale.x : 1.f;
	InvScale.y = Scale.y != 0 ? 1.f / Scale.y : 1.f;
	InvScale.z = Scale.z != 0 ? 1.f / Scale.z : 1.f;

	if (IsIdentity(Rotation))
	{
		FOREACH(Index, 0, Num)
		{
			const varying float3 OldPosition = MakeFloat3(
				bConstPositionX ? PositionX[0] : PositionX[Index],
				bConstPositionY ? PositionY[0] : PositionY[Index],
				bConstPositionZ ? PositionZ[0] : PositionZ[Index]);

			const varying float3 NewPosition = (OldPosition - Translation) * InvScale;

			OutPositionsX[Index] = NewPosition.x;
			OutPositionsY[Index] = NewPosition.y;
			OutPositionsZ[Index] = NewPosition.z;
		}
	}
	else
	{
		FOREACH(Index, 0, Num)
		{
			const varying float3 OldPosition = MakeFloat3(
				bConstPositionX ? PositionX[0] : PositionX[Index],
				bConstPositionY ? PositionY[0] : PositionY[Index],
				bConstPositionZ ? PositionZ[0] : PositionZ[Index]);

			const varying float3 NewPosition = UnrotateVector(OldPosition - Translation, Rotation) * InvScale;

			OutPositionsX[Index] = NewPosition.x;
			OutPositionsY[Index] = NewPosition.y;
			OutPositionsZ[Index] = NewPosition.z;
		}
	}
}

export void VoxelBufferUtilities_ApplyTransform_Bulk(
	const uniform float PositionX[], const uniform bool bConstPositionX,
	const uniform float PositionY[], const uniform bool bConstPositionY,
	const uniform float PositionZ[], const uniform bool bConstPositionZ,
	const uniform float TranslationX[], const uniform bool bConstTranslationX,
	const uniform float TranslationY[], const uniform bool bConstTranslationY,
	const uniform float TranslationZ[], const uniform bool bConstTranslationZ,
	const uniform float RotationX[], const uniform bool bConstRotationX,
	const uniform float RotationY[], const uniform bool bConstRotationY,
	const uniform float RotationZ[], const uniform bool bConstRotationZ,
	const uniform float RotationW[], const uniform bool bConstRotationW,
	const uniform float ScaleX[], const uniform bool bConstScaleX,
	const uniform float ScaleY[], const uniform bool bConstScaleY,
	const uniform float ScaleZ[], const uniform bool bConstScaleZ,
	const uniform int32 Num,
	uniform float OutPositionsX[],
	uniform float OutPositionsY[],
	uniform float OutPositionsZ[])
{
	FOREACH(Index, 0, Num)
	{
		const varying float3 OldPosition = MakeFloat3(
			bConstPositionX ? PositionX[0] : PositionX[Index],
			bConstPositionY ? PositionY[0] : PositionY[Index],
			bConstPositionZ ? PositionZ[0] : PositionZ[Index]);

		const varying float3 Translation = MakeFloat3(
			bConstTranslationX ? TranslationX[0] : TranslationX[Index],
			bConstTranslationY ? TranslationY[0] : TranslationY[Index],
			bConstTranslationZ ? TranslationZ[0] : TranslationZ[Index]);

		const varying float4 Rotation = MakeFloat4(
			bConstRotationX ? RotationX[0] : RotationX[Index],
			bConstRotationY ? RotationY[0] : RotationY[Index],
			bConstRotationZ ? RotationZ[0] : RotationZ[Index],
			bConstRotationW ? RotationW[0] : RotationW[Index]);

		const varying float3 Scale = MakeFloat3(
			bConstScaleX ? ScaleX[0] : ScaleX[Index],
			bConstScaleY ? ScaleY[0] : ScaleY[Index],
			bConstScaleZ ? ScaleZ[0] : ScaleZ[Index]);

		const varying float3 NewPosition = RotateVector(OldPosition * Scale, Rotation) + Translation;

		OutPositionsX[Index] = NewPosition.x;
		OutPositionsY[Index] = NewPosition.y;
		OutPositionsZ[Index] = NewPosition.z;
	}
}

export void VoxelBufferUtilities_ApplyTransform_Matrix(
	const uniform float PositionX[], const uniform bool bConstPositionX,
	const uniform float PositionY[], const uniform bool bConstPositionY,
	const uniform float PositionZ[], const uniform bool bConstPositionZ,
	const uniform int32 Num,
	const uniform float3& Translation,
	const uniform float3& MatrixX,
	const uniform float3& MatrixY,
	const uniform float3& MatrixZ,
	uniform float OutPositionsX[],
	uniform float OutPositionsY[],
	uniform float OutPositionsZ[])
{
	FOREACH(Index, 0, Num)
	{
		const varying float3 OldPosition = MakeFloat3(
			bConstPositionX ? PositionX[0] : PositionX[Index],
			bConstPositionY ? PositionY[0] : PositionY[Index],
			bConstPositionZ ? PositionZ[0] : PositionZ[Index]);

		const varying float3 NewPosition =
			Translation +
			MatrixX * OldPosition.x +
			MatrixY * OldPosition.y +
			MatrixZ * OldPosition.z;

		OutPositionsX[Index] = NewPosition.x;
		OutPositionsY[Index] = NewPosition.y;
		OutPositionsZ[Index] = NewPosition.z;
	}
}

export void VoxelBufferUtilities_TransformDistance(
	const uniform float Distance[],
	uniform float OutDistance[],
	const uniform float Scale,
	const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		OutDistance[Index] = Distance[Index] * Scale;
	}
}

export void VoxelBufferUtilities_Min8(
	const uniform float Buffer0[], const uniform bool bIsConstBuffer0,
	const uniform float Buffer1[], const uniform bool bIsConstBuffer1,
	const uniform float Buffer2[], const uniform bool bIsConstBuffer2,
	const uniform float Buffer3[], const uniform bool bIsConstBuffer3,
	const uniform float Buffer4[], const uniform bool bIsConstBuffer4,
	const uniform float Buffer5[], const uniform bool bIsConstBuffer5,
	const uniform float Buffer6[], const uniform bool bIsConstBuffer6,
	const uniform float Buffer7[], const uniform bool bIsConstBuffer7,
	uniform float Result[],
	const uniform int32 Num)
{
	if (!bIsConstBuffer0 &&
		!bIsConstBuffer1 &&
		!bIsConstBuffer2 &&
		!bIsConstBuffer3 &&
		!bIsConstBuffer4 &&
		!bIsConstBuffer5 &&
		!bIsConstBuffer6 &&
		!bIsConstBuffer7)
	{
		FOREACH(Index, 0, Num)
		{
			Result[Index] =
				min(Buffer0[Index],
				min(Buffer1[Index],
				min(Buffer2[Index],
				min(Buffer3[Index],
				min(Buffer4[Index],
				min(Buffer5[Index],
				min(Buffer6[Index], Buffer7[Index])))))));
		}
		return;
	}

	FOREACH(Index, 0, Num)
	{
		Result[Index] =
			min(bIsConstBuffer0 ? Buffer0[0] : Buffer0[Index],
			min(bIsConstBuffer1 ? Buffer1[0] : Buffer1[Index],
			min(bIsConstBuffer2 ? Buffer2[0] : Buffer2[Index],
			min(bIsConstBuffer3 ? Buffer3[0] : Buffer3[Index],
			min(bIsConstBuffer4 ? Buffer4[0] : Buffer4[Index],
			min(bIsConstBuffer5 ? Buffer5[0] : Buffer5[Index],
			min(bIsConstBuffer6 ? Buffer6[0] : Buffer6[Index], bIsConstBuffer7 ? Buffer7[0] : Buffer7[Index])))))));
	}
}

export void VoxelBufferUtilities_Max8(
	const uniform float Buffer0[], const uniform bool bIsConstBuffer0,
	const uniform float Buffer1[], const uniform bool bIsConstBuffer1,
	const uniform float Buffer2[], const uniform bool bIsConstBuffer2,
	const uniform float Buffer3[], const uniform bool bIsConstBuffer3,
	const uniform float Buffer4[], const uniform bool bIsConstBuffer4,
	const uniform float Buffer5[], const uniform bool bIsConstBuffer5,
	const uniform float Buffer6[], const uniform bool bIsConstBuffer6,
	const uniform float Buffer7[], const uniform bool bIsConstBuffer7,
	uniform float Result[],
	const uniform int32 Num)
{
	if (!bIsConstBuffer0 &&
		!bIsConstBuffer1 &&
		!bIsConstBuffer2 &&
		!bIsConstBuffer3 &&
		!bIsConstBuffer4 &&
		!bIsConstBuffer5 &&
		!bIsConstBuffer6 &&
		!bIsConstBuffer7)
	{
		FOREACH(Index, 0, Num)
		{
			Result[Index] =
				max(Buffer0[Index],
				max(Buffer1[Index],
				max(Buffer2[Index],
				max(Buffer3[Index],
				max(Buffer4[Index],
				max(Buffer5[Index],
				max(Buffer6[Index], Buffer7[Index])))))));
		}
		return;
	}

	FOREACH(Index, 0, Num)
	{
		Result[Index] =
			max(bIsConstBuffer0 ? Buffer0[0] : Buffer0[Index],
			max(bIsConstBuffer1 ? Buffer1[0] : Buffer1[Index],
			max(bIsConstBuffer2 ? Buffer2[0] : Buffer2[Index],
			max(bIsConstBuffer3 ? Buffer3[0] : Buffer3[Index],
			max(bIsConstBuffer4 ? Buffer4[0] : Buffer4[Index],
			max(bIsConstBuffer5 ? Buffer5[0] : Buffer5[Index],
			max(bIsConstBuffer6 ? Buffer6[0] : Buffer6[Index], bIsConstBuffer7 ? Buffer7[0] : Buffer7[Index])))))));
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

export void VoxelBufferUtilities_IntToFloat(
	const uniform int32 Int[],
	uniform float Float[],
	const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		Float[Index] = (float)Int[Index];
	}
}

export void VoxelBufferUtilities_IntToDouble(
	const uniform int32 Int[],
	uniform double Double[],
	const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		Double[Index] = (double)Int[Index];
	}
}

export void VoxelBufferUtilities_FloatToDouble(
	const uniform float Float[],
	uniform double Double[],
	const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		Double[Index] = (double)Float[Index];
	}
}

export void VoxelBufferUtilities_DoubleToFloat(
	const uniform double Double[],
	uniform float Float[],
	const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		Float[Index] = (float)Double[Index];
	}
}

export void VoxelBufferUtilities_PointIdToSeed(
	const uniform uint64 PointId[],
	uniform uint32 Seed[],
	const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const uint64 Value = PointId[Index];
		Seed[Index] = (uint32)Value ^ (uint32)(Value >> 32);
	}
}

export void VoxelBufferUtilities_Add(
	const uniform float A[],
	const uniform bool bConstantA,
	const uniform float B[],
	const uniform bool bConstantB,
	uniform float Result[],
	const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		Result[Index] = (bConstantA ? A[0] : A[Index]) + (bConstantB ? B[0] : B[Index]);
	}
}

export void VoxelBufferUtilities_Multiply(
	const uniform float A[],
	const uniform bool bConstantA,
	const uniform float B[],
	const uniform bool bConstantB,
	uniform float Result[],
	const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		Result[Index] = (bConstantA ? A[0] : A[Index]) * (bConstantB ? B[0] : B[Index]);
	}
}

export void VoxelBufferUtilities_Less(
	const uniform float A[],
	const uniform bool bConstantA,
	const uniform float B[],
	const uniform bool bConstantB,
	uniform uint8 Result[],
	const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		Result[Index] = (bConstantA ? A[0] : A[Index]) < (bConstantB ? B[0] : B[Index]) ? 1 : 0;
	}
}

export void VoxelBufferUtilities_Alpha(
	const uniform float A[],
	const uniform bool bConstantA,
	const uniform float B[],
	const uniform bool bConstantB,
	const uniform float Alpha[],
	const uniform bool bConstantAlpha,
	uniform float Result[],
	const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		Result[Index] = lerp(
			bConstantA ? A[0] : A[Index],
			bConstantB ? B[0] : B[Index],
			bConstantAlpha ? Alpha[0] : Alpha[Index]);
	}
}

export void VoxelBufferUtilities_Combine(
	const uniform float AX[],
	const uniform bool bConstantAX,
	const uniform float AY[],
	const uniform bool bConstantAY,
	const uniform float AZ[],
	const uniform bool bConstantAZ,
	const uniform float AW[],
	const uniform bool bConstantAW,
	const uniform float BX[],
	const uniform bool bConstantBX,
	const uniform float BY[],
	const uniform bool bConstantBY,
	const uniform float BZ[],
	const uniform bool bConstantBZ,
	const uniform float BW[],
	const uniform bool bConstantBW,
	uniform float X[],
	uniform float Y[],
	uniform float Z[],
	uniform float W[],
	const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const varying float A0 = bConstantAX ? AX[0] : AX[Index];
		const varying float A1 = bConstantAY ? AY[0] : AY[Index];
		const varying float A2 = bConstantAZ ? AZ[0] : AZ[Index];
		const varying float A3 = bConstantAW ? AW[0] : AW[Index];

		const varying float B0 = bConstantBX ? BX[0] : BX[Index];
		const varying float B1 = bConstantBY ? BY[0] : BY[Index];
		const varying float B2 = bConstantBZ ? BZ[0] : BZ[Index];
		const varying float B3 = bConstantBW ? BW[0] : BW[Index];

		X[Index] = A3 * B0 + A0 * B3 + A1 * B2 - A2 * B1;
		Y[Index] = A3 * B1 - A0 * B2 + A1 * B3 + A2 * B0;
		Z[Index] = A3 * B2 + A0 * B1 - A1 * B0 + A2 * B3;
		W[Index] = A3 * B3 - A0 * B0 - A1 * B1 - A2 * B2;
	}
}