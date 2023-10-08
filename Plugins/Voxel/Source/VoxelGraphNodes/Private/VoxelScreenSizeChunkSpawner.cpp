// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelScreenSizeChunkSpawner.h"
#include "VoxelRuntime.h"

DEFINE_UNIQUE_VOXEL_ID(FVoxelScreenSizeChunkId);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelScreenSizeChunkSpawner::Initialize(FVoxelRuntime& Runtime)
{
	VOXEL_FUNCTION_COUNTER();

	if (!ChunkScreenSizeValueFactory.IsValid())
	{
		// Default ScreenSizeChunkSpawner
		ensure(!QueryContext);
		ensure(!QueryParameters);
		return;
	}

	ensure(!ChunkScreenSizeValue.IsValid());
	ChunkScreenSizeValue = ChunkScreenSizeValueFactory.Compute(QueryContext.ToSharedRef(), QueryParameters.ToSharedRef());

	ChunkScreenSizeValue.OnChanged_GameThread(MakeWeakPtrLambda(this, [this](const float NewChunkScreenSize)
	{
		LastChunkScreenSize = NewChunkScreenSize;
		Refresh();
	}));;
}

void FVoxelScreenSizeChunkSpawner::Tick(FVoxelRuntime& Runtime)
{
	VOXEL_FUNCTION_COUNTER();

	if (bTaskInProgress)
	{
		return;
	}

	ON_SCOPE_EXIT
	{
		if (bUpdateQueued && LastViewOrigin.IsSet())
		{
			UpdateTree(LastViewOrigin.GetValue());
		}
	};

	FVector ViewOrigin = FVector::ZeroVector;
	if (!FVoxelGameUtilities::GetCameraView(Runtime.GetWorld_GameThread(), ViewOrigin))
	{
		return;
	}

	ViewOrigin = Runtime.GetLocalToWorld().Get_NoDependency().InverseTransformPosition(ViewOrigin);

	if (LastViewOrigin.IsSet() &&
		FVector::Distance(ViewOrigin, LastViewOrigin.GetValue()) < GVoxelChunkSpawnerCameraRefreshThreshold)
	{
		return;
	}

	LastViewOrigin = ViewOrigin;
	bUpdateQueued = true;
}

void FVoxelScreenSizeChunkSpawner::Refresh()
{
	bUpdateQueued = true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelScreenSizeChunkSpawner::UpdateTree(const FVector& ViewOrigin)
{
	VOXEL_FUNCTION_COUNTER();

	if (!LastChunkScreenSize.IsSet())
	{
		ensure(ChunkScreenSizeValue.IsValid());
		ensure(bUpdateQueued);
		return;
	}

	const int64 SizeInChunks = FMath::Max<int64>(FMath::CeilToInt64(WorldSize / (GetVoxelSize() * ChunkSize)), 2);
	const int32 OctreeDepth = FMath::Min<int32>(FMath::CeilLogTwo64(SizeInChunks), 29);

	ensure(bUpdateQueued);
	bUpdateQueued = false;

	ensure(!bTaskInProgress);
	bTaskInProgress = true;

	AsyncVoxelTask(MakeWeakPtrLambda(this, [this, ViewOrigin, OctreeDepth, OldTree = Octree]
	{
		const TSharedRef<FOctree> NewTree = MakeVoxelShared<FOctree>(
			OctreeDepth,
			ViewOrigin,
			*this);

		if (OldTree)
		{
			VOXEL_SCOPE_COUNTER("CopyFrom");
			NewTree->CopyFrom(*OldTree);
		}

		TMap<FChunkId, FChunkInfo> ChunkInfos;
		TSet<FChunkId> ChunksToAdd;
		TSet<FChunkId> ChunksToRemove;
		TSet<FChunkId> ChunksToUpdate;
		NewTree->Update(LastChunkScreenSize.GetValue(), ChunkInfos, ChunksToAdd, ChunksToRemove, ChunksToUpdate);

		{
			VOXEL_SCOPE_COUNTER("Sort");

			// Make sure that LOD 0 chunks are processed first
			ChunksToAdd.Sort([&](const FChunkId A, const FChunkId B)
			{
				return ChunkInfos[A].LOD < ChunkInfos[B].LOD;
			});
		}

		VOXEL_SCOPE_LOCK(CriticalSection);

		for (const FChunkId ChunkId : ChunksToUpdate)
		{
			FChunk& Chunk = *Chunks_RequiresLock.FindChecked(ChunkId);
			const FChunkInfo ChunkInfo = ChunkInfos[ChunkId];

			if (ensure(Chunk.ChunkRef))
			{
				Chunk.ChunkRef->SetTransitionMask(ChunkInfo.TransitionMask);
			}
		}

		// Set old chunks PreviousChunks to be themselves
		for (const FChunkId ChunkId : ChunksToRemove)
		{
			const TSharedPtr<FChunk> Chunk = Chunks_RequiresLock.FindRef(ChunkId);
			if (!ensure(Chunk))
			{
				continue;
			}

			if (ensure(Chunk->ChunkRef))
			{
				Chunk->ChunkRef->BeginDestroy();
			}

			const TSharedRef<FPreviousChunksLeaf> NewPreviousChunks = MakeVoxelShared<FPreviousChunksLeaf>();
			NewPreviousChunks->ChunkRef = Chunk->ChunkRef;
			Chunk->ChunkRef.Reset();

			if (Chunk->PreviousChunks)
			{
				NewPreviousChunks->Children.Add(MoveTemp(Chunk->PreviousChunks));
			}

			ensure(!Chunk->ChunkRef);
			ensure(!Chunk->PreviousChunks);

			Chunk->PreviousChunks = NewPreviousChunks;
		}

		for (const FChunkId ChunkId : ChunksToAdd)
		{
			const FChunkInfo ChunkInfo = ChunkInfos[ChunkId];

			ensure(!Chunks_RequiresLock.Contains(ChunkId));
			const TSharedPtr<FChunk> Chunk = Chunks_RequiresLock.Add(ChunkId, MakeVoxelShared<FChunk>());
			ensureVoxelSlow(ChunkInfo.ChunkBounds.Size().GetAbsMax() > 1);

			Chunk->ChunkRef = CreateChunk(ChunkInfo.LOD, ChunkSize, ChunkInfo.ChunkBounds);

			const TSharedRef<FPreviousChunks> PreviousChunks = MakeVoxelShared<FPreviousChunks>();
			if (OldTree)
			{
				OldTree->TraverseBounds(ChunkInfos[ChunkId].NodeBounds, [&](const FOctree::FNodeRef TreeNodeRef)
				{
					const FNode& Node = OldTree->GetNode(TreeNodeRef);
					if (!Node.bIsRendered ||
						!ChunksToRemove.Contains(Node.ChunkId))
					{
						return;
					}

					checkVoxelSlow(!OldTree->HasAnyChildren(TreeNodeRef));

					const TSharedPtr<FChunk> OldChunk = Chunks_RequiresLock.FindRef(Node.ChunkId);
					if (!ensure(OldChunk))
					{
						return;
					}

					ensure(!OldChunk->ChunkRef);
					ensure(OldChunk->PreviousChunks);
					PreviousChunks->Children.Add(OldChunk->PreviousChunks);
				});
			}

			if (PreviousChunks->Children.Num() > 0)
			{
				Chunk->PreviousChunks = PreviousChunks;
			}

			Chunk->ChunkRef->SetTransitionMask(ChunkInfo.TransitionMask);
			Chunk->ChunkRef->Compute(MakeWeakPtrLambda(this, [this, ChunkId]
			{
				VOXEL_SCOPE_LOCK(CriticalSection);
				if (!Chunks_RequiresLock.Contains(ChunkId))
				{
					return;
				}

				Chunks_RequiresLock[ChunkId]->PreviousChunks.Reset();
			}));
		}

		for (const FChunkId ChunkId : ChunksToRemove)
		{
			TSharedPtr<FChunk> Chunk;
			if (!ensure(Chunks_RequiresLock.RemoveAndCopyValue(ChunkId, Chunk)))
			{
				continue;
			}

			ensure(!Chunk->ChunkRef);
		}

		FVoxelUtilities::RunOnGameThread(MakeWeakPtrLambda(this, [=]
		{
			VOXEL_FUNCTION_COUNTER();
			check(IsInGameThread());

			ensure(bTaskInProgress);
			bTaskInProgress = false;

			if (NewTree->NumNodes() >= GVoxelChunkSpawnerMaxChunks)
			{
				VOXEL_MESSAGE(Error, "{0}: voxel.chunkspawner.MaxChunks reached", GraphNodeRef);
			}

			Octree = NewTree;
		}));
	}));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelScreenSizeChunkSpawner::FOctree::Update(
	const float ChunkScreenSize,
	TMap<FChunkId, FChunkInfo>& ChunkInfos,
	TSet<FChunkId>& ChunksToAdd,
	TSet<FChunkId>& ChunksToRemove,
	TSet<FChunkId>& ChunksToUpdate)
{
	VOXEL_FUNCTION_COUNTER();

	const auto ShowNode = [&](const FNodeRef NodeRef)
	{
		FNode& Node = GetNode(NodeRef);
		if (Node.bIsRendered)
		{
			return;
		}
		Node.bIsRendered = true;

		ChunkInfos.Add(Node.ChunkId,
		{
			GetChunkBounds(NodeRef),
			NodeRef.GetHeight(),
			Node.TransitionMask,
			NodeRef.GetBounds()
		});
		ChunksToAdd.Add(Node.ChunkId);
	};

	const auto HideNode = [&](const FNodeRef NodeRef)
	{
		FNode& Node = GetNode(NodeRef);
		if (!Node.bIsRendered)
		{
			return;
		}
		Node.bIsRendered = false;

		ChunkInfos.Add(Node.ChunkId,
		{
			GetChunkBounds(NodeRef),
			NodeRef.GetHeight(),
			Node.TransitionMask,
			NodeRef.GetBounds()
		});
		ChunksToRemove.Add(Node.ChunkId);
	};

	Traverse([&](const FNodeRef NodeRef)
	{
		// Always create root to avoid zero-centered bounds
		if (NodeRef.IsRoot())
		{
			if (!HasAnyChildren(NodeRef))
			{
				CreateAllChildren(NodeRef);
			}

			return true;
		}

		if (NumNodes() > GVoxelChunkSpawnerMaxChunks)
		{
			return false;
		}

		if (NodeRef.GetHeight() > 0)
		{
			const FVoxelBox ChunkBounds = GetChunkBounds(NodeRef);

			const double Distance = ChunkBounds.DistanceFromBoxToPoint(ViewOrigin);
			// Don't take the projection/FOV into account, as it leads to
			// unwanted/unstable results on different screen ratio or when zooming
			const double ScreenSize = ChunkBounds.Size().GetMax() / FMath::Max(1., Distance);

			if (ScreenSize > ChunkScreenSize ||
				NodeRef.GetHeight() > Object.MaxLOD)
			{
				if (!HasAnyChildren(NodeRef))
				{
					CreateAllChildren(NodeRef);
				}
				HideNode(NodeRef);

				return true;
			}
		}

		if (HasAnyChildren(NodeRef))
		{
			TraverseChildren(NodeRef, HideNode);
			DestroyAllChildren(NodeRef);
		}

		ShowNode(NodeRef);

		return false;
	});

	if (!Object.bEnableTransitions)
	{
		return;
	}

	VOXEL_SCOPE_COUNTER("Update transitions");

	Traverse([&](const FNodeRef NodeRef)
	{
		FNode& Node = GetNode(NodeRef);
		if (!Node.bIsRendered)
		{
			return;
		}

		uint8 TransitionMask = 0;

		for (int32 Direction = 0; Direction < 6; Direction++)
		{
			if (AdjacentNodeHasHigherHeight(NodeRef, Direction))
			{
				TransitionMask |= (1 << Direction);
			}
		}

		if (TransitionMask == Node.TransitionMask)
		{
			return;
		}
		Node.TransitionMask = TransitionMask;

		if (ChunksToAdd.Contains(Node.ChunkId))
		{
			ChunkInfos[Node.ChunkId].TransitionMask = TransitionMask;
		}
		else
		{
			ChunksToUpdate.Add(Node.ChunkId);

			ensure(!ChunkInfos.Contains(Node.ChunkId));
			ChunkInfos.Add(Node.ChunkId,
			{
				GetChunkBounds(NodeRef),
				NodeRef.GetHeight(),
				TransitionMask,
				NodeRef.GetBounds()
			});
		}
	});
}

FORCEINLINE bool FVoxelScreenSizeChunkSpawner::FOctree::AdjacentNodeHasHigherHeight(const FNodeRef NodeRef, const int32 Direction) const
{
	const int32 NodeSize = NodeRef.GetSize();

	FVector Offset;
	switch (Direction)
	{
	default: VOXEL_ASSUME(false);
	case 0: Offset = FVector(-NodeSize, 0, 0); break;
	case 1: Offset = FVector(+NodeSize, 0, 0); break;
	case 2: Offset = FVector(0, -NodeSize, 0); break;
	case 3: Offset = FVector(0, +NodeSize, 0); break;
	case 4: Offset = FVector(0, 0, -NodeSize); break;
	case 5: Offset = FVector(0, 0, +NodeSize); break;
	}

	const FVector PositionToQuery = NodeRef.GetBounds().ToVoxelBox().GetCenter() + Offset;

	if (!Root().GetBounds().ContainsFloat(PositionToQuery))
	{
		return {};
	}

	FNodeRef Result = Root();

	while (
		Result.GetHeight() > NodeRef.GetHeight() &&
		!GetNode(Result).bIsRendered)
	{
		if (!TryGetChild(Result, PositionToQuery, Result))
		{
			return false;
		}
	}

	checkVoxelSlow(Result.GetBounds().ContainsFloat(PositionToQuery));
	return Result.GetHeight() > NodeRef.GetHeight();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_MakeScreenSizeChunkSpawner, Spawner)
{
	const TValue<float> WorldSize = Get(WorldSizePin, Query);
	const TValue<int32> ChunkSize = Get(ChunkSizePin, Query);
	const TValue<int32> MaxLOD = Get(MaxLODPin, Query);
	const TValue<bool> EnableTransitions = Get(EnableTransitionsPin, Query);

	return VOXEL_ON_COMPLETE(WorldSize, ChunkSize, MaxLOD, EnableTransitions)
	{
		const TSharedRef<FVoxelScreenSizeChunkSpawner> Spawner = MakeVoxelShared<FVoxelScreenSizeChunkSpawner>();
		Spawner->GraphNodeRef = GetNodeRef();
		Spawner->WorldSize = WorldSize;
		Spawner->ChunkSize = FMath::Clamp(FMath::CeilToInt(ChunkSize / 2.f) * 2, 4, 128);
		Spawner->MaxLOD = MaxLOD;
		Spawner->bEnableTransitions = EnableTransitions;
		Spawner->ChunkScreenSizeValueFactory = MakeDynamicValueFactory(ChunkScreenSizePin);
		Spawner->QueryContext = Query.GetSharedContext();
		Spawner->QueryParameters = Query.GetSharedParameters();
		return Spawner;
	};
}