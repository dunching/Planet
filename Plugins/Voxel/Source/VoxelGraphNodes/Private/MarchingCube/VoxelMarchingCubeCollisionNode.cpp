// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "MarchingCube/VoxelMarchingCubeCollisionNode.h"
#include "MarchingCube/VoxelMarchingCubeNodes.h"
#include "VoxelInvoker.h"
#include "VoxelRuntime.h"
#include "Collision/VoxelCollisionComponent.h"
#include "Navigation/VoxelNavigationComponent.h"

VOXEL_CONSOLE_VARIABLE(
	VOXELGRAPHNODES_API, bool, GVoxelCollisionEnableInEditor, false,
	"voxel.collision.EnableInEditor",
	"");

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelNodeAliases::TValue<FVoxelCollider> FVoxelMarchingCubeCollisionExecNode::CreateCollider(
	const FVoxelQuery& InQuery,
	const float VoxelSize,
	const int32 ChunkSize,
	const FVoxelBox& Bounds) const
{
	checkVoxelSlow(FVoxelTaskReferencer::Get().IsReferenced(this));
	const FVoxelQuery Query = InQuery.EnterScope(*this);

	const TSharedRef<FVoxelQueryParameters> SurfaceParameters = Query.CloneParameters();
	SurfaceParameters->Add<FVoxelQueryChannelBoundsQueryParameter>().Bounds = Bounds;
	const TValue<FVoxelSurface> FutureSurface = GetNodeRuntime().Get(SurfacePin, Query.MakeNewQuery(SurfaceParameters));

	return VOXEL_CALL_NODE(FVoxelNode_CreateMarchingCubeCollider, ColliderPin, Query)
	{
		VOXEL_CALL_NODE_BIND(SurfacePin, VoxelSize, ChunkSize, Bounds, FutureSurface)
		{
			return VOXEL_CALL_NODE(FVoxelNode_GenerateMarchingCubeSurface, SurfacePin, Query)
			{
				VOXEL_CALL_NODE_BIND(DistancePin, FutureSurface)
				{
					return VOXEL_ON_COMPLETE(FutureSurface)
					{
						return FutureSurface->GetDistance(Query);
					};
				};
				VOXEL_CALL_NODE_BIND(VoxelSizePin, VoxelSize)
				{
					return VoxelSize;
				};
				VOXEL_CALL_NODE_BIND(ChunkSizePin, ChunkSize)
				{
					return ChunkSize;
				};
				VOXEL_CALL_NODE_BIND(BoundsPin, Bounds)
				{
					return Bounds;
				};
				VOXEL_CALL_NODE_BIND(EnableTransitionsPin)
				{
					return false;
				};
				VOXEL_CALL_NODE_BIND(PerfectTransitionsPin)
				{
					return false;
				};
				VOXEL_CALL_NODE_BIND(EnableDistanceChecksPin)
				{
					return true;
				};
				VOXEL_CALL_NODE_BIND(DistanceChecksTolerancePin)
				{
					return GetNodeRuntime().Get(DistanceChecksTolerancePin, Query);
				};
			};
		};

		VOXEL_CALL_NODE_BIND(PhysicalMaterialPin)
		{
			return GetNodeRuntime().Get(PhysicalMaterialPin, Query);
		};
	};
}

TVoxelUniquePtr<FVoxelExecNodeRuntime> FVoxelMarchingCubeCollisionExecNode::CreateExecRuntime(const TSharedRef<const FVoxelExecNode>& SharedThis) const
{
	return MakeVoxelUnique<FVoxelMarchingCubeCollisionExecNodeRuntime>(SharedThis);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelMarchingCubeCollisionExecNodeRuntime::Create()
{
	VOXEL_FUNCTION_COUNTER();

	if (!IsGameWorld() &&
		!GVoxelCollisionEnableInEditor)
	{
		return;
	}

	BodyInstance = GetConstantPin(Node.BodyInstancePin);
	bComputeCollision = GetConstantPin(Node.ComputeCollisionPin);
	bComputeNavmesh = GetConstantPin(Node.ComputeNavmeshPin);

	if (!bComputeCollision &&
		!bComputeNavmesh)
	{
		return;
	}

	const FName InvokerChannel = GetConstantPin(Node.InvokerChannelPin);
	const float VoxelSize = GetConstantPin(Node.VoxelSizePin);
	const int32 ChunkSize = GetConstantPin(Node.ChunkSizePin);
	const int32 FullChunkSize = FMath::CeilToInt(ChunkSize * VoxelSize);

	InvokerView = FVoxelInvokerManager::Get(GetWorld())->MakeView(
		InvokerChannel,
		FullChunkSize,
		0,
		GetLocalToWorld());

	InvokerView->Bind(
		MakeWeakPtrDelegate(this, [=](const TVoxelAddOnlySet<FIntVector>& ChunksToAdd)
		{
			VOXEL_SCOPE_COUNTER("OnAddChunk");
			VOXEL_SCOPE_LOCK(CriticalSection);

			for (const FIntVector& ChunkKey : ChunksToAdd)
			{
				TSharedPtr<FChunk>& Chunk = Chunks_RequiresLock.FindOrAdd(ChunkKey);
				if (ensure(!Chunk))
				{
					Chunk = MakeVoxelShared<FChunk>();
				}

				const FVoxelBox Bounds = FVoxelBox(FVector(ChunkKey) * FullChunkSize, FVector(ChunkKey + 1) * FullChunkSize);

				TVoxelDynamicValueFactory<FVoxelCollider> Factory(STATIC_FNAME("Marching Cube Collision"), [
					&Node = Node,
					VoxelSize,
					ChunkSize,
					Bounds](const FVoxelQuery& Query)
				{
					return Node.CreateCollider(Query, VoxelSize, ChunkSize, Bounds);
				});

				const TSharedRef<FVoxelQueryParameters> Parameters = MakeVoxelShared<FVoxelQueryParameters>();
				Parameters->Add<FVoxelLODQueryParameter>().LOD = 0;
				Chunk->Collider_RequiresLock = Factory
					.AddRef(NodeRef)
					.Priority(FVoxelTaskPriority::MakeBounds(
						Bounds,
						GetConstantPin(Node.PriorityOffsetPin),
						GetWorld(),
						GetLocalToWorld()))
					.Compute(GetContext(), Parameters);

				Chunk->Collider_RequiresLock.OnChanged(MakeWeakPtrLambda(this, [this, WeakChunk = MakeWeakPtr(Chunk)](const TSharedRef<const FVoxelCollider>& Collider)
				{
					QueuedColliders.Enqueue({ WeakChunk, Collider });
				}));
			}
		}),
		MakeWeakPtrDelegate(this, [this](const TVoxelAddOnlySet<FIntVector>& ChunksToRemove)
		{
			VOXEL_SCOPE_COUNTER("OnRemoveChunk");
			VOXEL_SCOPE_LOCK(CriticalSection);

			for (const FIntVector& ChunkKey : ChunksToRemove)
			{
				TSharedPtr<FChunk> Chunk;
				if (!ensure(Chunks_RequiresLock.RemoveAndCopyValue(ChunkKey, Chunk)))
				{
					return;
				}

				Chunk->Collider_RequiresLock = {};
				ChunksToDestroy.Enqueue(Chunk);
			}
		}));
}

void FVoxelMarchingCubeCollisionExecNodeRuntime::Destroy()
{
	VOXEL_FUNCTION_COUNTER();

	InvokerView = {};

	ProcessChunksToDestroy();
	{
		VOXEL_SCOPE_LOCK(CriticalSection);

		const TSharedPtr<FVoxelRuntime> Runtime = GetRuntime();
		for (const auto& It : Chunks_RequiresLock)
		{
			FChunk& Chunk = *It.Value;
			Chunk.Collider_RequiresLock = {};

			if (Runtime)
			{
				Runtime->DestroyComponent(Chunk.CollisionComponent_GameThread);
				Runtime->DestroyComponent(Chunk.NavigationComponent_GameThread);
			}
			else
			{
				Chunk.CollisionComponent_GameThread.Reset();
				Chunk.NavigationComponent_GameThread.Reset();
			}
		}
		Chunks_RequiresLock.Empty();
	}
	ProcessChunksToDestroy();
}

void FVoxelMarchingCubeCollisionExecNodeRuntime::Tick(FVoxelRuntime& Runtime)
{
	VOXEL_FUNCTION_COUNTER();
	ensure(!IsDestroyed());

	ProcessChunksToDestroy();
	ProcessQueuedColliders(Runtime);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelMarchingCubeCollisionExecNodeRuntime::ProcessChunksToDestroy()
{
	VOXEL_FUNCTION_COUNTER();

	const TSharedPtr<FVoxelRuntime> Runtime = GetRuntime();
	ensure(Runtime || IsDestroyed());

	TSharedPtr<FChunk> Chunk;
	while (ChunksToDestroy.Dequeue(Chunk))
	{
		if (!ensure(Chunk))
		{
			continue;
		}
		ensure(!Chunk->Collider_RequiresLock.IsValid());

		if (Runtime)
		{
			Runtime->DestroyComponent(Chunk->CollisionComponent_GameThread);
			Runtime->DestroyComponent(Chunk->NavigationComponent_GameThread);
		}
		else
		{
			Chunk->CollisionComponent_GameThread.Reset();
			Chunk->NavigationComponent_GameThread.Reset();
		}
	}
}

void FVoxelMarchingCubeCollisionExecNodeRuntime::ProcessQueuedColliders(FVoxelRuntime& Runtime)
{
	VOXEL_FUNCTION_COUNTER();
	check(IsInGameThread());
	ensure(!IsDestroyed());

	FQueuedCollider QueuedCollider;
	while (QueuedColliders.Dequeue(QueuedCollider))
	{
		const TSharedPtr<FChunk> Chunk = QueuedCollider.Chunk.Pin();
		if (!Chunk)
		{
			continue;
		}

		const TSharedRef<const FVoxelCollider> Collider = QueuedCollider.Collider.ToSharedRef();
		if (Collider->GetStruct() == StaticStructFast<FVoxelCollider>())
		{
			Runtime.DestroyComponent(Chunk->CollisionComponent_GameThread);
			Runtime.DestroyComponent(Chunk->NavigationComponent_GameThread);
			continue;
		}
		ensure(bComputeCollision || bComputeNavmesh);

		if (bComputeCollision)
		{
			UVoxelCollisionComponent* Component = Chunk->CollisionComponent_GameThread.Get();
			if (!Component)
			{
				Component = Runtime.CreateComponent<UVoxelCollisionComponent>();
				Chunk->CollisionComponent_GameThread = Component;
			}

			if (ensure(Component))
			{
				Component->SetRelativeLocation(Collider->GetOffset());
				Component->SetBodyInstance(*BodyInstance);
				Component->SetCollider(Collider);
			}
		}

		if (bComputeNavmesh)
		{
			UVoxelNavigationComponent* Component = Chunk->NavigationComponent_GameThread.Get();
			if (!Component)
			{
				Component = Runtime.CreateComponent<UVoxelNavigationComponent>();
				Chunk->NavigationComponent_GameThread = Component;
			}

			if (ensure(Component))
			{
				Component->SetRelativeLocation(Collider->GetOffset());
				Component->SetNavigationMesh(Collider->GetNavmesh());
			}
		}
	}
}