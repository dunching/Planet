// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelISPCNode.h"
#include "Buffer/VoxelFloatBuffers.h"
#include "VoxelMathNodes.generated.h"

// Returns the sine of A (expects Radians)
USTRUCT(Category = "Math|Trig", DisplayName = "Sin (Radians)", meta = (CompactNodeTitle = "SIN"))
struct VOXELGRAPHCORE_API FVoxelNode_Sin : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	// Angle in radians
	VOXEL_TEMPLATE_INPUT_PIN(float, Value, nullptr);
	// sin(x), between -1 and 1
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = sin({Value})";
	}
};

// Returns the sine of A (expects Degrees)
USTRUCT(Category = "Math|Trig", DisplayName = "Sin (Degrees)", meta = (CompactNodeTitle = "SINd"))
struct VOXELGRAPHCORE_API FVoxelNode_SinDegrees : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	// Angle in degrees
	VOXEL_TEMPLATE_INPUT_PIN(float, Value, nullptr);
	// sin(x), between -1 and 1
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = sin(PI / 180.f * {Value})";
	}
};

// Returns the cosine of A (expects Radians)
USTRUCT(Category = "Math|Trig", DisplayName = "Cos (Radians)", meta = (CompactNodeTitle = "COS"))
struct VOXELGRAPHCORE_API FVoxelNode_Cos : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	// Angle in radians
	VOXEL_TEMPLATE_INPUT_PIN(float, Value, nullptr);
	// cos(x), between -1 and 1
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = cos({Value})";
	}
};

// Returns the cosine of A (expects Degrees)
USTRUCT(Category = "Math|Trig", DisplayName = "Cos (Degrees)", meta = (CompactNodeTitle = "COSd"))
struct VOXELGRAPHCORE_API FVoxelNode_CosDegrees : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	// Angle in degrees
	VOXEL_TEMPLATE_INPUT_PIN(float, Value, nullptr);
	// cos(x), between -1 and 1
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = cos(PI / 180.f * {Value})";
	}
};

// Returns the tangent of A (expects Radians)
USTRUCT(Category = "Math|Trig", DisplayName = "Tan (Radians)", meta = (CompactNodeTitle = "TAN"))
struct VOXELGRAPHCORE_API FVoxelNode_Tan : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	// Angle in radians
	VOXEL_TEMPLATE_INPUT_PIN(float, Value, nullptr);
	// tan(x)
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = tan({Value})";
	}
};

// Returns the tangent of A (expects Degrees)
USTRUCT(Category = "Math|Trig", DisplayName = "Tan (Degrees)", meta = (CompactNodeTitle = "TANd"))
struct VOXELGRAPHCORE_API FVoxelNode_TanDegrees : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	// Angle in degrees
	VOXEL_TEMPLATE_INPUT_PIN(float, Value, nullptr);
	// tan(x)
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = tan(PI / 180.f * {Value})";
	}
};

// Returns the inverse sine (arcsin) of A (result is in Radians)
USTRUCT(Category = "Math|Trig", DisplayName = "Asin (Radians)", meta = (CompactNodeTitle = "ASIN"))
struct VOXELGRAPHCORE_API FVoxelNode_Asin : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	// Value between -1 and 1
	VOXEL_TEMPLATE_INPUT_PIN(float, Value, nullptr);
	// Angle in radians
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = asin({Value})";
	}
};

// Returns the inverse sine (arcsin) of A (result is in Degrees)
USTRUCT(Category = "Math|Trig", DisplayName = "Asin (Degrees)", meta = (CompactNodeTitle = "ASINd"))
struct VOXELGRAPHCORE_API FVoxelNode_AsinDegrees : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	// Value between -1 and 1
	VOXEL_TEMPLATE_INPUT_PIN(float, Value, nullptr);
	// Angle in degrees
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = 180.f / PI * asin({Value})";
	}
};

// Returns the inverse cosine (arccos) of A (result is in Radians)
USTRUCT(Category = "Math|Trig", DisplayName = "Acos (Radians)", meta = (CompactNodeTitle = "ACOS"))
struct VOXELGRAPHCORE_API FVoxelNode_Acos : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	// Value between -1 and 1
	VOXEL_TEMPLATE_INPUT_PIN(float, Value, nullptr);
	// Angle in radians
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = acos({Value})";
	}
};

// Returns the inverse cosine (arccos) of A (result is in Degrees)
USTRUCT(Category = "Math|Trig", DisplayName = "Acos (Degrees)", meta = (CompactNodeTitle = "ACOSd"))
struct VOXELGRAPHCORE_API FVoxelNode_AcosDegrees : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	// Value between -1 and 1
	VOXEL_TEMPLATE_INPUT_PIN(float, Value, nullptr);
	// Angle in degrees
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = 180.f / PI * acos({Value})";
	}
};

// Returns the inverse tan (atan) (result is in Radians)
USTRUCT(Category = "Math|Trig", DisplayName = "Atan (Radians)", meta = (CompactNodeTitle = "ATAN"))
struct VOXELGRAPHCORE_API FVoxelNode_Atan : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	// Value
	VOXEL_TEMPLATE_INPUT_PIN(float, Value, nullptr);
	// Angle in radians
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = atan({Value})";
	}
};

// Returns the inverse tan (atan) (result is in Degrees)
USTRUCT(Category = "Math|Trig", DisplayName = "Atan (Degrees)", meta = (CompactNodeTitle = "ATANd"))
struct VOXELGRAPHCORE_API FVoxelNode_AtanDegrees : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	// Value
	VOXEL_TEMPLATE_INPUT_PIN(float, Value, nullptr);
	// Angle in degrees
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = 180.f / PI * atan({Value})";
	}
};

// Returns the inverse tan (atan2) of Y/X (result is in Radians)
USTRUCT(Category = "Math|Trig", DisplayName = "Atan2 (Radians)", meta = (CompactNodeTitle = "ATAN2"))
struct VOXELGRAPHCORE_API FVoxelNode_Atan2 : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(float, Y, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(float, X, nullptr);
	// Angle in radians
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = atan2({Y}, {X})";
	}
};

// Returns the inverse tan (atan2) of Y/X (result is in Degrees)
USTRUCT(Category = "Math|Trig", DisplayName = "Atan2 (Degrees)", meta = (CompactNodeTitle = "ATAN2d"))
struct VOXELGRAPHCORE_API FVoxelNode_Atan2Degrees : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(float, Y, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(float, X, nullptr);
	// Angle in degrees
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = 180.f / PI * atan2({Y}, {X})";
	}
};

USTRUCT(Category = "Math|Trig", DisplayName = "Get PI", meta = (CompactNodeTitle = "PI"))
struct VOXELGRAPHCORE_API FVoxelNode_GetPI : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = PI";
	}
};

USTRUCT(Category = "Math|Trig", meta = (CompactNodeTitle = "R2D"))
struct VOXELGRAPHCORE_API FVoxelNode_RadiansToDegrees : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(float, A, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {A} * (180.f / PI)";
	}
};

USTRUCT(Category = "Math|Trig", meta = (CompactNodeTitle = "D2R"))
struct VOXELGRAPHCORE_API FVoxelNode_DegreesToRadians : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(float, A, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {A} * (PI / 180.f)";
	}
};

// Linearly interpolates between four input values based on input coordinates
USTRUCT(Category = "Math|Float")
struct VOXELGRAPHCORE_API FVoxelNode_BilinearInterpolation : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	// Usually frac(Position)
	VOXEL_TEMPLATE_INPUT_PIN(FVector2D, Alpha, nullptr);
	// Value at X=0 Y=0
	VOXEL_TEMPLATE_INPUT_PIN(float, X0Y0, nullptr);
	// Value at X=1 Y=0
	VOXEL_TEMPLATE_INPUT_PIN(float, X1Y0, nullptr);
	// Value at X=0 Y=1
	VOXEL_TEMPLATE_INPUT_PIN(float, X0Y1, nullptr);
	// Value at X=1 Y=1
	VOXEL_TEMPLATE_INPUT_PIN(float, X1Y1, nullptr);
	// Interpolated value
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = BilinearInterpolation({X0Y0}, {X1Y0}, {X0Y1}, {X1Y1}, {Alpha}.x, {Alpha}.y)";
	}
};

// Returns the logical complement of the Boolean value (NOT A)
USTRUCT(Category = "Math|Boolean", DisplayName = "NOT Boolean", meta = (Keywords = "! not negate", CompactNodeTitle = "NOT"))
struct VOXELGRAPHCORE_API FVoxelNode_BooleanNOT : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(bool, A, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(bool, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = !{A}";
	}
};

// Returns the logical Not OR of two values: !(A || B)
USTRUCT(Category = "Math|Boolean", DisplayName = "NOR Boolean", meta = (Keywords = "!^ nor", CompactNodeTitle = "NOR"))
struct VOXELGRAPHCORE_API FVoxelNode_BooleanNOR : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(bool, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(bool, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(bool, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = !({A} || {B})";
	}
};

// Returns the logical exclusive OR of two values (A XOR B)
USTRUCT(Category = "Math|Boolean", DisplayName = "XOR Boolean", meta = (Keywords = "^ xor", CompactNodeTitle = "XOR"))
struct VOXELGRAPHCORE_API FVoxelNode_BooleanXOR : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(bool, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(bool, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(bool, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {A} != {B}";
	}
};

USTRUCT(Category = "Math|Integer")
struct VOXELGRAPHCORE_API FVoxelNode_LeftShift : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(int32, Value, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(int32, Shift, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(int32, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "IGNORE_PERF_WARNING\n{ReturnValue} = {Value} << clamp({Shift}, 0, 31)";
	}
};

USTRUCT(Category = "Math|Integer")
struct VOXELGRAPHCORE_API FVoxelNode_RightShift : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(int32, Value, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(int32, Shift, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(int32, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "IGNORE_PERF_WARNING\n{ReturnValue} = {Value} >> clamp({Shift}, 0, 31)";
	}
};

// Bitwise AND (A & B)
USTRUCT(Category = "Math|Integer", DisplayName = "Bitwise AND", meta = (Keywords = "& and", CompactNodeTitle = "&"))
struct VOXELGRAPHCORE_API FVoxelNode_Bitwise_And : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(int32, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(int32, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(int32, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {A} & {B}";
	}
};

// Bitwise OR (A | B)
USTRUCT(Category = "Math|Integer", DisplayName = "Bitwise OR", meta = (Keywords = "| or", CompactNodeTitle = "|"))
struct VOXELGRAPHCORE_API FVoxelNode_Bitwise_Or : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(int32, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(int32, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(int32, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {A} | {B}";
	}
};

// Bitwise XOR (A ^ B)
USTRUCT(Category = "Math|Integer", DisplayName = "Bitwise XOR", meta = (Keywords = "^ xor", CompactNodeTitle = "^"))
struct VOXELGRAPHCORE_API FVoxelNode_Bitwise_Xor : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(int32, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(int32, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(int32, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {A} ^ {B}";
	}
};

// Bitwise NOT (~A)
USTRUCT(Category = "Math|Integer", DisplayName = "Bitwise NOT", meta = (Keywords = "~ not", CompactNodeTitle = "~"))
struct VOXELGRAPHCORE_API FVoxelNode_Bitwise_Not : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(int32, A, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(int32, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = ~{A}";
	}
};

USTRUCT(Category = "Math|Rotation", meta = (NativeMakeFunc))
struct VOXELGRAPHCORE_API FVoxelNode_MakeQuaternion : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(float, Roll, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(float, Pitch, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(float, Yaw, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(FQuat, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = MakeQuaternionFromEuler({Pitch}, {Yaw}, {Roll})";
	}
};

USTRUCT(Category = "Math|Rotation", meta = (NativeBreakFunc))
struct VOXELGRAPHCORE_API FVoxelNode_BreakQuaternion : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FQuat, Quat, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(float, Roll);
	VOXEL_TEMPLATE_OUTPUT_PIN(float, Pitch);
	VOXEL_TEMPLATE_OUTPUT_PIN(float, Yaw);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "MakeEulerFromQuaternion({Quat}, {Pitch}, {Yaw}, {Roll})";
	}
};

// Make a rotation from a Z axis. Z doesn't need to be normalized
USTRUCT(Category = "Math|Rotation")
struct VOXELGRAPHCORE_API FVoxelNode_MakeRotationFromZ : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	// Doesn't need to be normalized
	VOXEL_TEMPLATE_INPUT_PIN(FVector, Z, nullptr);
	// Rotation
	VOXEL_TEMPLATE_OUTPUT_PIN(FQuat, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = MakeQuaternionFromZ({Z})";
	}
};

USTRUCT(Category = "Color")
struct VOXELGRAPHCORE_API FVoxelNode_DistanceToColor : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(float, Distance, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(float, Scale, 100.f);
	VOXEL_TEMPLATE_OUTPUT_PIN(FLinearColor, Color);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{Color} = GetDistanceFieldColor({Distance} / {Scale})";
	}
};