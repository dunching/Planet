// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelISPCNodeHelpers.isph"
#include "VoxelInterpolationNodesImpl.isph"

enum EVoxelInterpolationType{	EVoxelInterpolationType_Linear = 0,
	EVoxelInterpolationType_SmoothStep = 1,
	EVoxelInterpolationType_Step = 2,
	EVoxelInterpolationType_SinusoidalIn = 3,
	EVoxelInterpolationType_SinusoidalOut = 4,
	EVoxelInterpolationType_SinusoidalInOut = 5,
	EVoxelInterpolationType_EaseIn = 6,
	EVoxelInterpolationType_EaseOut = 7,
	EVoxelInterpolationType_EaseInOut = 8,
	EVoxelInterpolationType_ExpoIn = 9,
	EVoxelInterpolationType_ExpoOut = 10,
	EVoxelInterpolationType_ExpoInOut = 11,
	EVoxelInterpolationType_CircularIn = 12,
	EVoxelInterpolationType_CircularOut = 13,
	EVoxelInterpolationType_CircularInOut = 14,
	EVoxelInterpolationType_EVoxelInterpolationType_MAX = 15,
};
export void VoxelNode_VoxelNode_Interpolate(const FVoxelBuffer* uniform Buffers, const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const uint8 InterpolationType = LoadByte(Buffers[0], Index);
		const float A = LoadFloat(Buffers[1], Index);
		const float B = LoadFloat(Buffers[2], Index);
		const float Alpha = LoadFloat(Buffers[3], Index);
		const float Exp = LoadFloat(Buffers[4], Index);
		const int32 Steps = LoadInt32(Buffers[5], Index);
		float ReturnValue;

		switch (InterpolationType)
		{
			default: check(false);
			ReturnValue = 0.f;
			break;
			case EVoxelInterpolationType_Linear: ReturnValue = lerp(A, B, Alpha);
			break;
			case EVoxelInterpolationType_SmoothStep: ReturnValue = SmoothStep(A, B, Alpha);
			break;
			case EVoxelInterpolationType_Step: ReturnValue = InterpStep(A, B, Alpha, Steps);
			break;
			case EVoxelInterpolationType_SinusoidalIn: ReturnValue = InterpSinIn(A, B, Alpha);
			break;
			case EVoxelInterpolationType_SinusoidalOut: ReturnValue = InterpSinOut(A, B, Alpha);
			break;
			case EVoxelInterpolationType_SinusoidalInOut: ReturnValue = InterpSinInOut(A, B, Alpha);
			break;
			case EVoxelInterpolationType_EaseIn: ReturnValue = InterpEaseIn(A, B, Alpha, Exp);
			break;
			case EVoxelInterpolationType_EaseOut: ReturnValue = InterpEaseOut(A, B, Alpha, Exp);
			break;
			case EVoxelInterpolationType_EaseInOut: ReturnValue = InterpEaseInOut(A, B, Alpha, Exp);
			break;
			case EVoxelInterpolationType_ExpoIn: ReturnValue = InterpExpoIn(A, B, Alpha);
			break;
			case EVoxelInterpolationType_ExpoOut: ReturnValue = InterpExpoOut(A, B, Alpha);
			break;
			case EVoxelInterpolationType_ExpoInOut: ReturnValue = InterpExpoInOut(A, B, Alpha);
			break;
			case EVoxelInterpolationType_CircularIn: ReturnValue = InterpCircularIn(A, B, Alpha);
			break;
			case EVoxelInterpolationType_CircularOut: ReturnValue = InterpCircularOut(A, B, Alpha);
			break;
			case EVoxelInterpolationType_CircularInOut: ReturnValue = InterpCircularInOut(A, B, Alpha);
			break;
		}
		;

		StoreFloat(Buffers[6], Index, ReturnValue);
	}
}