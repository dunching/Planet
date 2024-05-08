// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelPointCollisionSmallChunk.h"
#include "VoxelRuntime.h"
#include "VoxelInstancedCollisionComponent.h"

DEFINE_VOXEL_INSTANCE_COUNTER(FVoxelPointCollisionSmallChunk);

FVoxelPointCollisionSmallChunk::FVoxelPointCollisionSmallChunk(
	const FVoxelGraphNodeRef& NodeRef,
	const FVoxelPointChunkRef& ChunkRef,
	const FVoxelBox& Bounds,
	const TSharedRef<const FVoxelRuntimeInfo>& RuntimeInfo,
	const TSharedRef<const FBodyInstance>& BodyInstance,
	const TSharedRef<FVoxelPointCollisionLargeChunk>& LargeChunk)
	: NodeRef(NodeRef)
	, ChunkRef(ChunkRef)
	, Bounds(Bounds)
	, RuntimeInfo(RuntimeInfo)
	, BodyInstance(BodyInstance)
	, LargeChunk(LargeChunk)
{
}

void FVoxelPointCollisionSmallChunk::Initialize()
{
	LargeChunk->AddOnChanged(MakeWeakPtrLambda(this, [this](const FMeshToPoints& MeshToPoints)
	{
		UpdatePoints(MeshToPoints);
	}));
}

void FVoxelPointCollisionSmallChunk::Destroy(FVoxelRuntime& Runtime)
{
	VOXEL_FUNCTION_COUNTER();
	VOXEL_SCOPE_LOCK(CriticalSection);
	check(IsInGameThread());

	for (auto& It : MeshToComponent_RequiresLock)
	{
		Runtime.DestroyComponent(It.Value.Component);
	}
	MeshToComponent_RequiresLock.Empty();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelPointCollisionSmallChunk::UpdatePoints(const FMeshToPoints& MeshToPoints)
{
	VOXEL_SCOPE_COUNTER_FORMAT("FVoxelPointCollisionSmallChunk::UpdatePoints Num=%d", MeshToPoints.Num());
	ensure(!IsInGameThread());
	FVoxelNodeStatScope StatScope(*this, MeshToPoints.Num());

	VOXEL_SCOPE_LOCK(CriticalSection);

	TVoxelArray<TSharedPtr<FVoxelInstancedCollisionData>> LockedDatas;
	for (const auto& It : MeshToComponent_RequiresLock)
	{
		LockedDatas.Add(It.Value.Data);
	}

	for (const TSharedPtr<FVoxelInstancedCollisionData>& Data : LockedDatas)
	{
		Data->CriticalSection.Lock();
	}
	ON_SCOPE_EXIT
	{
		for (const TSharedPtr<FVoxelInstancedCollisionData>& Data : LockedDatas)
		{
			Data->CriticalSection.Unlock();
		}
	};

	{
		VOXEL_SCOPE_COUNTER("Remove instances")

		for (const auto& It : MeshToComponent_RequiresLock)
		{
			const TVoxelAddOnlyMap<FVoxelPointId, FPointTransform>* PointToTransforms = MeshToPoints.Find(It.Key);

			FVoxelInstancedCollisionDataImpl& Data = It.Value.Data->GetDataImpl_RequiresLock();
			for (int32 Index = 0; Index < Data.PointIds.Num(); Index++)
			{
				const FVoxelPointId PointId = Data.PointIds[Index];
				if (!PointId.IsValid())
				{
					// Unassigned
					checkVoxelSlow(Data.FreeIndices.Contains(Index));
					continue;
				}

				if (PointToTransforms)
				{
					if (const FPointTransform* Transform = PointToTransforms->Find(PointId))
					{
						if (Bounds.Contains(
							Transform->PositionX,
							Transform->PositionY,
							Transform->PositionZ))
						{
							continue;
						}
					}
				}

				Data.PointIds[Index] = {};
				Data.Transforms[Index] = {};
				Data.InstanceBodiesToDelete.Add(MoveTemp(Data.InstanceBodies[Index]));

				Data.FreeIndices.Add(Index);

				int32 OtherIndex = -1;
				ensure(Data.PointIdToIndex.RemoveAndCopyValue(PointId, OtherIndex));
				ensure(Index == OtherIndex);
			}
		}
	}

	{
		VOXEL_SCOPE_COUNTER("Add instances")

		for (const auto& MeshIt : MeshToPoints)
		{
			const FVoxelStaticMesh Mesh = MeshIt.Key;
			FComponent& Component = MeshToComponent_RequiresLock.FindOrAdd(Mesh);
			if (!Component.Data)
			{
				Component.Data = MakeVoxelShared<FVoxelInstancedCollisionData>(Mesh, ChunkRef);
				Component.Data->CriticalSection.Lock();
				LockedDatas.Add(Component.Data);
			}
			FVoxelInstancedCollisionDataImpl& Data = Component.Data->GetDataImpl_RequiresLock();

			for (const auto& PointIt : MeshIt.Value)
			{
				const FVoxelPointId PointId = PointIt.Key;
				const FPointTransform& Transform = PointIt.Value;

				if (!Bounds.Contains(
					Transform.PositionX,
					Transform.PositionY,
					Transform.PositionZ))
				{
					continue;
				}

				const FTransform3f NewTransform(
					FQuat4f(
						Transform.RotationX,
						Transform.RotationY,
						Transform.RotationZ,
						Transform.RotationW),
					FVector3f(
						Transform.PositionX,
						Transform.PositionY,
						Transform.PositionZ),
					FVector3f(
						Transform.ScaleX,
						Transform.ScaleY,
						Transform.ScaleZ));

				if (const int32* IndexPtr = Data.PointIdToIndex.Find(PointId))
				{
					const int32 Index = *IndexPtr;

					checkVoxelSlow(Data.PointIds[Index] == PointId);

					FTransform3f& OldTransform = Data.Transforms[Index];
					if (!OldTransform.Equals(NewTransform))
					{
						OldTransform = NewTransform;
						Data.InstanceBodiesToUpdate.Add(Index);
					}

					continue;
				}

				int32 Index;
				if (Data.FreeIndices.Num() > 0)
				{
					Index = Data.FreeIndices.Pop(false);
				}
				else
				{
					Index = Data.PointIds.Emplace();
					ensureVoxelSlow(Index == Data.Transforms.Emplace());
					ensureVoxelSlow(Index == Data.InstanceBodies.Emplace());
				}

				Data.PointIdToIndex.Add_CheckNew(PointId, Index);
				Data.InstanceBodiesToUpdate.Add(Index);

				Data.PointIds[Index] = PointId;
				Data.Transforms[Index] = NewTransform;
				ensureVoxelSlow(!Data.InstanceBodies[Index].IsValid());
			}
		}
	}

	FVoxelUtilities::RunOnGameThread_Async(MakeWeakPtrLambda(this, [this]
	{
		VOXEL_SCOPE_COUNTER("UpdatePoints_GameThread");
		VOXEL_SCOPE_LOCK(CriticalSection);

		const TSharedPtr<FVoxelRuntime> Runtime = GetRuntime();
		if (!ensure(Runtime))
		{
			return;
		}

		for (auto It = MeshToComponent_RequiresLock.CreateIterator(); It; ++It)
		{
			bool bDestroy;
			{
				// Make sure to not recursively lock: DestroyComponent will also lock
				VOXEL_SCOPE_LOCK(It.Value().Data->CriticalSection);
				bDestroy = It.Value().Data->GetDataImpl_RequiresLock().PointIdToIndex.Num() == 0;
			}

			if (bDestroy)
			{
				Runtime->DestroyComponent(It.Value().Component);
				It.RemoveCurrent();
				continue;
			}

			UVoxelInstancedCollisionComponent* Component = It.Value().Component.Get();
			if (!Component)
			{
				Component = Runtime->CreateComponent<UVoxelInstancedCollisionComponent>();

				FVoxelGameUtilities::CopyBodyInstance(
					Component->BodyInstance,
					*BodyInstance);

				Component->SetData(It.Value().Data.ToSharedRef());
				It.Value().Component = Component;
			}

			ensure(Component->GetData() == It.Value().Data);
			Component->Update();
		}
	}));
}