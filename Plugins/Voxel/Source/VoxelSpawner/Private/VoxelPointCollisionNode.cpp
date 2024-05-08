// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelPointCollisionNode.h"
#include "VoxelPointCollisionLargeChunk.h"
#include "VoxelPointCollisionSmallChunk.h"
#include "VoxelInvoker.h"

TVoxelUniquePtr<FVoxelExecNodeRuntime> FVoxelPointCollisionExecNode::CreateExecRuntime(const TSharedRef<const FVoxelExecNode>& SharedThis) const
{
	return MakeVoxelUnique<FVoxelPointCollisionExecNodeRuntime>(SharedThis);
}

void FVoxelPointCollisionExecNodeRuntime::Create()
{
	VOXEL_FUNCTION_COUNTER();
	ensure(SmallChunks_RequiresLock.Num() == 0);
	ensure(LargeChunks_RequiresLock.Num() == 0);

	const TSharedRef<const FVoxelChunkedPointSet> ChunkedPoints = GetConstantPin(Node.ChunkedPointsPin);
	const TSharedRef<const FBodyInstance> BodyInstance = GetConstantPin(Node.BodyInstancePin);

	if (!ChunkedPoints->IsValid())
	{
		return;
	}

	const int32 LargeChunkSize = ChunkedPoints->GetChunkSize();
	const int32 SmallChunkSize = GetConstantPin(Node.ChunkSizePin);

	InvokerView = FVoxelInvokerManager::Get(GetWorld())->MakeView(
			GetConstantPin(Node.InvokerChannelPin),
			SmallChunkSize,
			FMath::CeilToInt(GetConstantPin(Node.DistanceOffsetPin)),
			GetLocalToWorld());

	InvokerView->Bind_Async(
		MakeWeakPtrDelegate(this, [=](const TVoxelAddOnlySet<FIntVector>& ChunksToAdd)
		{
			VOXEL_SCOPE_COUNTER("OnAddChunk");
			VOXEL_SCOPE_LOCK(CriticalSection);

			for (const FIntVector& ChunkKey : ChunksToAdd)
			{
				const FIntVector SmallChunkMin = ChunkKey * SmallChunkSize;
				const FIntVector LargeChunkMin = FVoxelUtilities::DivideFloor(SmallChunkMin, LargeChunkSize) * LargeChunkSize;

				FVoxelPointChunkRef ChunkRef;
				ChunkRef.ChunkProviderRef = ChunkedPoints->GetChunkProviderRef();
				ChunkRef.ChunkMin = LargeChunkMin;
				ChunkRef.ChunkSize = LargeChunkSize;

				TWeakPtr<FVoxelPointCollisionLargeChunk>& WeakLargeChunk = LargeChunks_RequiresLock.FindOrAdd(LargeChunkMin);
				TSharedPtr<FVoxelPointCollisionLargeChunk> LargeChunk = WeakLargeChunk.Pin();
				if (!LargeChunk)
				{
					LargeChunk = MakeVoxelShared<FVoxelPointCollisionLargeChunk>(
						GetNodeRef(),
						ChunkRef,
						ChunkedPoints->GetPointsValue(LargeChunkMin, GetConstantPin(Node.PriorityOffsetPin)));
					LargeChunk->Initialize();

					WeakLargeChunk = LargeChunk;
				}

				const TSharedRef<FVoxelPointCollisionSmallChunk> Chunk = MakeVoxelShared<FVoxelPointCollisionSmallChunk>(
					GetNodeRef(),
					ChunkRef,
					FVoxelBox(SmallChunkMin, SmallChunkMin + SmallChunkSize),
					GetRuntimeInfo(),
					BodyInstance,
					LargeChunk.ToSharedRef());

				Chunk->Initialize();

				ensure(!SmallChunks_RequiresLock.Contains(SmallChunkMin));
				SmallChunks_RequiresLock.Add(SmallChunkMin, Chunk);
			}
		}),
		MakeWeakPtrDelegate(this, [=](const TVoxelAddOnlySet<FIntVector>& ChunksToRemove)
		{
			VOXEL_SCOPE_COUNTER("OnRemoveChunk");

			TVoxelArray<TSharedPtr<FVoxelPointCollisionSmallChunk>> ChunksToDestroy;
			{
				VOXEL_SCOPE_LOCK(CriticalSection);
				for (const FIntVector& ChunkKey : ChunksToRemove)
				{
					const FIntVector ChunkMin = ChunkKey * SmallChunkSize;

					TSharedPtr<FVoxelPointCollisionSmallChunk> Chunk;
					if (!ensure(SmallChunks_RequiresLock.RemoveAndCopyValue(ChunkMin, Chunk)))
					{
						continue;
					}

					ChunksToDestroy.Add(Chunk);
				}
			}

			FVoxelUtilities::RunOnGameThread(MakeWeakPtrLambda(this, [this, ChunksToDestroy = MoveTemp(ChunksToDestroy)]
			{
				VOXEL_SCOPE_COUNTER("OnRemoveChunk_GameThread");

				const TSharedPtr<FVoxelRuntime> Runtime = GetRuntime();
				if (!ensure(Runtime))
				{
					return;
				}

				for (const TSharedPtr<FVoxelPointCollisionSmallChunk>& Chunk : ChunksToDestroy)
				{
					Chunk->Destroy(*Runtime);
				}
			}));
		}));
}

void FVoxelPointCollisionExecNodeRuntime::Destroy()
{
	VOXEL_FUNCTION_COUNTER();

	InvokerView = {};

	const TSharedPtr<FVoxelRuntime> Runtime = GetRuntime();
	if (!Runtime)
	{
		return;
	}

	VOXEL_SCOPE_LOCK(CriticalSection);
	for (const auto& It : SmallChunks_RequiresLock)
	{
		It.Value->Destroy(*Runtime);
	}
	SmallChunks_RequiresLock.Empty();
}