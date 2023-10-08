// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelInvokerChunkSpawner.h"
#include "VoxelInvoker.h"
#include "VoxelRuntime.h"

void FVoxelInvokerChunkSpawner::Tick(FVoxelRuntime& Runtime)
{
	VOXEL_FUNCTION_COUNTER();
	check(IsInGameThread());

	const int32 FullChunkSize = FMath::CeilToInt(GetVoxelSize() * ChunkSize);

	if (InvokerView_GameThread &&
		InvokerView_GameThread->ChunkSize == FullChunkSize)
	{
		return;
	}

	VOXEL_SCOPE_LOCK(CriticalSection);

	ChunkRefs_RequiresLock.Empty();

	InvokerView_GameThread = FVoxelInvokerManager::Get(Runtime.GetWorld())->MakeView(
		InvokerChannel,
		FullChunkSize,
		0,
		Runtime.GetLocalToWorld());

	InvokerViewBindRef_GameThread = MakeSharedVoid();

	InvokerView_GameThread->Bind_Async(
		MakeWeakPtrDelegate(InvokerViewBindRef_GameThread, MakeWeakPtrLambda(this, [=](const TVoxelAddOnlySet<FIntVector>& ChunksToAdd)
		{
			VOXEL_SCOPE_COUNTER("OnAddChunk");
			VOXEL_SCOPE_LOCK(CriticalSection);

			for (const FIntVector& Chunk : ChunksToAdd)
			{
				const TSharedRef<FVoxelChunkRef> ChunkRef = CreateChunk(
					LOD,
					ChunkSize,
					FVoxelBox(FVector(Chunk) * FullChunkSize, FVector(Chunk + 1) * FullChunkSize));
				ChunkRef->Compute();

				ensure(!ChunkRefs_RequiresLock.Contains(Chunk));
				ChunkRefs_RequiresLock.Add(Chunk) = ChunkRef;
			}
		})),
		MakeWeakPtrDelegate(InvokerViewBindRef_GameThread, MakeWeakPtrLambda(this, [this](const TVoxelAddOnlySet<FIntVector>& ChunksToRemove)
		{
			VOXEL_SCOPE_COUNTER("OnRemoveChunk");
			VOXEL_SCOPE_LOCK(CriticalSection);

			for (const FIntVector& Chunk : ChunksToRemove)
			{
				ensure(ChunkRefs_RequiresLock.Remove(Chunk));
			}
		})));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_MakeInvokerChunkSpawner, Spawner)
{
	const TValue<int32> LOD = Get(LODPin, Query);
	const TValue<float> WorldSize = Get(WorldSizePin, Query);
	const TValue<int32> ChunkSize = Get(ChunkSizePin, Query);
	const TValue<FName> InvokerChannel = Get(InvokerChannelPin, Query);

	return VOXEL_ON_COMPLETE(LOD, WorldSize, ChunkSize, InvokerChannel)
	{
		const TSharedRef<FVoxelInvokerChunkSpawner> Spawner = MakeVoxelShared<FVoxelInvokerChunkSpawner>();
		Spawner->NodeRef = GetNodeRef();
		Spawner->LOD = FMath::Clamp(LOD, 0, 30);
		Spawner->WorldSize = WorldSize;
		Spawner->ChunkSize = FMath::Clamp(FMath::CeilToInt(ChunkSize / 2.f) * 2, 4, 128);
		Spawner->InvokerChannel = InvokerChannel;
		return Spawner;
	};
}