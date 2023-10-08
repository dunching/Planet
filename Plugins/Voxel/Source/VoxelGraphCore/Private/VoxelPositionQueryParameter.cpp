// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelPositionQueryParameter.h"
#include "VoxelBufferUtilities.h"
#include "VoxelPositionQueryParameterImpl.ispc.generated.h"

FVoxelBox FVoxelPositionQueryParameter::GetBounds() const
{
	VOXEL_SCOPE_LOCK(CriticalSection);

	if (CachedBounds_RequiresLock)
	{
		return *CachedBounds_RequiresLock;
	}

	if (PrecomputedBounds)
	{
		CachedBounds_RequiresLock = PrecomputedBounds.GetValue();
		return *CachedBounds_RequiresLock;
	}

	if (!CachedPositions_RequiresLock)
	{
		VOXEL_SCOPE_COUNTER("Compute positions");
		CachedPositions_RequiresLock = (*Compute)();
		ensure(CachedPositions_RequiresLock->Num() > 0);
	}

	VOXEL_SCOPE_COUNTER("Compute bounds");

	const FFloatInterval MinMaxX = CachedPositions_RequiresLock->X.GetStorage().GetMinMaxSafe();
	const FFloatInterval MinMaxY = CachedPositions_RequiresLock->Y.GetStorage().GetMinMaxSafe();
	const FFloatInterval MinMaxZ = CachedPositions_RequiresLock->Z.GetStorage().GetMinMaxSafe();

	CachedBounds_RequiresLock = FVoxelBox(
		FVector(MinMaxX.Min, MinMaxY.Min, MinMaxZ.Min),
		FVector(MinMaxX.Max, MinMaxY.Max, MinMaxZ.Max));

	return *CachedBounds_RequiresLock;
}

FVoxelVectorBuffer FVoxelPositionQueryParameter::GetPositions() const
{
	VOXEL_SCOPE_LOCK(CriticalSection);

	if (CachedPositions_RequiresLock)
	{
		return *CachedPositions_RequiresLock;
	}

	VOXEL_SCOPE_COUNTER("Compute positions");
	CachedPositions_RequiresLock = (*Compute)();
	ensure(CachedPositions_RequiresLock->Num() > 0);

	return *CachedPositions_RequiresLock;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelPositionQueryParameter::Initialize(
	const FVoxelVectorBuffer& NewPositions,
	const TOptional<FVoxelBox>& NewBounds)
{
	ensure(NewPositions.Num() > 0);

	PrecomputedBounds = NewBounds;
	Compute = MakeVoxelShared<TVoxelUniqueFunction<FVoxelVectorBuffer()>>([NewPositions]
	{
		return NewPositions;
	});

	if (VOXEL_DEBUG && NewBounds)
	{
		CheckBounds();
	}
}

void FVoxelPositionQueryParameter::Initialize(
	TVoxelUniqueFunction<FVoxelVectorBuffer()>&& NewCompute,
	const TOptional<FVoxelBox>& NewBounds)
{
	PrecomputedBounds = NewBounds;
	Compute = MakeSharedCopy(MoveTemp(NewCompute));

	if (VOXEL_DEBUG && NewBounds)
	{
		CheckBounds();
	}
}

void FVoxelPositionQueryParameter::InitializeGradient(
	const FVoxelVectorBuffer& NewPositions,
	const TOptional<FVoxelBox>& NewBounds)
{
	ensure(NewPositions.Num() > 0);

	bIsGradient = true;
	PrecomputedBounds = NewBounds;
	Compute = MakeVoxelShared<TVoxelUniqueFunction<FVoxelVectorBuffer()>>([NewPositions]
	{
		return NewPositions;
	});

	if (VOXEL_DEBUG && NewBounds)
	{
		CheckBounds();
	}
}

void FVoxelPositionQueryParameter::InitializeGrid(
	const FVector3f& Start,
	const float Step,
	const FIntVector& Size)
{
	if (!ensure(int64(Size.X) * int64(Size.Y) * int64(Size.Z) < MAX_int32))
	{
		return;
	}

	Grid = MakeVoxelShared<FGrid>(FGrid
	{
		Start,
		Step,
		Size
	});

	Initialize(
		[=]
		{
			VOXEL_SCOPE_COUNTER("VoxelPositionQueryParameter_WritePositions3D");

			const int32 Num = Size.X * Size.Y * Size.Z;

			FVoxelFloatBufferStorage X; X.Allocate(Num);
			FVoxelFloatBufferStorage Y;	Y.Allocate(Num);
			FVoxelFloatBufferStorage Z;	Z.Allocate(Num);

			ForeachVoxelBufferChunk(Num, [&](const FVoxelBufferIterator& Iterator)
			{
				ispc::VoxelPositionQueryParameter_WritePositions3D(
					X.GetData(Iterator),
					Y.GetData(Iterator),
					Z.GetData(Iterator),
					Start.X,
					Start.Y,
					Start.Z,
					Size.X,
					Size.Y,
					Step,
					Iterator.GetIndex(),
					Iterator.Num());
			});

			return FVoxelVectorBuffer::Make(X, Y, Z);
		},
		FVoxelBox(FVector(Start), FVector(Start) + Step * FVector(Size)));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelQuery FVoxelPositionQueryParameter::TransformQuery(const FVoxelQuery& Query, const FMatrix& Transform)
{
	VOXEL_FUNCTION_COUNTER();

	const FVoxelPositionQueryParameter* PositionQueryParameter = Query.GetParameters().Find<FVoxelPositionQueryParameter>();
	if (!ensure(PositionQueryParameter))
	{
		return Query;
	}

	if (Transform.Equals(FMatrix::Identity))
	{
		// Nothing to be done
		return Query;
	}

	const FVoxelVectorBuffer Positions = PositionQueryParameter->GetPositions();

	const TSharedRef<FVoxelQueryParameters> Parameters = Query.CloneParameters();
	if (PositionQueryParameter->Grid &&
		Transform.Rotator().IsNearlyZero() &&
		Transform.GetScaleVector().IsUniform())
	{
		Parameters->Add<FVoxelPositionQueryParameter>().InitializeGrid(
			FMatrix44f(Transform).TransformPosition(PositionQueryParameter->Grid->Start),
			PositionQueryParameter->Grid->Step * Transform.GetMaximumAxisScale(),
			PositionQueryParameter->Grid->Size);
	}
	else
	{
		Parameters->Add<FVoxelPositionQueryParameter>().Initialize([=]
		{
			return FVoxelBufferUtilities::ApplyTransform(Positions, Transform);
		});
	}
	return Query.MakeNewQuery(Parameters);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelPositionQueryParameter::CheckBounds() const
{
	VOXEL_FUNCTION_COUNTER();

	const FVoxelVectorBuffer Positions = GetPositions();
	const FVoxelBox Bounds = GetBounds();

	for (int32 Index = 0; Index < Positions.Num(); Index++)
	{
		const FVector3f Position = Positions[Index];

		ensure(Bounds.Min.X - 1 <= Position.X);
		ensure(Bounds.Min.Y - 1 <= Position.Y);
		ensure(Bounds.Min.Z - 1 <= Position.Z);

		ensure(Position.X <= Bounds.Max.X + 1);
		ensure(Position.Y <= Bounds.Max.Y + 1);
		ensure(Position.Z <= Bounds.Max.Z + 1);
	}
}