// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelBufferUtilities.h"
#include "Point/VoxelPointId.h"
#include "VoxelPositionQueryParameter.h"
#include "VoxelBufferUtilitiesImpl.ispc.generated.h"

FVoxelVectorBuffer FVoxelBufferUtilities::ApplyTransform(const FVoxelVectorBuffer& Buffer, const FTransform& Transform)
{
	if (Transform.Equals(FTransform::Identity))
	{
		return Buffer;
	}

	VOXEL_FUNCTION_COUNTER_NUM(Buffer.Num(), 1024);

	FVoxelFloatBufferStorage ResultX; ResultX.Allocate(Buffer.Num());
	FVoxelFloatBufferStorage ResultY; ResultY.Allocate(Buffer.Num());
	FVoxelFloatBufferStorage ResultZ; ResultZ.Allocate(Buffer.Num());

	ForeachVoxelBufferChunk(Buffer.Num(), [&](const FVoxelBufferIterator& Iterator)
	{
		ispc::VoxelBufferUtilities_ApplyTransform(
			Buffer.X.GetData(Iterator), Buffer.X.IsConstant(),
			Buffer.Y.GetData(Iterator), Buffer.Y.IsConstant(),
			Buffer.Z.GetData(Iterator), Buffer.Z.IsConstant(),
			Iterator.Num(),
			GetISPCValue(FVector3f(Transform.GetTranslation())),
			GetISPCValue(FVector4f(
				Transform.GetRotation().X,
				Transform.GetRotation().Y,
				Transform.GetRotation().Z,
				Transform.GetRotation().W)),
			GetISPCValue(FVector3f(Transform.GetScale3D())),
			ResultX.GetData(Iterator),
			ResultY.GetData(Iterator),
			ResultZ.GetData(Iterator));
	});

	FVoxelVectorBuffer Result;
	Result.X = FVoxelFloatBuffer::Make(ResultX);
	Result.Y = FVoxelFloatBuffer::Make(ResultY);
	Result.Z = FVoxelFloatBuffer::Make(ResultZ);
	return Result;
}

FVoxelVectorBuffer FVoxelBufferUtilities::ApplyInverseTransform(const FVoxelVectorBuffer& Buffer, const FTransform& Transform)
{
	if (Transform.Equals(FTransform::Identity))
	{
		return Buffer;
	}

	VOXEL_FUNCTION_COUNTER_NUM(Buffer.Num(), 1024);

	FVoxelFloatBufferStorage ResultX; ResultX.Allocate(Buffer.Num());
	FVoxelFloatBufferStorage ResultY; ResultY.Allocate(Buffer.Num());
	FVoxelFloatBufferStorage ResultZ; ResultZ.Allocate(Buffer.Num());

	ForeachVoxelBufferChunk(Buffer.Num(), [&](const FVoxelBufferIterator& Iterator)
	{
		ispc::VoxelBufferUtilities_ApplyInverseTransform(
			Buffer.X.GetData(Iterator), Buffer.X.IsConstant(),
			Buffer.Y.GetData(Iterator), Buffer.Y.IsConstant(),
			Buffer.Z.GetData(Iterator), Buffer.Z.IsConstant(),
			Iterator.Num(),
			GetISPCValue(FVector3f(Transform.GetTranslation())),
			GetISPCValue(FVector4f(
				Transform.GetRotation().X,
				Transform.GetRotation().Y,
				Transform.GetRotation().Z,
				Transform.GetRotation().W)),
			GetISPCValue(FVector3f(Transform.GetScale3D())),
			ResultX.GetData(Iterator),
			ResultY.GetData(Iterator),
			ResultZ.GetData(Iterator));
	});

	FVoxelVectorBuffer Result;
	Result.X = FVoxelFloatBuffer::Make(ResultX);
	Result.Y = FVoxelFloatBuffer::Make(ResultY);
	Result.Z = FVoxelFloatBuffer::Make(ResultZ);
	return Result;
}

FVoxelVectorBuffer FVoxelBufferUtilities::ApplyTransform(
	const FVoxelVectorBuffer& Buffer,
	const FVoxelVectorBuffer& Translation,
	const FVoxelQuaternionBuffer& Rotation,
	const FVoxelVectorBuffer& Scale)
{
	const FVoxelBufferAccessor BufferAccessor(Buffer, Translation, Rotation, Scale);
	if (!ensure(BufferAccessor.IsValid()))
	{
		return {};
	}

	VOXEL_FUNCTION_COUNTER_NUM(BufferAccessor.Num(), 1024);

	FVoxelFloatBufferStorage ResultX; ResultX.Allocate(BufferAccessor.Num());
	FVoxelFloatBufferStorage ResultY; ResultY.Allocate(BufferAccessor.Num());
	FVoxelFloatBufferStorage ResultZ; ResultZ.Allocate(BufferAccessor.Num());

	ForeachVoxelBufferChunk(BufferAccessor.Num(), [&](const FVoxelBufferIterator& Iterator)
	{
		ispc::VoxelBufferUtilities_ApplyTransform_Bulk(
			Buffer.X.GetData(Iterator), Buffer.X.IsConstant(),
			Buffer.Y.GetData(Iterator), Buffer.Y.IsConstant(),
			Buffer.Z.GetData(Iterator), Buffer.Z.IsConstant(),
			Translation.X.GetData(Iterator), Translation.X.IsConstant(),
			Translation.Y.GetData(Iterator), Translation.Y.IsConstant(),
			Translation.Z.GetData(Iterator), Translation.Z.IsConstant(),
			Rotation.X.GetData(Iterator), Rotation.X.IsConstant(),
			Rotation.Y.GetData(Iterator), Rotation.Y.IsConstant(),
			Rotation.Z.GetData(Iterator), Rotation.Z.IsConstant(),
			Rotation.W.GetData(Iterator), Rotation.W.IsConstant(),
			Scale.X.GetData(Iterator), Scale.X.IsConstant(),
			Scale.Y.GetData(Iterator), Scale.Y.IsConstant(),
			Scale.Z.GetData(Iterator), Scale.Z.IsConstant(),
			Iterator.Num(),
			ResultX.GetData(Iterator),
			ResultY.GetData(Iterator),
			ResultZ.GetData(Iterator));
	});

	return FVoxelVectorBuffer::Make(ResultX, ResultY, ResultZ);
}

FVoxelVectorBuffer FVoxelBufferUtilities::ApplyTransform(const FVoxelVectorBuffer& Buffer, const FMatrix& Matrix)
{
	const FTransform Transform(Matrix);
	if (Transform.ToMatrixWithScale().Equals(Matrix))
	{
		return ApplyTransform(Buffer, Transform);
	}

	ensure(!Matrix.GetScaleVector().IsUniform());
	VOXEL_SCOPE_COUNTER_NUM("Non-Uniform Scale", Buffer.Num(), 1024);

	const FVector Translation = Matrix.GetOrigin();
	const FMatrix BaseMatrix = Matrix.RemoveTranslation();
	ensure(Matrix.Equals(BaseMatrix * FTranslationMatrix(Translation)));

	ensure(FMath::IsNearlyEqual(BaseMatrix.M[0][3], 0));
	ensure(FMath::IsNearlyEqual(BaseMatrix.M[1][3], 0));
	ensure(FMath::IsNearlyEqual(BaseMatrix.M[2][3], 0));
	ensure(FMath::IsNearlyEqual(BaseMatrix.M[3][0], 0));
	ensure(FMath::IsNearlyEqual(BaseMatrix.M[3][1], 0));
	ensure(FMath::IsNearlyEqual(BaseMatrix.M[3][2], 0));
	ensure(FMath::IsNearlyEqual(BaseMatrix.M[3][3], 1));

	FVoxelFloatBufferStorage ResultX; ResultX.Allocate(Buffer.Num());
	FVoxelFloatBufferStorage ResultY; ResultY.Allocate(Buffer.Num());
	FVoxelFloatBufferStorage ResultZ; ResultZ.Allocate(Buffer.Num());

	ForeachVoxelBufferChunk(Buffer.Num(), [&](const FVoxelBufferIterator& Iterator)
	{
		ispc::VoxelBufferUtilities_ApplyTransform_Matrix(
			Buffer.X.GetData(Iterator), Buffer.X.IsConstant(),
			Buffer.Y.GetData(Iterator), Buffer.Y.IsConstant(),
			Buffer.Z.GetData(Iterator), Buffer.Z.IsConstant(),
			Iterator.Num(),
			GetISPCValue(FVector3f(Translation)),
			GetISPCValue(FVector3f(BaseMatrix.M[0][0], BaseMatrix.M[0][1], BaseMatrix.M[0][2])),
			GetISPCValue(FVector3f(BaseMatrix.M[1][0], BaseMatrix.M[1][1], BaseMatrix.M[1][2])),
			GetISPCValue(FVector3f(BaseMatrix.M[2][0], BaseMatrix.M[2][1], BaseMatrix.M[2][2])),
			ResultX.GetData(Iterator),
			ResultY.GetData(Iterator),
			ResultZ.GetData(Iterator));
	});

	FVoxelVectorBuffer Result;
	Result.X = FVoxelFloatBuffer::Make(ResultX);
	Result.Y = FVoxelFloatBuffer::Make(ResultY);
	Result.Z = FVoxelFloatBuffer::Make(ResultZ);
	return Result;
}

FVoxelFloatBuffer FVoxelBufferUtilities::TransformDistance(const FVoxelFloatBuffer& Distance, const FMatrix& Transform)
{
	const float Scale = Transform.GetScaleVector().GetAbsMax();
	if (Scale == 1.f)
	{
		return Distance;
	}

	VOXEL_FUNCTION_COUNTER_NUM(Distance.Num(), 1024);

	FVoxelFloatBufferStorage Result;
	Result.Allocate(Distance.Num());

	ForeachVoxelBufferChunk(Distance.Num(), [&](const FVoxelBufferIterator& Iterator)
	{
		ispc::VoxelBufferUtilities_TransformDistance(
			Distance.GetData(Iterator),
			Result.GetData(Iterator),
			Scale,
			Iterator.Num());
	});

	return FVoxelFloatBuffer::Make(Result);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelFloatBuffer FVoxelBufferUtilities::Min8(
	const FVoxelFloatBuffer& Buffer0,
	const FVoxelFloatBuffer& Buffer1,
	const FVoxelFloatBuffer& Buffer2,
	const FVoxelFloatBuffer& Buffer3,
	const FVoxelFloatBuffer& Buffer4,
	const FVoxelFloatBuffer& Buffer5,
	const FVoxelFloatBuffer& Buffer6,
	const FVoxelFloatBuffer& Buffer7)
{
	const FVoxelBufferAccessor BufferAccessor(
		Buffer0,
		Buffer1,
		Buffer2,
		Buffer3,
		Buffer4,
		Buffer5,
		Buffer6,
		Buffer7);
	if (!ensure(BufferAccessor.IsValid()))
	{
		return {};
	}

	VOXEL_FUNCTION_COUNTER_NUM(BufferAccessor.Num(), 1024);

	FVoxelFloatBufferStorage Result;
	Result.Allocate(BufferAccessor.Num());

	ForeachVoxelBufferChunk(BufferAccessor.Num(), [&](const FVoxelBufferIterator& Iterator)
	{
		ispc::VoxelBufferUtilities_Min8(
			Buffer0.GetData(Iterator), Buffer0.IsConstant(),
			Buffer1.GetData(Iterator), Buffer1.IsConstant(),
			Buffer2.GetData(Iterator), Buffer2.IsConstant(),
			Buffer3.GetData(Iterator), Buffer3.IsConstant(),
			Buffer4.GetData(Iterator), Buffer4.IsConstant(),
			Buffer5.GetData(Iterator), Buffer5.IsConstant(),
			Buffer6.GetData(Iterator), Buffer6.IsConstant(),
			Buffer7.GetData(Iterator), Buffer7.IsConstant(),
			Result.GetData(Iterator),
			Iterator.Num());
	});

	return FVoxelFloatBuffer::Make(Result);
}

FVoxelFloatBuffer FVoxelBufferUtilities::Max8(
	const FVoxelFloatBuffer& Buffer0,
	const FVoxelFloatBuffer& Buffer1,
	const FVoxelFloatBuffer& Buffer2,
	const FVoxelFloatBuffer& Buffer3,
	const FVoxelFloatBuffer& Buffer4,
	const FVoxelFloatBuffer& Buffer5,
	const FVoxelFloatBuffer& Buffer6,
	const FVoxelFloatBuffer& Buffer7)
{
	const FVoxelBufferAccessor BufferAccessor(
		Buffer0,
		Buffer1,
		Buffer2,
		Buffer3,
		Buffer4,
		Buffer5,
		Buffer6,
		Buffer7);
	if (!ensure(BufferAccessor.IsValid()))
	{
		return {};
	}

	VOXEL_FUNCTION_COUNTER_NUM(BufferAccessor.Num(), 1024);

	FVoxelFloatBufferStorage Result;
	Result.Allocate(BufferAccessor.Num());

	ForeachVoxelBufferChunk(BufferAccessor.Num(), [&](const FVoxelBufferIterator& Iterator)
	{
		ispc::VoxelBufferUtilities_Max8(
			Buffer0.GetData(Iterator), Buffer0.IsConstant(),
			Buffer1.GetData(Iterator), Buffer1.IsConstant(),
			Buffer2.GetData(Iterator), Buffer2.IsConstant(),
			Buffer3.GetData(Iterator), Buffer3.IsConstant(),
			Buffer4.GetData(Iterator), Buffer4.IsConstant(),
			Buffer5.GetData(Iterator), Buffer5.IsConstant(),
			Buffer6.GetData(Iterator), Buffer6.IsConstant(),
			Buffer7.GetData(Iterator), Buffer7.IsConstant(),
			Result.GetData(Iterator),
			Iterator.Num());
	});

	return FVoxelFloatBuffer::Make(Result);
}

FVoxelVectorBuffer FVoxelBufferUtilities::Min8(
	const FVoxelVectorBuffer& Buffer0,
	const FVoxelVectorBuffer& Buffer1,
	const FVoxelVectorBuffer& Buffer2,
	const FVoxelVectorBuffer& Buffer3,
	const FVoxelVectorBuffer& Buffer4,
	const FVoxelVectorBuffer& Buffer5,
	const FVoxelVectorBuffer& Buffer6,
	const FVoxelVectorBuffer& Buffer7)
{
	FVoxelVectorBuffer Result;
	Result.X = Min8(Buffer0.X, Buffer1.X, Buffer2.X, Buffer3.X, Buffer4.X, Buffer5.X, Buffer6.X, Buffer7.X);
	Result.Y = Min8(Buffer0.Y, Buffer1.Y, Buffer2.Y, Buffer3.Y, Buffer4.Y, Buffer5.Y, Buffer6.Y, Buffer7.Y);
	Result.Z = Min8(Buffer0.Z, Buffer1.Z, Buffer2.Z, Buffer3.Z, Buffer4.Z, Buffer5.Z, Buffer6.Z, Buffer7.Z);
	return Result;
}

FVoxelVectorBuffer FVoxelBufferUtilities::Max8(
	const FVoxelVectorBuffer& Buffer0,
	const FVoxelVectorBuffer& Buffer1,
	const FVoxelVectorBuffer& Buffer2,
	const FVoxelVectorBuffer& Buffer3,
	const FVoxelVectorBuffer& Buffer4,
	const FVoxelVectorBuffer& Buffer5,
	const FVoxelVectorBuffer& Buffer6,
	const FVoxelVectorBuffer& Buffer7)
{
	FVoxelVectorBuffer Result;
	Result.X = Max8(Buffer0.X, Buffer1.X, Buffer2.X, Buffer3.X, Buffer4.X, Buffer5.X, Buffer6.X, Buffer7.X);
	Result.Y = Max8(Buffer0.Y, Buffer1.Y, Buffer2.Y, Buffer3.Y, Buffer4.Y, Buffer5.Y, Buffer6.Y, Buffer7.Y);
	Result.Z = Max8(Buffer0.Z, Buffer1.Z, Buffer2.Z, Buffer3.Z, Buffer4.Z, Buffer5.Z, Buffer6.Z, Buffer7.Z);
	return Result;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelFloatBuffer FVoxelBufferUtilities::IntToFloat(const FVoxelInt32Buffer& Buffer)
{
	VOXEL_FUNCTION_COUNTER_NUM(Buffer.Num(), 1024);

	FVoxelFloatBufferStorage Result;
	Result.Allocate(Buffer.Num());

	ForeachVoxelBufferChunk(Buffer.Num(), [&](const FVoxelBufferIterator& Iterator)
	{
		ispc::VoxelBufferUtilities_IntToFloat(
			Buffer.GetData(Iterator),
			Result.GetData(Iterator),
			Iterator.Num());
	});

	return FVoxelFloatBuffer::Make(Result);
}

FVoxelDoubleBuffer FVoxelBufferUtilities::IntToDouble(const FVoxelInt32Buffer& Buffer)
{
	VOXEL_FUNCTION_COUNTER_NUM(Buffer.Num(), 1024);

	FVoxelDoubleBufferStorage Result;
	Result.Allocate(Buffer.Num());

	ForeachVoxelBufferChunk(Buffer.Num(), [&](const FVoxelBufferIterator& Iterator)
	{
		ispc::VoxelBufferUtilities_IntToDouble(
			Buffer.GetData(Iterator),
			Result.GetData(Iterator),
			Iterator.Num());
	});

	return FVoxelDoubleBuffer::Make(Result);
}

FVoxelDoubleBuffer FVoxelBufferUtilities::FloatToDouble(const FVoxelFloatBuffer& Float)
{
	VOXEL_FUNCTION_COUNTER_NUM(Float.Num(), 1024);

	FVoxelDoubleBufferStorage Result;
	Result.Allocate(Float.Num());

	ForeachVoxelBufferChunk(Float.Num(), [&](const FVoxelBufferIterator& Iterator)
	{
		ispc::VoxelBufferUtilities_FloatToDouble(
			Float.GetData(Iterator),
			Result.GetData(Iterator),
			Iterator.Num());
	});

	return FVoxelDoubleBuffer::Make(Result);
}

FVoxelFloatBuffer FVoxelBufferUtilities::DoubleToFloat(const FVoxelDoubleBuffer& Double)
{
	VOXEL_FUNCTION_COUNTER_NUM(Double.Num(), 1024);

	FVoxelFloatBufferStorage Result;
	Result.Allocate(Double.Num());

	ForeachVoxelBufferChunk(Double.Num(), [&](const FVoxelBufferIterator& Iterator)
	{
		ispc::VoxelBufferUtilities_DoubleToFloat(
			Double.GetData(Iterator),
			Result.GetData(Iterator),
			Iterator.Num());
	});

	return FVoxelFloatBuffer::Make(Result);
}

FVoxelSeedBuffer FVoxelBufferUtilities::PointIdToSeed(const FVoxelPointIdBuffer& Buffer)
{
	VOXEL_FUNCTION_COUNTER_NUM(Buffer.Num(), 1024);

	FVoxelSeedBufferStorage Result;
	Result.Allocate(Buffer.Num());

	ForeachVoxelBufferChunk(Buffer.Num(), [&](const FVoxelBufferIterator& Iterator)
	{
		ispc::VoxelBufferUtilities_PointIdToSeed(
			ReinterpretCastPtr<uint64>(Buffer.GetData(Iterator)),
			ReinterpretCastPtr<uint32>(Result.GetData(Iterator)),
			Iterator.Num());
	});

	return FVoxelSeedBuffer::Make(Result);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelFloatBuffer FVoxelBufferUtilities::Add(const FVoxelFloatBuffer& A, const FVoxelFloatBuffer& B)
{
	const FVoxelBufferAccessor BufferAccessor(A, B);
	if (!ensure(BufferAccessor.IsValid()))
	{
		return {};
	}

	VOXEL_FUNCTION_COUNTER_NUM(BufferAccessor.Num(), 1024);

	FVoxelFloatBufferStorage Result;
	Result.Allocate(BufferAccessor.Num());

	ForeachVoxelBufferChunk(BufferAccessor.Num(), [&](const FVoxelBufferIterator& Iterator)
	{
		ispc::VoxelBufferUtilities_Add(
			A.GetData(Iterator),
			A.IsConstant(),
			B.GetData(Iterator),
			B.IsConstant(),
			Result.GetData(Iterator),
			Iterator.Num());
	});

	return FVoxelFloatBuffer::Make(Result);
}

FVoxelFloatBuffer FVoxelBufferUtilities::Multiply(const FVoxelFloatBuffer& A, const FVoxelFloatBuffer& B)
{
	const FVoxelBufferAccessor BufferAccessor(A, B);
	if (!ensure(BufferAccessor.IsValid()))
	{
		return {};
	}

	VOXEL_FUNCTION_COUNTER_NUM(BufferAccessor.Num(), 1024);

	FVoxelFloatBufferStorage Result;
	Result.Allocate(BufferAccessor.Num());

	ForeachVoxelBufferChunk(BufferAccessor.Num(), [&](const FVoxelBufferIterator& Iterator)
	{
		ispc::VoxelBufferUtilities_Multiply(
			A.GetData(Iterator),
			A.IsConstant(),
			B.GetData(Iterator),
			B.IsConstant(),
			Result.GetData(Iterator),
			Iterator.Num());
	});

	return FVoxelFloatBuffer::Make(Result);
}

FVoxelBoolBuffer FVoxelBufferUtilities::Less(const FVoxelFloatBuffer& A, const FVoxelFloatBuffer& B)
{
	const FVoxelBufferAccessor BufferAccessor(A, B);
	if (!ensure(BufferAccessor.IsValid()))
	{
		return {};
	}

	VOXEL_FUNCTION_COUNTER_NUM(BufferAccessor.Num(), 1024);

	FVoxelBoolBufferStorage Result;
	Result.Allocate(BufferAccessor.Num());

	ForeachVoxelBufferChunk(BufferAccessor.Num(), [&](const FVoxelBufferIterator& Iterator)
	{
		ispc::VoxelBufferUtilities_Less(
			A.GetData(Iterator),
			A.IsConstant(),
			B.GetData(Iterator),
			B.IsConstant(),
			Result.GetData(Iterator),
			Iterator.Num());
	});

	return FVoxelBoolBuffer::Make(Result);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelVectorBuffer FVoxelBufferUtilities::Add(const FVoxelVectorBuffer& A, const FVoxelVectorBuffer& B)
{
	FVoxelVectorBuffer Result;
	Result.X = Add(A.X, B.X);
	Result.Y = Add(A.Y, B.Y);
	Result.Z = Add(A.Z, B.Z);
	return Result;
}

FVoxelVectorBuffer FVoxelBufferUtilities::Multiply(const FVoxelVectorBuffer& A, const FVoxelVectorBuffer& B)
{
	FVoxelVectorBuffer Result;
	Result.X = Multiply(A.X, B.X);
	Result.Y = Multiply(A.Y, B.Y);
	Result.Z = Multiply(A.Z, B.Z);
	return Result;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelFloatBuffer FVoxelBufferUtilities::Lerp(const FVoxelFloatBuffer& A, const FVoxelFloatBuffer& B, const FVoxelFloatBuffer& Alpha)
{
	const FVoxelBufferAccessor BufferAccessor(A, B, Alpha);
	if (!ensure(BufferAccessor.IsValid()))
	{
		return {};
	}

	VOXEL_FUNCTION_COUNTER_NUM(BufferAccessor.Num(), 1024);

	FVoxelFloatBufferStorage Result;
	Result.Allocate(BufferAccessor.Num());

	ForeachVoxelBufferChunk(BufferAccessor.Num(), [&](const FVoxelBufferIterator& Iterator)
	{
		ispc::VoxelBufferUtilities_Alpha(
			A.GetData(Iterator),
			A.IsConstant(),
			B.GetData(Iterator),
			B.IsConstant(),
			Alpha.GetData(Iterator),
			Alpha.IsConstant(),
			Result.GetData(Iterator),
			Iterator.Num());
	});

	return FVoxelFloatBuffer::Make(Result);
}

FVoxelVector2DBuffer FVoxelBufferUtilities::Lerp(const FVoxelVector2DBuffer& A, const FVoxelVector2DBuffer& B, const FVoxelFloatBuffer& Alpha)
{
	FVoxelVector2DBuffer Result;
	Result.X = Lerp(A.X, B.X, Alpha);
	Result.Y = Lerp(A.Y, B.Y, Alpha);
	return Result;
}

FVoxelVectorBuffer FVoxelBufferUtilities::Lerp(const FVoxelVectorBuffer& A, const FVoxelVectorBuffer& B, const FVoxelFloatBuffer& Alpha)
{
	FVoxelVectorBuffer Result;
	Result.X = Lerp(A.X, B.X, Alpha);
	Result.Y = Lerp(A.Y, B.Y, Alpha);
	Result.Z = Lerp(A.Z, B.Z, Alpha);
	return Result;
}

FVoxelLinearColorBuffer FVoxelBufferUtilities::Lerp(const FVoxelLinearColorBuffer& A, const FVoxelLinearColorBuffer& B, const FVoxelFloatBuffer& Alpha)
{
	FVoxelLinearColorBuffer Result;
	Result.R = Lerp(A.R, B.R, Alpha);
	Result.G = Lerp(A.G, B.G, Alpha);
	Result.B = Lerp(A.B, B.B, Alpha);
	Result.A = Lerp(A.A, B.A, Alpha);
	return Result;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelQuaternionBuffer FVoxelBufferUtilities::Combine(const FVoxelQuaternionBuffer& A, const FVoxelQuaternionBuffer& B)
{
	const FVoxelBufferAccessor BufferAccessor(A, B);
	if (!ensure(BufferAccessor.IsValid()))
	{
		return {};
	}

	VOXEL_FUNCTION_COUNTER_NUM(BufferAccessor.Num(), 1024);

	FVoxelFloatBufferStorage X;
	FVoxelFloatBufferStorage Y;
	FVoxelFloatBufferStorage Z;
	FVoxelFloatBufferStorage W;
	X.Allocate(BufferAccessor.Num());
	Y.Allocate(BufferAccessor.Num());
	Z.Allocate(BufferAccessor.Num());
	W.Allocate(BufferAccessor.Num());

	ForeachVoxelBufferChunk(BufferAccessor.Num(), [&](const FVoxelBufferIterator& Iterator)
	{
		ispc::VoxelBufferUtilities_Combine(
			A.X.GetData(Iterator),
			A.X.IsConstant(),
			A.Y.GetData(Iterator),
			A.Y.IsConstant(),
			A.Z.GetData(Iterator),
			A.Z.IsConstant(),
			A.W.GetData(Iterator),
			A.W.IsConstant(),
			B.X.GetData(Iterator),
			B.X.IsConstant(),
			B.Y.GetData(Iterator),
			B.Y.IsConstant(),
			B.Z.GetData(Iterator),
			B.Z.IsConstant(),
			B.W.GetData(Iterator),
			B.W.IsConstant(),
			X.GetData(Iterator),
			Y.GetData(Iterator),
			Z.GetData(Iterator),
			W.GetData(Iterator),
			Iterator.Num());
	});

	return FVoxelQuaternionBuffer::Make(X, Y, Z, W);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelBufferUtilities::MakePalette(
	const FVoxelTerminalBuffer& Buffer,
	FVoxelInt32Buffer& OutIndices,
	FVoxelTerminalBuffer& OutPalette)
{
	VOXEL_FUNCTION_COUNTER_NUM(Buffer.Num(), 1024);
	check(Buffer.GetStruct() == OutPalette.GetStruct());

	if (Buffer.Num() == 0)
	{
		OutIndices = FVoxelInt32Buffer::MakeEmpty();
		OutPalette.SetAsEmpty();
		return;
	}
	if (Buffer.IsConstant())
	{
		OutIndices = FVoxelInt32Buffer::Make(0);
		Buffer.CopyTo(OutPalette);
		return;
	}

	if (Buffer.IsA<FVoxelSimpleTerminalBuffer>())
	{
		const FVoxelSimpleTerminalBuffer& SimpleBuffer = CastChecked<FVoxelSimpleTerminalBuffer>(Buffer);
		FVoxelSimpleTerminalBuffer& OutSimplePalette = CastChecked<FVoxelSimpleTerminalBuffer>(OutPalette);

		FVoxelInt32BufferStorage Indices;
		Indices.Allocate(Buffer.Num());

		const TSharedRef<FVoxelBufferStorage> Palette = OutSimplePalette.MakeNewStorage();

		VOXEL_SWITCH_TERMINAL_TYPE_SIZE(SimpleBuffer.GetTypeSize())
		{
			using Type = VOXEL_GET_TYPE(TypeInstance);

			TVoxelAddOnlyChunkedMap<Type, int32> ValueToIndex;
			ValueToIndex.Reserve(Buffer.Num());

			for (int32 Index = 0; Index < Buffer.Num(); Index++)
			{
				const Type Value = SimpleBuffer.GetStorage<Type>()[Index];

				if (const int32* ExistingIndex = ValueToIndex.Find(Value))
				{
					Indices[Index] = *ExistingIndex;
				}
				else
				{
					const int32 NewIndex = Palette->As<Type>().Add(Value);
					ValueToIndex.Add_CheckNew_NoRehash(Value, NewIndex);
					Indices[Index] = NewIndex;
				}
			}

			OutIndices.SetStorage(Indices);
			OutSimplePalette.SetStorage(Palette);
		};
	}
	else
	{
		// Not supported
		ensure(false);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelBufferUtilities::Select(
	FVoxelTerminalBuffer& OutBuffer,
	const FVoxelBuffer& Indices,
	const TConstVoxelArrayView<const FVoxelTerminalBuffer*> Buffers)
{
	VOXEL_SCOPE_COUNTER_FORMAT_COND(Indices.Num() > 1024, "Select Num=%d", Indices.Num());

	if (!ensure(Indices.Num() >= 2) ||
		!ensure(Buffers.Num() > 0))
	{
		return;
	}

	const FVoxelPinType InnerType = OutBuffer.GetInnerType();
	for (const FVoxelTerminalBuffer* Buffer : Buffers)
	{
		if (!ensure(Buffer->GetInnerType() == InnerType) ||
			!ensure(Buffer->Num() == 1 || Buffer->Num() == Indices.Num()))
		{
			return;
		}
	}

	if (OutBuffer.IsA<FVoxelSimpleTerminalBuffer>())
	{
		FVoxelSimpleTerminalBuffer& OutSimpleBuffer = CastChecked<FVoxelSimpleTerminalBuffer>(OutBuffer);
		const TConstVoxelArrayView<const FVoxelSimpleTerminalBuffer*> SimpleBuffers = ReinterpretCastVoxelArrayView<const FVoxelSimpleTerminalBuffer*>(Buffers);

		const TSharedRef<FVoxelBufferStorage> Storage = OutSimpleBuffer.MakeNewStorage();
		Storage->Allocate(Indices.Num());

		if (Indices.IsA<FVoxelBoolBuffer>())
		{
			ForeachVoxelBufferChunk(Indices.Num(), [&](const FVoxelBufferIterator& Iterator)
			{
				const TConstVoxelArrayView<bool> ConditionView = Indices.AsChecked<FVoxelBoolBuffer>().GetRawView_NotConstant(Iterator);

				VOXEL_SWITCH_TERMINAL_TYPE_SIZE(Storage->GetTypeSize())
				{
					using Type = VOXEL_GET_TYPE(TypeInstance);

					const TVoxelArrayView<Type> WriteView = Storage->As<Type>().GetRawView_NotConstant(Iterator);
					checkVoxelSlow(Buffers.Num() == 2);

					const TVoxelBufferStorage<Type>& FalseBuffer = SimpleBuffers[0]->GetStorage<Type>();
					const TVoxelBufferStorage<Type>& TrueBuffer = SimpleBuffers[1]->GetStorage<Type>();

					if (FalseBuffer.IsConstant() ||
						TrueBuffer.IsConstant())
					{
						for (int32 Index = 0; Index < Iterator.Num(); Index++)
						{
							WriteView[Index] = ConditionView[Index] ? TrueBuffer[Index] : FalseBuffer[Index];
						}
					}
					else
					{
						const TConstVoxelArrayView<Type> False = FalseBuffer.GetRawView_NotConstant(Iterator);
						const TConstVoxelArrayView<Type> True = TrueBuffer.GetRawView_NotConstant(Iterator);

						for (int32 Index = 0; Index < Iterator.Num(); Index++)
						{
							WriteView[Index] = ConditionView[Index] ? True[Index] : False[Index];
						}
					}
				};
			});
		}
		else if (Indices.IsA<FVoxelByteBuffer>())
		{
			ForeachVoxelBufferChunk(Indices.Num(), [&](const FVoxelBufferIterator& Iterator)
			{
				const TConstVoxelArrayView<uint8> IndicesView = Indices.AsChecked<FVoxelByteBuffer>().GetRawView_NotConstant(Iterator);

				VOXEL_SWITCH_TERMINAL_TYPE_SIZE(Storage->GetTypeSize())
				{
					using Type = VOXEL_GET_TYPE(TypeInstance);
					const TVoxelArrayView<Type> WriteView = Storage->As<Type>().GetRawView_NotConstant(Iterator);

					for (int32 Index = 0; Index < Iterator.Num(); Index++)
					{
						const int32 BufferIndex = IndicesView[Index];

						if (!SimpleBuffers.IsValidIndex(BufferIndex))
						{
							WriteView[Index] = 0;
							continue;
						}

						const FVoxelSimpleTerminalBuffer* SimpleBuffer = SimpleBuffers[BufferIndex];
						WriteView[Index] = SimpleBuffer->GetStorage<Type>()[Iterator.GetIndex() + Index];
					}
				};
			});
		}
		else
		{
			ForeachVoxelBufferChunk(Indices.Num(), [&](const FVoxelBufferIterator& Iterator)
			{
				const TConstVoxelArrayView<int32> IndicesView = Indices.AsChecked<FVoxelInt32Buffer>().GetRawView_NotConstant(Iterator);

				VOXEL_SWITCH_TERMINAL_TYPE_SIZE(Storage->GetTypeSize())
				{
					using Type = VOXEL_GET_TYPE(TypeInstance);
					const TVoxelArrayView<Type> WriteView = Storage->As<Type>().GetRawView_NotConstant(Iterator);

					for (int32 Index = 0; Index < Iterator.Num(); Index++)
					{
						const int32 BufferIndex = IndicesView[Index];

						if (!SimpleBuffers.IsValidIndex(BufferIndex))
						{
							WriteView[Index] = 0;
							continue;
						}

						const FVoxelSimpleTerminalBuffer* SimpleBuffer = SimpleBuffers[BufferIndex];
						WriteView[Index] = SimpleBuffer->GetStorage<Type>()[Iterator.GetIndex() + Index];
					}
				};
			});
		}

		OutSimpleBuffer.SetStorage(Storage);
	}
	else
	{
		FVoxelComplexTerminalBuffer& OutComplexBuffer = CastChecked<FVoxelComplexTerminalBuffer>(OutBuffer);
		const TConstVoxelArrayView<const FVoxelComplexTerminalBuffer*> ComplexBuffers = ReinterpretCastVoxelArrayView<const FVoxelComplexTerminalBuffer*>(Buffers);

		const TSharedRef<FVoxelComplexBufferStorage> Storage = OutComplexBuffer.MakeNewStorage();
		Storage->Allocate(Indices.Num());

		if (Indices.IsA<FVoxelBoolBuffer>())
		{
			check(Buffers.Num() == 2);

			for (int32 Index = 0; Index < Indices.Num(); Index++)
			{
				ComplexBuffers[Indices.AsChecked<FVoxelBoolBuffer>()[Index] ? 1 : 0]->GetStorage()[Index].CopyTo((*Storage)[Index]);
			}
		}
		else if (Indices.IsA<FVoxelByteBuffer>())
		{
			for (int32 Index = 0; Index < Indices.Num(); Index++)
			{
				const int32 BufferIndex = Indices.AsChecked<FVoxelByteBuffer>()[Index];
				if (!ComplexBuffers.IsValidIndex(BufferIndex))
				{
					continue;
				}

				ComplexBuffers[BufferIndex]->GetStorage()[Index].CopyTo((*Storage)[Index]);
			}
		}
		else
		{
			for (int32 Index = 0; Index < Indices.Num(); Index++)
			{
				const int32 BufferIndex = Indices.AsChecked<FVoxelInt32Buffer>()[Index];
				if (!ComplexBuffers.IsValidIndex(BufferIndex))
				{
					continue;
				}

				ComplexBuffers[BufferIndex]->GetStorage()[Index].CopyTo((*Storage)[Index]);
			}
		}

		OutComplexBuffer.SetStorage(Storage);
	}
}

TSharedRef<const FVoxelBuffer> FVoxelBufferUtilities::Select(
	const FVoxelPinType& InnerType,
	const FVoxelBuffer& Indices,
	const TConstVoxelArrayView<const FVoxelBuffer*> Buffers)
{
	check(Buffers.Num() > 0);

	if (!ensure(
		Indices.IsA<FVoxelBoolBuffer>() ||
		Indices.IsA<FVoxelByteBuffer>() ||
		Indices.IsA<FVoxelInt32Buffer>()))
	{
		return FVoxelBuffer::Make(InnerType);
	}

	if (Indices.IsA<FVoxelBoolBuffer>() &&
		!ensure(Buffers.Num() == 2))
	{
		return FVoxelBuffer::Make(InnerType);
	}

	int32 Num = Indices.Num();
	for (const FVoxelBuffer* Buffer : Buffers)
	{
		if (!ensure(Buffer->GetInnerType().CanBeCastedTo(InnerType)) ||
			!ensure(FVoxelBufferAccessor::MergeNum(Num, *Buffer)))
		{
			return FVoxelBuffer::Make(InnerType);
		}
	}

	VOXEL_FUNCTION_COUNTER_NUM(Num, 1024);

	if (Indices.IsConstant())
	{
		if (Indices.IsA<FVoxelBoolBuffer>())
		{
			return Buffers[Indices.AsChecked<FVoxelBoolBuffer>().GetConstant() ? 1 : 0]->MakeSharedCopy();
		}
		else if (Indices.IsA<FVoxelByteBuffer>())
		{
			const int32 Index = Indices.AsChecked<FVoxelByteBuffer>().GetConstant();
			if (!Buffers.IsValidIndex(Index))
			{
				return FVoxelBuffer::Make(InnerType);
			}
			return Buffers[Index]->MakeSharedCopy();
		}
		else
		{
			const int32 Index = Indices.AsChecked<FVoxelInt32Buffer>().GetConstant();
			if (!Buffers.IsValidIndex(Index))
			{
				return FVoxelBuffer::Make(InnerType);
			}
			return Buffers[Index]->MakeSharedCopy();
		}
	}

	const TSharedRef<FVoxelBuffer> NewBuffer = FVoxelBuffer::Make(InnerType);
	for (int32 Index = 0; Index < NewBuffer->NumTerminalBuffers(); Index++)
	{
		TVoxelArray<const FVoxelTerminalBuffer*> TerminalBuffers;
		TerminalBuffers.Reserve(Buffers.Num());

		for (const FVoxelBuffer* Buffer : Buffers)
		{
			checkVoxelSlow(Buffer->NumTerminalBuffers() == NewBuffer->NumTerminalBuffers());
			TerminalBuffers.Add(&Buffer->GetTerminalBuffer(Index));
		}

		Select(NewBuffer->GetTerminalBuffer(Index), Indices, TerminalBuffers);
	}
	return NewBuffer;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelBufferUtilities::Gather(
	FVoxelTerminalBuffer& OutBuffer,
	const FVoxelTerminalBuffer& Buffer,
	const FVoxelInt32Buffer& Indices)
{
	VOXEL_SCOPE_COUNTER_FORMAT_COND(Indices.Num() > 1024, "FVoxelBufferUtilities::Gather Num=%d", Indices.Num());
	check(OutBuffer.GetStruct() == Buffer.GetStruct());

	if (Buffer.IsConstant())
	{
		Buffer.CopyTo(OutBuffer);
		return;
	}
	if (Indices.Num() == 0)
	{
		OutBuffer.SetAsEmpty();
		return;
	}

	if (Indices.Num() == 1)
	{
		const int32 Index = Indices[0];
		if (Index == -1)
		{
			OutBuffer.InitializeFromConstant(FVoxelRuntimePinValue(OutBuffer.GetInnerType()));
		}
		else
		{
			OutBuffer.InitializeFromConstant(Buffer.GetGeneric(Index));
		}
		return;
	}

	if (Buffer.IsA<FVoxelSimpleTerminalBuffer>())
	{
		FVoxelSimpleTerminalBuffer& OutSimpleBuffer = CastChecked<FVoxelSimpleTerminalBuffer>(OutBuffer);
		const FVoxelSimpleTerminalBuffer& SimpleBuffer = CastChecked<FVoxelSimpleTerminalBuffer>(Buffer);
		const int32 TypeSize = SimpleBuffer.GetTypeSize();

		const TSharedRef<FVoxelBufferStorage> Storage = OutSimpleBuffer.MakeNewStorage();
		Storage->Allocate(Indices.Num());

		ForeachVoxelBufferChunk(Indices.Num(), [&](const FVoxelBufferIterator& Iterator)
		{
			const TConstVoxelArrayView<int32> IndicesView = Indices.GetRawView_NotConstant(Iterator);

			VOXEL_SWITCH_TERMINAL_TYPE_SIZE(TypeSize)
			{
				using Type = VOXEL_GET_TYPE(TypeInstance);

				const TVoxelArrayView<Type> WriteView = Storage->As<Type>().GetRawView_NotConstant(Iterator);
				const TVoxelBufferStorage<Type>& ReadView = SimpleBuffer.GetStorage<Type>();

				for (int32 WriteIndex = 0; WriteIndex < IndicesView.Num(); WriteIndex++)
				{
					const int32 ReadIndex = IndicesView[WriteIndex];
					WriteView[WriteIndex] = ReadIndex == -1 ? 0 : ReadView[ReadIndex];
				}
			};
		});

		OutSimpleBuffer.SetStorage(Storage);
	}
	else
	{
		FVoxelComplexTerminalBuffer& OutComplexBuffer = CastChecked<FVoxelComplexTerminalBuffer>(OutBuffer);
		const FVoxelComplexTerminalBuffer& ComplexBuffer = CastChecked<FVoxelComplexTerminalBuffer>(Buffer);

		const TSharedRef<FVoxelComplexBufferStorage> Storage = OutComplexBuffer.MakeNewStorage();
		Storage->Allocate(Indices.Num());

		for (int32 WriteIndex = 0; WriteIndex < Indices.Num(); WriteIndex++)
		{
			const int32 ReadIndex = Indices[WriteIndex];
			if (ReadIndex == -1)
			{
				continue;
			}

			ComplexBuffer.GetStorage()[ReadIndex].CopyTo((*Storage)[WriteIndex]);
		}

		OutComplexBuffer.SetStorage(Storage);
	}
}

TSharedRef<const FVoxelBuffer> FVoxelBufferUtilities::Gather(
	const FVoxelBuffer& Buffer,
	const FVoxelInt32Buffer& Indices)
{
	VOXEL_FUNCTION_COUNTER_NUM(Indices.Num(), 1024);

	const TSharedRef<FVoxelBuffer> NewBuffer = FVoxelBuffer::Make(Buffer.GetInnerType());
	check(Buffer.NumTerminalBuffers() == NewBuffer->NumTerminalBuffers());
	for (int32 Index = 0; Index < Buffer.NumTerminalBuffers(); Index++)
	{
		Gather(
			NewBuffer->GetTerminalBuffer(Index),
			Buffer.GetTerminalBuffer(Index),
			Indices);
	}
	return NewBuffer;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelBufferUtilities::Replicate(
	FVoxelTerminalBuffer& OutBuffer,
	const FVoxelTerminalBuffer& Buffer,
	const TConstVoxelArrayView<int32> Counts,
	const int32 NewNum)
{
	VOXEL_SCOPE_COUNTER_FORMAT_COND(NewNum > 1024, "FVoxelBufferUtilities::Replicate Num=%d", NewNum);

	if (NewNum == 0)
	{
		return;
	}
	if (Buffer.IsConstant())
	{
		Buffer.CopyTo(OutBuffer);
		return;
	}

	if (VOXEL_DEBUG)
	{
		int32 ActualNum = 0;
		for (const int32 Count : Counts)
		{
			ActualNum += Count;
		}
		check(NewNum == ActualNum);
	}

	if (Buffer.IsA<FVoxelSimpleTerminalBuffer>())
	{
		FVoxelSimpleTerminalBuffer& OutSimpleBuffer = CastChecked<FVoxelSimpleTerminalBuffer>(OutBuffer);
		const FVoxelSimpleTerminalBuffer& SimpleBuffer = CastChecked<FVoxelSimpleTerminalBuffer>(Buffer);
		const int32 TypeSize = SimpleBuffer.GetTypeSize();

		const TSharedRef<FVoxelBufferStorage> Storage = OutSimpleBuffer.MakeNewStorage();
		Storage->Allocate(NewNum);

		VOXEL_SWITCH_TERMINAL_TYPE_SIZE(TypeSize)
		{
			using Type = VOXEL_GET_TYPE(TypeInstance);

			int32 WriteIndex = 0;
			for (int32 ReadIndex = 0; ReadIndex < Counts.Num(); ReadIndex++)
			{
				const int32 Count = Counts[ReadIndex];
				const Type Value = SimpleBuffer.GetStorage<Type>()[ReadIndex];

				FVoxelBufferIterator Iterator;
				Iterator.Initialize(WriteIndex + Count, WriteIndex);
				for (; Iterator; ++Iterator)
				{
					FVoxelUtilities::SetAll(
						Storage->As<Type>().GetRawView_NotConstant(Iterator),
						Value);
				}

				WriteIndex += Count;
			}
			ensure(WriteIndex == NewNum);
		};

		OutSimpleBuffer.SetStorage(Storage);
	}
	else
	{
		FVoxelComplexTerminalBuffer& OutComplexBuffer = CastChecked<FVoxelComplexTerminalBuffer>(OutBuffer);
		const FVoxelComplexTerminalBuffer& ComplexBuffer = CastChecked<FVoxelComplexTerminalBuffer>(Buffer);

		const TSharedRef<FVoxelComplexBufferStorage> Storage = OutComplexBuffer.MakeNewStorage();
		Storage->Allocate(NewNum);

		int32 WriteIndex = 0;
		for (int32 ReadIndex = 0; ReadIndex < Counts.Num(); ReadIndex++)
		{
			const int32 Count = Counts[ReadIndex];
			const FConstVoxelStructView Value = ComplexBuffer.GetStorage()[ReadIndex];

			for (int32 Index = 0; Index < Count; Index++)
			{
				Value.CopyTo((*Storage)[WriteIndex++]);
			}
		}
		ensure(WriteIndex == NewNum);

		OutComplexBuffer.SetStorage(Storage);
	}
}

TSharedRef<const FVoxelBuffer> FVoxelBufferUtilities::Replicate(
	const FVoxelBuffer& Buffer,
	const TConstVoxelArrayView<int32> Counts,
	const int32 NewNum)
{
	VOXEL_FUNCTION_COUNTER_NUM(Buffer.Num(), 1024);

	if (VOXEL_DEBUG)
	{
		int32 NewNumCheck = 0;
		for (const int32 Count : Counts)
		{
			NewNumCheck += Count;
		}
		check(NewNum == NewNumCheck);
	}

	const TSharedRef<FVoxelBuffer> NewBuffer = FVoxelBuffer::Make(Buffer.GetInnerType());

	for (int32 Index = 0; Index < NewBuffer->NumTerminalBuffers(); Index++)
	{
		Replicate(
			NewBuffer->GetTerminalBuffer(Index),
			Buffer.GetTerminalBuffer(Index),
			Counts,
			NewNum);
	}

	return NewBuffer;
}