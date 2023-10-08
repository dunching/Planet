// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelQuery.h"
#include "VoxelChunkSpawner.generated.h"

class FVoxelRuntime;

DECLARE_UNIQUE_VOXEL_ID_EXPORT(VOXELGRAPHCORE_API, FVoxelChunkId);

extern VOXELGRAPHCORE_API int32 GVoxelChunkSpawnerMaxChunks;
extern VOXELGRAPHCORE_API float GVoxelChunkSpawnerCameraRefreshThreshold;

enum class EVoxelChunkAction
{
	Compute,
	SetTransitionMask,
	BeginDestroy,
	Destroy
};
struct FVoxelChunkAction
{
	EVoxelChunkAction Action = {};
	FVoxelChunkId ChunkId;

	uint8 TransitionMask = 0;
	TSharedPtr<const TVoxelUniqueFunction<void()>> OnComputeComplete;

	FVoxelChunkAction() = default;
	FVoxelChunkAction(const EVoxelChunkAction Action, const FVoxelChunkId ChunkId)
		: Action(Action)
		, ChunkId(ChunkId)
	{
	}
};

struct FVoxelChunkActionQueue
{
	TQueue<FVoxelChunkAction, EQueueMode::Mpsc> AsyncQueue;
	TQueue<FVoxelChunkAction, EQueueMode::Mpsc> GameQueue;

	void Enqueue(const FVoxelChunkAction& Action)
	{
		switch (Action.Action)
		{
		default: ensure(false);
		case EVoxelChunkAction::Compute:
		case EVoxelChunkAction::BeginDestroy:
		{
			AsyncQueue.Enqueue(Action);
		}
		break;
		case EVoxelChunkAction::SetTransitionMask:
		case EVoxelChunkAction::Destroy:
		{
			GameQueue.Enqueue(Action);
		}
		break;
		}
	}
};

struct VOXELGRAPHCORE_API FVoxelChunkRef
{
	const FVoxelChunkId ChunkId;
	const TSharedRef<FVoxelChunkActionQueue> Queue;

	FVoxelChunkRef(const FVoxelChunkId ChunkId, const TSharedRef<FVoxelChunkActionQueue>& Queue)
		: ChunkId(ChunkId)
		, Queue(Queue)
	{
	}
	~FVoxelChunkRef()
	{
		Queue->Enqueue(FVoxelChunkAction(EVoxelChunkAction::Destroy, ChunkId));
	}

	void BeginDestroy() const
	{
		Queue->Enqueue(FVoxelChunkAction(EVoxelChunkAction::BeginDestroy, ChunkId));
	}
	void Compute(TVoxelUniqueFunction<void()>&& OnComputeComplete = nullptr) const
	{
		FVoxelChunkAction Action(EVoxelChunkAction::Compute, ChunkId);
		if (OnComputeComplete)
		{
			Action.OnComputeComplete = MakeSharedCopy(MoveTemp(OnComputeComplete));
		}
		Queue->Enqueue(Action);
	}
	void SetTransitionMask(uint8 TransitionMask) const
	{
		FVoxelChunkAction Action(EVoxelChunkAction::SetTransitionMask, ChunkId);
		Action.TransitionMask = TransitionMask;
		Queue->Enqueue(Action);
	}
};

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelChunkSpawner
	: public FVoxelVirtualStruct
	, public TSharedFromThis<FVoxelChunkSpawner>
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

public:
	FORCEINLINE float GetVoxelSize() const
	{
		return PrivateVoxelSize;
	}

	TSharedRef<FVoxelChunkRef> CreateChunk(
		int32 LOD,
		int32 ChunkSize,
		const FVoxelBox& Bounds) const;

	virtual void Initialize(FVoxelRuntime& Runtime) {}
	virtual void Tick(FVoxelRuntime& Runtime) {}

private:
	using FCreateChunk = TFunction<TSharedPtr<FVoxelChunkRef>(
		int32 LOD,
		int32 ChunkSize,
		const FVoxelBox& Bounds)>;

	float PrivateVoxelSize = 0.f;
	FCreateChunk PrivateCreateChunkLambda;

	friend class FVoxelMarchingCubeExecNodeRuntime;
};