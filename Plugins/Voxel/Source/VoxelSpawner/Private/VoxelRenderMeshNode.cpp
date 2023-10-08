// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelRenderMeshNode.h"
#include "VoxelRenderMeshChunk.h"
#include "VoxelInvoker.h"

TVoxelUniquePtr<FVoxelExecNodeRuntime> FVoxelRenderMeshExecNode::CreateExecRuntime(const TSharedRef<const FVoxelExecNode>& SharedThis) const
{
	return MakeVoxelUnique<FVoxelRenderMeshExecNodeRuntime>(SharedThis);
}

void FVoxelRenderMeshExecNodeRuntime::Create()
{
	VOXEL_FUNCTION_COUNTER();
	ensure(Chunks_RequiresLock.Num() == 0);

	const TSharedRef<const FVoxelChunkedPointSet> ChunkedPoints = GetConstantPin(Node.ChunkedPointsPin);
	const float RenderDistance = GetConstantPin(Node.RenderDistancePin);
	const float MinRenderDistance = GetConstantPin(Node.MinRenderDistancePin);
	const float FadeDistance = GetConstantPin(Node.FadeDistancePin);
	const TSharedRef<const FVoxelFoliageSettings> FoliageSettings = GetConstantPin(Node.FoliageSettingsPin);

	if (!ChunkedPoints->IsValid())
	{
		return;
	}

	const int32 ChunkSize = ChunkedPoints->GetChunkSize();

	InvokerView = FVoxelInvokerManager::Get(GetWorld())->MakeView(
		STATIC_FNAME("Camera"),
		ChunkSize,
		FMath::CeilToInt(GetConstantPin(Node.RenderDistancePin)),
		GetLocalToWorld());

	InvokerView->Bind_Async(
		MakeWeakPtrDelegate(this, [=](const TVoxelAddOnlySet<FIntVector>& ChunksToAdd)
		{
			VOXEL_SCOPE_COUNTER("OnAddChunk");

			TVoxelArray<TWeakPtr<FVoxelRenderMeshChunk>> WeakChunks;
			WeakChunks.Reserve(ChunksToAdd.Num());
			{
				VOXEL_SCOPE_LOCK(CriticalSection);

				for (const FIntVector& ChunkKey : ChunksToAdd)
				{
					const FIntVector ChunkMin = ChunkKey * ChunkSize;

					FVoxelPointChunkRef ChunkRef;
					ChunkRef.ChunkProviderRef = ChunkedPoints->GetChunkProviderRef();
					ChunkRef.ChunkMin = ChunkMin;
					ChunkRef.ChunkSize = ChunkSize;

					const TSharedRef<FVoxelRenderMeshChunk> Chunk = MakeVoxelShared<FVoxelRenderMeshChunk>(
						GetNodeRef(),
						ChunkRef,
						GetRuntimeInfo(),
						ChunkedPoints->GetPointsValue(ChunkMin, GetConstantPin(Node.PriorityOffsetPin)),
						RenderDistance,
						MinRenderDistance,
						FadeDistance,
						FoliageSettings);

					Chunk->Initialize();

					WeakChunks.Add(Chunk);

					ensure(!Chunks_RequiresLock.Contains(ChunkMin));
					Chunks_RequiresLock.Add(ChunkMin, Chunk);
				}
			}
		}),
		MakeWeakPtrDelegate(this, [=](const TVoxelAddOnlySet<FIntVector>& ChunksToRemove)
		{
			VOXEL_SCOPE_COUNTER("OnRemoveChunk");

			TVoxelArray<TSharedPtr<FVoxelRenderMeshChunk>> ChunksToDestroy;
			{
				VOXEL_SCOPE_LOCK(CriticalSection);
				for (const FIntVector& ChunkKey : ChunksToRemove)
				{
					const FIntVector ChunkMin = ChunkKey * ChunkSize;

					TSharedPtr<FVoxelRenderMeshChunk> Chunk;
					if (!ensure(Chunks_RequiresLock.RemoveAndCopyValue(ChunkMin, Chunk)))
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

				for (const TSharedPtr<FVoxelRenderMeshChunk>& Chunk : ChunksToDestroy)
				{
					Chunk->Destroy(*Runtime);
				}
			}));
		}));
}

void FVoxelRenderMeshExecNodeRuntime::Destroy()
{
	VOXEL_FUNCTION_COUNTER();

	InvokerView = {};

	VOXEL_SCOPE_LOCK(CriticalSection);

	const TSharedPtr<FVoxelRuntime> Runtime = GetRuntime();
	if (!Runtime)
	{
		return;
	}

	for (const auto& It : Chunks_RequiresLock)
	{
		It.Value->Destroy(*Runtime);
	}
}

FVoxelOptionalBox FVoxelRenderMeshExecNodeRuntime::GetBounds() const
{
	VOXEL_FUNCTION_COUNTER();
	VOXEL_SCOPE_LOCK(CriticalSection);

	FVoxelOptionalBox Bounds;
	for (const auto& It : Chunks_RequiresLock)
	{
		Bounds += It.Value->GetBounds();
	}
	return Bounds;
}