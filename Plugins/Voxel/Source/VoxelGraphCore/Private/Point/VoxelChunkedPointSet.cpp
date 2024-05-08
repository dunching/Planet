// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Point/VoxelChunkedPointSet.h"
#include "VoxelDebugNode.h"
#include "VoxelTaskGroup.h"
#include "VoxelDependency.h"
#include "Buffer/VoxelStaticMeshBuffer.h"

FVoxelChunkedPointSet::FVoxelChunkedPointSet(
	const int32 ChunkSize,
	const FVoxelPointChunkProviderRef& ChunkProviderRef,
	const TSharedRef<FVoxelQueryContext>& Context,
	const TSharedRef<const TVoxelComputeValue<FVoxelPointSet>>& ComputePoints)
	: ChunkSize(FMath::Max(10, ChunkSize))
	, ChunkProviderRef(ChunkProviderRef)
	, Context(Context)
	, ComputePoints(ComputePoints)
{
	ensure(ChunkSize > 0);
}

TVoxelFutureValue<FVoxelPointSet> FVoxelChunkedPointSet::GetPoints(
	FVoxelDependencyTracker& DependencyTracker,
	const FIntVector& ChunkMin) const
{
	VOXEL_FUNCTION_COUNTER();
	ensure(IsValid());
	ensure(ChunkMin % ChunkSize == 0);

	const TSharedRef<FVoxelDebugQueryParameter> DebugParameter = MakeVoxelShared<FVoxelDebugQueryParameter>();

	FVoxelPointChunkRef ChunkRef;
	ChunkRef.ChunkProviderRef = ChunkProviderRef;
	ChunkRef.ChunkMin = ChunkMin;
	ChunkRef.ChunkSize = ChunkSize;

	const TSharedRef<FVoxelQueryParameters> Parameters = MakeVoxelShared<FVoxelQueryParameters>();
	Parameters->Add(DebugParameter);
	Parameters->Add<FVoxelPointChunkRefQueryParameter>().ChunkRef = ChunkRef;

	const FVoxelQuery Query = FVoxelQuery::Make(
		Context.ToSharedRef(),
		Parameters,
		DependencyTracker.AsShared());

	const TVoxelFutureValue<FVoxelPointSet> Points = (*ComputePoints)(Query);

	return
		MakeVoxelTask()
		.Dependency(Points)
		.Execute<FVoxelPointSet>([=]
		{
			VOXEL_SCOPE_LOCK(DebugParameter->CriticalSection);

			if (DebugParameter->Entries_RequiresLock.Num() == 0)
			{
				return Points;
			}

			TVoxelArray<FVoxelGraphNodeRef> NodeRefs;
			TVoxelArray<TSharedPtr<const TVoxelComputeValue<FVoxelPointSet>>> Computes;
			for (const auto& It : DebugParameter->Entries_RequiresLock)
			{
				if (!It.Value.Type.Is<FVoxelPointSet>())
				{
					continue;
				}

				NodeRefs.Add(It.Value.NodeRef);
				Computes.Add(ReinterpretCastSharedPtr<const TVoxelComputeValue<FVoxelPointSet>>(It.Value.Compute));
			}

			if (NodeRefs.Num() == 0)
			{
				return Points;
			}

			if (NodeRefs.Num() > 1)
			{
				VOXEL_MESSAGE(Error, "Multiple nodes being debugged: {0}", NodeRefs);
			}

			const TVoxelFutureValue<FVoxelPointSet> PointsOverride = (*Computes[0])(Query);
			return
				MakeVoxelTask()
				.Thread(EVoxelTaskThread::GameThread)
				.Dependency(PointsOverride)
				.Execute<FVoxelPointSet>([=]
				{
					FVoxelPointSet FinalPoints = PointsOverride.Get_CheckCompleted();
					if (!FinalPoints.Find(FVoxelPointAttributes::Mesh))
					{
						static FVoxelStaticMesh DefaultMesh;
						if (!DefaultMesh.StaticMesh.IsValid())
						{
							DefaultMesh = FVoxelStaticMesh::Make(LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/EditorMeshes/EditorCube.EditorCube")));
						}

						FinalPoints.Add(FVoxelPointAttributes::Mesh, MakeSharedCopy(FVoxelStaticMeshBuffer::Make(DefaultMesh)));
					}
					return FinalPoints;
				});
		});
}

TVoxelDynamicValue<FVoxelPointSet> FVoxelChunkedPointSet::GetPointsValue(
	const FIntVector& ChunkMin,
	const double PriorityOffset) const
{
	ensure(ChunkMin % ChunkSize == 0);

	TVoxelDynamicValueFactory<FVoxelPointSet> Factory(
		STATIC_FNAME("ChunkedPointSet"),
		MakeWeakPtrLambda(this, [=](const FVoxelQuery& Query)
		{
			return GetPoints(Query.GetDependencyTracker(), ChunkMin);
		}));

	return Factory
		.Priority(FVoxelTaskPriority::MakeBounds(
			FVoxelBox(ChunkMin, ChunkMin + ChunkSize),
			PriorityOffset,
			GetWorld(), GetLocalToWorld()))
		.Compute(Context.ToSharedRef());
}

void FVoxelChunkedPointSet::GetPointsInBounds(
	const FVoxelBox& Bounds,
	TFunction<void(const TSharedRef<const FVoxelPointSet>&)> Callback,
	TFunction<bool(const FVoxelIntBox& ChunkBounds)> ShouldComputeChunk) const
{
	VOXEL_FUNCTION_COUNTER();

	FVoxelTaskGroup::StartAsyncTask<FVoxelPointSet>(
		STATIC_FNAME("FVoxelChunkedPointSet::GetPointsInBounds"),
		Context.ToSharedRef(),
		[This = AsShared(), ChunkSize = ChunkSize, Bounds = Bounds, ShouldComputeChunk = MoveTemp(ShouldComputeChunk)]() -> TVoxelFutureValue<FVoxelPointSet>
		{
			const FIntVector Min = FVoxelUtilities::FloorToInt(Bounds.Min / ChunkSize);
			const FIntVector Max = FVoxelUtilities::CeilToInt(Bounds.Max / ChunkSize);

			const double Count = FVoxelIntBox(Min, Max).Count_LargeBox();
			if (!ensure(Count < 1024 * 1024))
			{
				return {};
			}

			const TSharedRef<FVoxelDependencyTracker> DependencyTracker = FVoxelDependencyTracker::Create("GetPointsInBounds");

			TVoxelArray<TVoxelFutureValue<FVoxelPointSet>> FuturePointSets;
			for (int32 X = Min.X; X < Max.X; X++)
			{
				for (int32 Y = Min.Y; Y < Max.Y; Y++)
				{
					for (int32 Z = Min.Z; Z < Max.Z; Z++)
					{
						const FIntVector ChunkMin = FIntVector(X, Y, Z) * ChunkSize;
						if (ShouldComputeChunk &&
							!ShouldComputeChunk(FVoxelIntBox(ChunkMin, ChunkMin + ChunkSize)))
						{
							continue;
						}

						const TVoxelFutureValue<FVoxelPointSet> FuturePointSet = This->GetPoints(*DependencyTracker, ChunkMin);
						if (!FuturePointSet.IsValid())
						{
							continue;
						}
						FuturePointSets.Add(FuturePointSet);
					}
				}
			}

			return
				MakeVoxelTask(STATIC_FNAME("GetPointsInBounds_Merge"))
				.Dependencies(FuturePointSets)
				.Execute<FVoxelPointSet>([=]
				{
					TVoxelArray<TSharedRef<const FVoxelPointSet>> PointSets;
					for (const TVoxelFutureValue<FVoxelPointSet>& FuturePointSet : FuturePointSets)
					{
						const FVoxelPointSet& PointSet = FuturePointSet.Get_CheckCompleted();
						if (PointSet.Num() == 0)
						{
							continue;
						}

						const TSharedPtr<const FVoxelBuffer> PositionsPtr = PointSet.Find(FVoxelPointAttributes::Position);
						if (!ensure(PositionsPtr) ||
							!ensure(PositionsPtr->IsA<FVoxelVectorBuffer>()))
						{
							continue;
						}

						const FVoxelVectorBuffer Positions = CastChecked<FVoxelVectorBuffer>(*PositionsPtr);

						FVoxelInt32BufferStorage Indices;
						for (int32 Index = 0; Index < PointSet.Num(); Index++)
						{
							if (Bounds.Contains(Positions[Index]))
							{
								Indices.Add(Index);
							}
						}
						PointSets.Add(PointSet.Gather(FVoxelInt32Buffer::Make(Indices)));
					}
					return FVoxelPointSet::Merge(PointSets);
				});
		},
		[Callback = MoveTemp(Callback)](const TSharedRef<const FVoxelPointSet>& PointSet)
		{
			Callback(PointSet);
		});
}