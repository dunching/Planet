// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelDistanceFieldUtilities_Old.h"

void FVoxelDistanceFieldUtilities::JumpFlood(
	const FIntVector& Size,
	TVoxelArray<FVector3f>& InOutSurfacePositions,
	const bool bParallel,
	const FVoxelIntBox* InBounds)
{
	check(InOutSurfacePositions.Num() == Size.X * Size.Y * Size.Z);

	const FVoxelIntBox DefaultBounds(0, Size);
	check(!InBounds || InBounds->IsValid());
	check(!InBounds || DefaultBounds.Contains(*InBounds));
	const FVoxelIntBox Bounds = InBounds ? *InBounds : DefaultBounds;

	VOXEL_SCOPE_COUNTER_FORMAT("JumpFlood %dx%dx%d", Bounds.Size().X, Bounds.Size().Y, Bounds.Size().Z);

	bool bUseTempAsSrc = false;

	TVoxelArray<FVector3f> Temp;
	Temp.Empty(InOutSurfacePositions.Num());
	Temp.SetNumUninitialized(InOutSurfacePositions.Num());

	const int32 PowerOfTwo = FMath::CeilLogTwo(Size.GetMax());
	for (int32 Pass = 0; Pass < PowerOfTwo; Pass++)
	{
		// -1: we want to start with half the size
		const int32 Step = 1 << (PowerOfTwo - 1 - Pass);
		JumpFloodStep_CPU(
			Size,
			Bounds,
			bUseTempAsSrc ? Temp : InOutSurfacePositions,
			bUseTempAsSrc ? InOutSurfacePositions : Temp,
			Step,
			bParallel);

		bUseTempAsSrc = !bUseTempAsSrc;
	}

	if (bUseTempAsSrc)
	{
		InOutSurfacePositions = MoveTemp(Temp);
	}
}

void FVoxelDistanceFieldUtilities::GetDistancesFromSurfacePositions(
	const FIntVector& Size,
	TVoxelArrayView<const FVector3f> SurfacePositions,
	TVoxelArrayView<float> InOutDistances,
	const FVoxelIntBox* InBounds)
{
	VOXEL_FUNCTION_COUNTER();

	check(SurfacePositions.Num() == InOutDistances.Num());
	check(SurfacePositions.Num() == Size.X * Size.Y * Size.Z);

	const FVoxelIntBox DefaultBounds(0, Size);
	check(!InBounds || InBounds->IsValid());
	check(!InBounds || DefaultBounds.Contains(*InBounds));

	const FVoxelIntBox Bounds = InBounds ? *InBounds : DefaultBounds;

	for (int32 Z = Bounds.Min.Z; Z < Bounds.Max.Z; Z++)
	{
		for (int32 Y = Bounds.Min.Y; Y < Bounds.Max.Y; Y++)
		{
			float* RESTRICT const DistancePtr = &InOutDistances[FVoxelUtilities::Get3DIndex<int32>(Size, 0, Y, Z)];
			const FVector3f* RESTRICT const SurfacePositionPtr = &SurfacePositions[FVoxelUtilities::Get3DIndex<int32>(Size, 0, Y, Z)];
			for (int32 X = Bounds.Min.X; X < Bounds.Max.X; X++)
			{
				float& Distance = *(DistancePtr + X);
				const FVector3f& SurfacePosition = *(SurfacePositionPtr + X);

				checkVoxelSlow(&Distance == &InOutDistances[FVoxelUtilities::Get3DIndex<int32>(Size, X, Y, Z)]);
				checkVoxelSlow(&SurfacePosition == &SurfacePositions[FVoxelUtilities::Get3DIndex<int32>(Size, X, Y, Z)]);

				ensureVoxelSlow(IsSurfacePositionValid(SurfacePosition));

				const float NewDistance = FVector3f::Distance(FVector3f(X, Y, Z), SurfacePosition);

				// Keep sign
				Distance = Distance > 0 ? NewDistance : -NewDistance;

				ensureVoxelSlow(FMath::Abs(Distance) < Size.Size() * 2);
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelDistanceFieldUtilities::GetSurfacePositionsFromDensities(
	const FIntVector& Size,
	TVoxelArrayView<const float> Densities,
	TVoxelArrayView<float> OutDistances,
	TVoxelArrayView<FVector3f> OutSurfacePositions,
	bool bParallel,
	FVoxelOptionalIntBox* OutBounds)
{
	VOXEL_FUNCTION_COUNTER();

	const FIntVector DensitiesSize = Size + 2;

	check(Densities.Num() == DensitiesSize.X * DensitiesSize.Y * DensitiesSize.Z);
	check(OutDistances.Num() == Size.X * Size.Y * Size.Z);
	check(OutSurfacePositions.Num() == Size.X * Size.Y * Size.Z);

	const auto Body = [&](auto ComputeOutBounds, const int32 Z)
	{
		for (int32 Y = 0; Y < Size.Y; Y++)
		{
			const FIntVector PositionYZ(0, Y, Z);
			int32 DensitiesIndex = FVoxelUtilities::Get3DIndex<int32>(DensitiesSize, PositionYZ + 1);
			int32 OutIndex = FVoxelUtilities::Get3DIndex<int32>(Size, PositionYZ);
			for (int32 X = 0; X < Size.X; X++)
			{
				ON_SCOPE_EXIT
				{
					DensitiesIndex++;
					OutIndex++;
				};

				const float Value = Densities[DensitiesIndex];

				// Take the max: this is the one that will "push" the value the closest to us
				// Only consider positive values, so that there's a surface between us
				// By symmetry, take the min value negative if Value is positive
				float MaxNeighborValue = 0;
				FVector3f MaxNeighborIndex;

#define	CheckNeighbor(bIsNegative, DX, DY, DZ) \
				{ \
					const int32 NeighborIndex = DensitiesIndex + DX + DensitiesSize.X * DY + DensitiesSize.X * DensitiesSize.Y * DZ; \
					checkVoxelSlow(NeighborIndex == FVoxelUtilities::Get3DIndex<int32>(DensitiesSize, FIntVector(X, Y, Z) + FIntVector(DX, DY, DZ) + 1)); \
					const float NeighborValue = Densities[NeighborIndex]; \
					\
					if (bIsNegative ? (NeighborValue > MaxNeighborValue) : (NeighborValue < MaxNeighborValue)) \
					{ \
						MaxNeighborValue = NeighborValue; \
						MaxNeighborIndex = FVector3f(DX, DY, DZ); \
					} \
				}

				if (Value <= 0)
				{
					CheckNeighbor(true, -1, 0, 0);
					CheckNeighbor(true, +1, 0, 0);
					CheckNeighbor(true, 0, -1, 0);
					CheckNeighbor(true, 0, +1, 0);
					CheckNeighbor(true, 0, 0, -1);
					CheckNeighbor(true, 0, 0, +1);

					OutDistances[OutIndex] = -1;
				}
				else
				{
					CheckNeighbor(false, -1, 0, 0);
					CheckNeighbor(false, +1, 0, 0);
					CheckNeighbor(false, 0, -1, 0);
					CheckNeighbor(false, 0, +1, 0);
					CheckNeighbor(false, 0, 0, -1);
					CheckNeighbor(false, 0, 0, +1);

					OutDistances[OutIndex] = 1;
				}
#undef CheckNeighbor

				if (MaxNeighborValue == 0.f)
				{
					OutSurfacePositions[OutIndex] = MakeInvalidSurfacePosition();
				}
				else
				{
					const float Alpha = Value / (Value - MaxNeighborValue);
					OutSurfacePositions[OutIndex] = FVector3f(X, Y, Z) + MaxNeighborIndex * Alpha;

					if (ComputeOutBounds)
					{
						*OutBounds += FIntVector(X, Y, Z);
					}
				}
			}
		}
	};

	if (bParallel)
	{
		// Make the threads work on adjacent Z to have a hot cache
		const int32 NumThreads = FMath::Min(FTaskGraphInterface::Get().GetNumWorkerThreads(), Size.Z);
		ParallelFor(NumThreads, [&](int32 ThreadIndex)
		{
			VOXEL_SCOPE_COUNTER("Loop");
			const int32 LayersPerThread = FVoxelUtilities::DivideCeil_Positive(Size.Z, NumThreads);
			const int32 StartIndex = ThreadIndex * LayersPerThread;
			const int32 EndIndex = FMath::Min((ThreadIndex + 1) * LayersPerThread, Size.Z);

			for (int32 Z = StartIndex; Z < EndIndex; Z++)
			{
				if (OutBounds)
				{
					Body(FVoxelTrueType(), Z);
				}
				else
				{
					Body(FVoxelFalseType(), Z);
				}
			}
		});
	}
	else
	{
		for (int32 Z = 0; Z < Size.Z; Z++)
		{
			if (OutBounds)
			{
				Body(FVoxelTrueType(), Z);
			}
			else
			{
				Body(FVoxelFalseType(), Z);
			}
		}
	}
}

void FVoxelDistanceFieldUtilities::GetSurfacePositionsFromDensities(
	const FIntVector& Size,
	const TArray<float>& Densities,
	TArray<float>& OutDistances,
	TArray<FVector3f>& OutSurfacePositions,
	bool bParallel,
	FVoxelOptionalIntBox* OutBounds)
{
	VOXEL_FUNCTION_COUNTER();

	const int64 Num = Size.X * Size.Y * Size.Z;
	if (!ensure(Num < MAX_int32))
	{
		return;
	}

	OutDistances.Empty(Num);
	OutDistances.SetNumUninitialized(Num);

	OutSurfacePositions.Empty(Num);
	OutSurfacePositions.SetNumUninitialized(Num);

	GetSurfacePositionsFromDensities(
		Size,
		Densities,
		TArrayView<float>(OutDistances),
		TArrayView<FVector3f>(OutSurfacePositions),
		bParallel,
		OutBounds);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template<int32 Value>
struct FInt
{
 	constexpr operator int32() const { return Value; }
};

void FVoxelDistanceFieldUtilities::JumpFloodStep_CPU(
	const FIntVector& Size,
	const FVoxelIntBox& Bounds,
	TVoxelArrayView<const FVector3f> InData,
	TVoxelArrayView<FVector3f> OutData,
	int32 Step,
	bool bParallel)
{
	VOXEL_FUNCTION_COUNTER();

	check(InData.Num() == OutData.Num());
	check(InData.Num() == Size.X * Size.Y * Size.Z);

	const auto DoWork = [&](auto StaticStep, int32 Z)
	{
		for (int32 Y = Bounds.Min.Y; Y < Bounds.Max.Y; Y++)
		{
			int32 Index = FVoxelUtilities::Get3DIndex<int32>(Size, FIntVector(Bounds.Min.X, Y, Z));
			for (int32 X = Bounds.Min.X; X < Bounds.Max.X; X++)
			{
				ON_SCOPE_EXIT
				{
					Index++;
				};

				float BestDistance = MAX_flt;
				FVector3f BestSurfacePosition = MakeInvalidSurfacePosition();

#define CheckNeighbor_DX(DX, DY, DZ) \
				{ \
					const int32 NeighborIndex = Index + DX * StaticStep + DY * StaticStep * Size.X + DZ * StaticStep * Size.X * Size.Y; \
					checkVoxelSlow(NeighborIndex == FVoxelUtilities::Get3DIndex<int32>(Size, FIntVector(X, Y, Z) + FIntVector(DX * StaticStep, DY * StaticStep, DZ * StaticStep))); \
					const FVector3f NeighborSurfacePosition = InData[NeighborIndex]; \
					const float Distance = FVector3f::DistSquared(NeighborSurfacePosition, FVector3f(X, Y, Z)); \
					if (Distance < BestDistance) \
					{ \
						BestDistance = Distance; \
						BestSurfacePosition = NeighborSurfacePosition; \
					} \
				}

#define CheckNeighbor_DY(DY, DZ) \
				if (X - StaticStep >= Bounds.Min.X) { CheckNeighbor_DX(-1, DY, DZ); } \
				CheckNeighbor_DX(+0, DY, DZ); \
				if (X + StaticStep < Bounds.Max.X) { CheckNeighbor_DX(+1, DY, DZ); }

#define CheckNeighbor_DZ(DZ) \
				if (Y - StaticStep >= Bounds.Min.Y) { CheckNeighbor_DY(-1, DZ); } \
				CheckNeighbor_DY(+0, DZ); \
				if (Y + StaticStep < Bounds.Max.Y) { CheckNeighbor_DY(+1, DZ); }

#define CheckNeighbor() \
				if (Z - StaticStep >= Bounds.Min.Z) { CheckNeighbor_DZ(-1); } \
				CheckNeighbor_DZ(+0); \
				if (Z + StaticStep < Bounds.Max.Z) { CheckNeighbor_DZ(+1); }

				CheckNeighbor();

				OutData[Index] = BestSurfacePosition;

#undef CheckNeighbor
#undef CheckNeighbor_DX
#undef CheckNeighbor_DY
#undef CheckNeighbor_DZ
			}
		}
	};

	const auto DispatchDoWork = [&](int32 Z)
	{
		switch (Step)
		{
#define CASE(X) case X: DoWork(FInt<X>(), Z); break;
			CASE(1);
			CASE(2);
			CASE(4);
			CASE(8);
			CASE(16);
			CASE(32);
			CASE(64);
			CASE(128);
			CASE(256);
#undef CASE
		default: DoWork(Step, Z); break;
		}
	};

	if (bParallel)
	{
		// Make the threads work on adjacent Z to have a hot cache
		const int32 BoundsSizeZ = Bounds.Size().Z;
		const int32 NumThreads = FMath::Min(FTaskGraphInterface::Get().GetNumWorkerThreads(), BoundsSizeZ);
		const int32 LayersPerThread = FVoxelUtilities::DivideCeil_Positive(BoundsSizeZ, NumThreads);

		ParallelFor(NumThreads, [&](int32 ThreadIndex)
		{
			VOXEL_SCOPE_COUNTER("Loop");
			const int32 StartIndex = ThreadIndex * LayersPerThread;
			const int32 EndIndex = FMath::Min((ThreadIndex + 1) * LayersPerThread, BoundsSizeZ);

			for (int32 Index = StartIndex; Index < EndIndex; Index++)
			{
				DispatchDoWork(Bounds.Min.Z + Index);
			}
		});
	}
	else
	{
		for (int32 Z = Bounds.Min.Z; Z < Bounds.Max.Z; Z++)
		{
			DispatchDoWork(Z);
		}
	}
}