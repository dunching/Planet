// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelMinimal.isph"

// ReSharper disable CppVariableCanBeMadeConstexpr

enum ERichCurveInterpMode
{
	RCIM_Constant,
	RCIM_Linear,
	RCIM_Cubic
};

struct FRichCurveKey
{
	float Time;
	float Value;
	float ArriveTangent;
	float LeaveTangent;
	ERichCurveInterpMode InterpMode;
};

FORCEINLINE varying float BezierInterp(
	const varying float P0,
	const varying float P1,
	const varying float P2,
	const varying float P3,
	const varying float Alpha)
{
	const varying float P01 = lerp(P0, P1, Alpha);
	const varying float P12 = lerp(P1, P2, Alpha);
	const varying float P23 = lerp(P2, P3, Alpha);
	const varying float P012 = lerp(P01, P12, Alpha);
	const varying float P123 = lerp(P12, P23, Alpha);
	const varying float P0123 = lerp(P012, P123, Alpha);
	return P0123;
}

FORCEINLINE float Eval(
	const uniform FRichCurveKey* uniform Keys,
	const uniform int32 NumKeys,
	const varying float Time)
{
	VOXEL_ASSUME(NumKeys >= 2);

	varying ERichCurveInterpMode InterpMode;

	varying float Key1Time;
	varying float Key2Time;
	varying float Key1Value;
	varying float Key2Value;
	varying float Key1LeaveTangent;
	varying float Key2ArriveTangent;

	// Always initialize to avoid undeterministic behavior
	{
		const FRichCurveKey uniform& Key1 = Keys[0];
		const FRichCurveKey uniform& Key2 = Keys[1];

		InterpMode = Key1.InterpMode;

		Key1Time = Key1.Time;
		Key2Time = Key2.Time;
		Key1Value = Key1.Value;
		Key2Value = Key2.Value;
		Key1LeaveTangent = Key1.LeaveTangent;
		Key2ArriveTangent = Key2.ArriveTangent;
	}

	for (uniform int32 Index = 1; Index < NumKeys - 1; Index++)
	{
		const FRichCurveKey uniform& Key1 = Keys[Index];
		const FRichCurveKey uniform& Key2 = Keys[Index + 1];

		if (Time < Key1.Time ||
			Time > Key2.Time)
		{
			continue;
		}

		InterpMode = Key1.InterpMode;

		Key1Time = Key1.Time;
		Key2Time = Key2.Time;
		Key1Value = Key1.Value;
		Key2Value = Key2.Value;
		Key1LeaveTangent = Key1.LeaveTangent;
		Key2ArriveTangent = Key2.ArriveTangent;
	}

	if (InterpMode == RCIM_Constant)
	{
		return Key1Value;
	}

	const varying float Alpha = (Time - Key1Time) / (Key2Time - Key1Time);

	if (InterpMode == RCIM_Linear)
	{
		return lerp(Key1Value, Key2Value, Alpha);
	}

	check(InterpMode == RCIM_Cubic);

	const varying float OneThird = 1.0f / 3.0f;
	const varying float Diff = Key2Time - Key1Time;

	const varying float P0 = Key1Value;
	const varying float P3 = Key2Value;
	const varying float P1 = P0 + Key1LeaveTangent * Diff * OneThird;
	const varying float P2 = P3 - Key2ArriveTangent * Diff * OneThird;

	return BezierInterp(P0, P1, P2, P3, Alpha);
}

export void VoxelCurveFunctionLibrary_SampleCurve(
	const uniform FRichCurveKey* uniform Keys,
	const uniform int32 NumKeys,
	const uniform float Values[],
	const uniform int32 Num,
	uniform float OutValues[])
{
	check(NumKeys >= 2);

	FOREACH(Index, 0, Num)
	{
		OutValues[Index] = Eval(
			Keys,
			NumKeys,
			Values[Index]);
	}
}