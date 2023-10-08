// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Preview/VoxelBasePreviewHandlers.h"

void FVoxelPreviewHandler_Grayscale::GenerateColors(const FVoxelRuntimePinValue& Value, TVoxelArrayView<FLinearColor> Colors) const
{
	VOXEL_FUNCTION_COUNTER();

	const FVoxelFloatBuffer Buffer = Value.Get<FVoxelFloatBuffer>();

	if (!FVoxelBufferAccessor(Buffer, Colors).IsValid())
	{
		FVoxelUtilities::SetAll(Colors, FLinearColor::Red);
		return;
	}

	const FFloatInterval MinMax = Buffer.GetStorage().GetMinMaxSafe();
	MinValue = MinMax.Min;
	MaxValue = MinMax.Max;

	ParallelFor(Colors, [&](FLinearColor& OutColor, const int32 Index)
	{
		const float Scalar = Buffer[Index];

		if (!FMath::IsFinite(Scalar))
		{
			OutColor = FColor::Magenta;
			return;
		}

		const float ScaledValue = GetNormalizedValue(Scalar, MinMax);
		OutColor = FLinearColor(ScaledValue, ScaledValue, ScaledValue);
	});
}

FString FVoxelPreviewHandler_Grayscale::GetValueAt(const FVoxelRuntimePinValue& Value, const int32 Index) const
{
	const FVoxelFloatBuffer Buffer = Value.Get<FVoxelFloatBuffer>();
	if (!Buffer.IsValidIndex(Index))
	{
		return "Invalid";
	}
	return LexToSanitizedString(Buffer[Index]);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelPreviewHandler_Vector2D::GenerateColors(const FVoxelRuntimePinValue& Value, TVoxelArrayView<FLinearColor> Colors) const
{
	VOXEL_FUNCTION_COUNTER();

	const FVoxelVector2DBuffer Buffer = Value.Get<FVoxelVector2DBuffer>();

	if (!FVoxelBufferAccessor(Buffer, Colors).IsValid())
	{
		FVoxelUtilities::SetAll(Colors, FLinearColor::Red);
		return;
	}

	const FFloatInterval MinMaxX = Buffer.X.GetStorage().GetMinMaxSafe();
	const FFloatInterval MinMaxY = Buffer.Y.GetStorage().GetMinMaxSafe();

	MinValue.X = MinMaxX.Min;
	MinValue.Y = MinMaxY.Min;

	MaxValue.X = MinMaxX.Max;
	MaxValue.Y = MinMaxY.Max;

	ParallelFor(Colors, [&](FLinearColor& OutColor, const int32 Index)
	{
		const FVector2f Vector2D = Buffer[Index];

		if (Vector2D.ContainsNaN())
		{
			OutColor = FColor::Magenta;
			return;
		}

		OutColor = FLinearColor(
			GetNormalizedValue(Vector2D.X, MinMaxX),
			GetNormalizedValue(Vector2D.Y, MinMaxY),
			0);
	});
}

FString FVoxelPreviewHandler_Vector2D::GetValueAt(const FVoxelRuntimePinValue& Value, const int32 Index) const
{
	const FVoxelVector2DBuffer Buffer = Value.Get<FVoxelVector2DBuffer>();
	if (!Buffer.IsValidIndex(Index))
	{
		return "Invalid";
	}
	return Buffer[Index].ToString();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelPreviewHandler_Vector::GenerateColors(const FVoxelRuntimePinValue& Value, TVoxelArrayView<FLinearColor> Colors) const
{
	VOXEL_FUNCTION_COUNTER();

	const FVoxelVectorBuffer Buffer = Value.Get<FVoxelVectorBuffer>();

	if (!FVoxelBufferAccessor(Buffer, Colors).IsValid())
	{
		FVoxelUtilities::SetAll(Colors, FLinearColor::Red);
		return;
	}

	const FFloatInterval MinMaxX = Buffer.X.GetStorage().GetMinMaxSafe();
	const FFloatInterval MinMaxY = Buffer.Y.GetStorage().GetMinMaxSafe();
	const FFloatInterval MinMaxZ = Buffer.Z.GetStorage().GetMinMaxSafe();

	MinValue.X = MinMaxX.Min;
	MinValue.Y = MinMaxY.Min;
	MinValue.Z = MinMaxZ.Min;

	MaxValue.X = MinMaxX.Max;
	MaxValue.Y = MinMaxY.Max;
	MaxValue.Z = MinMaxZ.Max;

	ParallelFor(Colors, [&](FLinearColor& OutColor, const int32 Index)
	{
		const FVector3f Vector = Buffer[Index];

		if (Vector.ContainsNaN())
		{
			OutColor = FColor::Magenta;
			return;
		}

		OutColor = FLinearColor(
			GetNormalizedValue(Vector.X, MinMaxX),
			GetNormalizedValue(Vector.Y, MinMaxY),
			GetNormalizedValue(Vector.Z, MinMaxZ));
	});
}

FString FVoxelPreviewHandler_Vector::GetValueAt(const FVoxelRuntimePinValue& Value, const int32 Index) const
{
	const FVoxelVectorBuffer Buffer = Value.Get<FVoxelVectorBuffer>();
	if (!Buffer.IsValidIndex(Index))
	{
		return "Invalid";
	}
	return Buffer[Index].ToString();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelPreviewHandler_Color::GenerateColors(const FVoxelRuntimePinValue& Value, TVoxelArrayView<FLinearColor> Colors) const
{
	VOXEL_FUNCTION_COUNTER();

	const FVoxelLinearColorBuffer Buffer = Value.Get<FVoxelLinearColorBuffer>();

	if (!FVoxelBufferAccessor(Buffer, Colors).IsValid())
	{
		FVoxelUtilities::SetAll(Colors, FLinearColor::Red);
		return;
	}

	const FFloatInterval MinMaxR = Buffer.R.GetStorage().GetMinMaxSafe();
	const FFloatInterval MinMaxG = Buffer.G.GetStorage().GetMinMaxSafe();
	const FFloatInterval MinMaxB = Buffer.B.GetStorage().GetMinMaxSafe();
	const FFloatInterval MinMaxA = Buffer.A.GetStorage().GetMinMaxSafe();

	MinValue.R = MinMaxR.Min;
	MinValue.G = MinMaxG.Min;
	MinValue.B = MinMaxB.Min;
	MinValue.A = MinMaxA.Min;

	MaxValue.R = MinMaxR.Max;
	MaxValue.G = MinMaxG.Max;
	MaxValue.B = MinMaxB.Max;
	MaxValue.A = MinMaxA.Max;

	ParallelFor(Colors, [&](FLinearColor& OutColor, const int32 Index)
	{
		const FLinearColor Color = Buffer[Index];

		if (!FMath::IsFinite(Color.R) ||
			!FMath::IsFinite(Color.G) ||
			!FMath::IsFinite(Color.B) ||
			!FMath::IsFinite(Color.A))
		{
			OutColor = FColor::Magenta;
			return;
		}

		OutColor = FLinearColor(
			GetNormalizedValue(Color.R, MinMaxR),
			GetNormalizedValue(Color.G, MinMaxG),
			GetNormalizedValue(Color.B, MinMaxB));
	});
}

FString FVoxelPreviewHandler_Color::GetValueAt(const FVoxelRuntimePinValue& Value, const int32 Index) const
{
	const FVoxelLinearColorBuffer Buffer = Value.Get<FVoxelLinearColorBuffer>();
	if (!Buffer.IsValidIndex(Index))
	{
		return "Invalid";
	}
	return Buffer[Index].ToString();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelFutureValue FVoxelPreviewHandler_DistanceField::GetFinalValue(
	const FVoxelQuery& Query,
	const FVoxelRuntimePinValue& Value) const
{
	if (Value.Is<FVoxelFloatBuffer>())
	{
		return Value;
	}

	return Value.Get<FVoxelSurface>().GetDistance(Query);
}

void FVoxelPreviewHandler_DistanceField::GenerateColors(const FVoxelRuntimePinValue& Value, TVoxelArrayView<FLinearColor> Colors) const
{
	VOXEL_FUNCTION_COUNTER();

	const FVoxelFloatBuffer Buffer = Value.Get<FVoxelFloatBuffer>();

	if (!FVoxelBufferAccessor(Buffer, Colors).IsValid())
	{
		FVoxelUtilities::SetAll(Colors, FLinearColor::Red);
		return;
	}

	const FFloatInterval MinMax = Buffer.GetStorage().GetMinMaxSafe();
	MinValue = MinMax.Min;
	MaxValue = MinMax.Max;

	const float Divisor = FMath::Max(FMath::Abs(MinMax.Min), FMath::Abs(MinMax.Max));

	ParallelFor(Colors, [&](FLinearColor& OutColor, const int32 Index)
	{
		const float Scalar = Buffer[Index];

		if (!FMath::IsFinite(Scalar))
		{
			OutColor = FColor::Magenta;
			return;
		}

		const float ScaledValue = Scalar / Divisor;
		OutColor = FVoxelUtilities::GetDistanceFieldColor(ScaledValue);
	});
}

FString FVoxelPreviewHandler_DistanceField::GetValueAt(const FVoxelRuntimePinValue& Value, const int32 Index) const
{
	const FVoxelFloatBuffer Buffer = Value.Get<FVoxelFloatBuffer>();
	if (!Buffer.IsValidIndex(Index))
	{
		return "Invalid";
	}
	return LexToSanitizedString(Buffer[Index]);
}