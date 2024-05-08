// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Point/VoxelPointNodes.h"

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_GetPoints, Out)
{
	const TValue<FVoxelChunkedPointSet> ChunkedPoints = Get(InPin, Query);

	return VOXEL_ON_COMPLETE(ChunkedPoints)
	{
		if (!ChunkedPoints->IsValid())
		{
			return {};
		}

		FindVoxelQueryParameter(FVoxelPointChunkRefQueryParameter, PointChunkRefQueryParameter);
		const FVoxelPointChunkRef& ChunkRef = PointChunkRefQueryParameter->ChunkRef;
		const int32 ChunkSize = ChunkedPoints->GetChunkSize();

		const FIntVector Min = FVoxelUtilities::DivideFloor(ChunkRef.ChunkMin, ChunkSize) * ChunkSize;
		const FIntVector Max = FVoxelUtilities::DivideCeil(ChunkRef.ChunkMin + ChunkRef.ChunkSize, ChunkSize) * ChunkSize;

		TVoxelArray<TValue<FVoxelPointSet>> AllPoints;
		for (int32 X = Min.X; X < Max.X; X += ChunkSize)
		{
			for (int32 Y = Min.Y; Y < Max.Y; Y += ChunkSize)
			{
				for (int32 Z = Min.Z; Z < Max.Z; Z += ChunkSize)
				{
					AllPoints.Add(ChunkedPoints->GetPoints(
						Query.GetDependencyTracker(),
						FIntVector(X, Y, Z)));
				}
			}
		}

		const FVoxelBox Bounds = ChunkRef.GetBounds();

		return VOXEL_ON_COMPLETE(AllPoints, Bounds)
		{
			TVoxelArray<TSharedRef<const FVoxelPointSet>> AllPointsFiltered = AllPoints;
			for (TSharedRef<const FVoxelPointSet>& Points : AllPointsFiltered)
			{
				FindVoxelPointSetAttribute(*Points, FVoxelPointAttributes::Position, FVoxelVectorBuffer, Positions);

				FVoxelInt32BufferStorage Indices;
				for (int32 Index = 0; Index < Points->Num(); Index++)
				{
					if (Bounds.Contains(Positions[Index]))
					{
						Indices.Add(Index);
					}
				}
				Points = Points->Gather(FVoxelInt32Buffer::Make(Indices));
			}
			return FVoxelPointSet::Merge(AllPointsFiltered);
		};
	};
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_FilterPoints, Out)
{
	const TValue<FVoxelPointSet> Points = Get(InPin, Query);

	return VOXEL_ON_COMPLETE(Points)
	{
		if (Points->Num() == 0)
		{
			return {};
		}

		const TValue<FVoxelBoolBuffer> KeepPoint = Get(KeepPointPin, Points->MakeQuery(Query));

		return VOXEL_ON_COMPLETE(Points, KeepPoint)
		{
			if (!Points->CheckNum(this, KeepPoint.Num()))
			{
				return Points;
			}

			FindVoxelPointSetAttribute(*Points, FVoxelPointAttributes::Id, FVoxelPointIdBuffer, IdBuffer);
			FVoxelNodeStatScope StatScope(*this, Points->Num());

			FVoxelInt32BufferStorage Indices;
			for (int32 Index = 0; Index < Points->Num(); Index++)
			{
				if (KeepPoint[Index])
				{
					Indices.Add(Index);
				}
			}
			return Points->Gather(FVoxelInt32Buffer::Make(Indices));
		};
	};
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_DensityFilter, Out)
{
	const TValue<FVoxelPointSet> Points = Get(InPin, Query);

	return VOXEL_ON_COMPLETE(Points)
	{
		if (Points->Num() == 0)
		{
			return {};
		}

		const TValue<FVoxelFloatBuffer> Densities = Get(DensityPin, Points->MakeQuery(Query));
		const TValue<FVoxelSeed> Seed = Get(SeedPin, Points->MakeQuery(Query));

		return VOXEL_ON_COMPLETE(Points, Densities, Seed)
		{
			if (!Points->CheckNum(this, Densities.Num()))
			{
				return Points;
			}

			FindVoxelPointSetAttribute(*Points, FVoxelPointAttributes::Id, FVoxelPointIdBuffer, IdBuffer);
			FVoxelNodeStatScope StatScope(*this, Points->Num());

			const FVoxelPointRandom Random(Seed, STATIC_HASH("DensityFilter"));

			FVoxelInt32BufferStorage Indices;
			for (int32 Index = 0; Index < Points->Num(); Index++)
			{
				const FVoxelPointId Id = IdBuffer[Index];
				const float Fraction = Random.GetFraction(Id);
				if (Fraction >= Densities[Index])
				{
					continue;
				}

				Indices.Add(Index);
			}
			return Points->Gather(FVoxelInt32Buffer::Make(Indices));
		};
	};
}