// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelInstancedCollisionComponent.h"
#include "Point/VoxelPointOverrideManager.h"
#include "SceneManagement.h"
#include "Engine/StaticMesh.h"
#include "PrimitiveSceneProxy.h"
#include "PhysicsEngine/BodySetup.h"

VOXEL_CONSOLE_VARIABLE(
	VOXELSPAWNER_API, bool, GVoxelFoliageShowInstancesCollisions, true,
	"voxel.foliage.ShowInstancesCollisions",
	"");

DEFINE_VOXEL_COUNTER(STAT_VoxelNumCollisionInstances);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelInstancedCollisionDataImpl::FlushInstanceBodiesToDelete()
{
	VOXEL_SCOPE_COUNTER_FORMAT("FlushInstanceBodiesToDelete Num=%d", InstanceBodiesToDelete.Num());

	for (const TSharedPtr<FBodyInstance>& Body : InstanceBodiesToDelete)
	{
		if (ensure(Body))
		{
			Body->TermBody();
		}
	}
	InstanceBodiesToDelete.Reset();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

UVoxelInstancedCollisionComponent::UVoxelInstancedCollisionComponent()
{
	SetGenerateOverlapEvents(false);

	// Needed for ItemIndex to be set
	bMultiBodyOverlap = true;
}

UBodySetup* UVoxelInstancedCollisionComponent::GetBodySetup()
{
	if (!Data)
	{
		return nullptr;
	}

	const UStaticMesh* StaticMesh = Data->Mesh.StaticMesh.Get();
	if (!StaticMesh)
	{
		return nullptr;
	}

	return StaticMesh->GetBodySetup();
}

bool UVoxelInstancedCollisionComponent::ShouldCreatePhysicsState() const
{
	return true;
}

FBoxSphereBounds UVoxelInstancedCollisionComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	VOXEL_FUNCTION_COUNTER();

	if (!Data)
	{
		return Super::CalcBounds(LocalToWorld);
	}

	VOXEL_SCOPE_LOCK(Data->CriticalSection);
	ensureVoxelSlow(Data->ChunkRef.IsValid());
	return Data->ChunkRef.GetBounds().ToFBox();
}

void UVoxelInstancedCollisionComponent::OnComponentDestroyed(const bool bDestroyingHierarchy)
{
	VOXEL_FUNCTION_COUNTER();

	ReturnToPool();

	Super::OnComponentDestroyed(bDestroyingHierarchy);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class FVoxelInstancedCollisionSceneProxy : public FPrimitiveSceneProxy
{
	const UVoxelInstancedCollisionComponent& Component;
	const TSharedRef<FVoxelInstancedCollisionData> Data;
	const UBodySetup* BodySetup;

public:
	explicit FVoxelInstancedCollisionSceneProxy(UVoxelInstancedCollisionComponent& Component)
		: FPrimitiveSceneProxy(&Component)
		, Component(Component)
		, Data(Component.Data.ToSharedRef())
		, BodySetup(Component.GetBodySetup())
	{
	}

	//~ Begin FPrimitiveSceneProxy Interface
	virtual void GetDynamicMeshElements(
		const TArray<const FSceneView*>& Views,
		const FSceneViewFamily& ViewFamily,
		const uint32 VisibilityMap,
		FMeshElementCollector& Collector) const override
	{
		VOXEL_FUNCTION_COUNTER();

		if (!GVoxelFoliageShowInstancesCollisions)
		{
			return;
		}

		static int32 NumRendered = 0;
		static uint64 NumRenderedFrame = 0;

		if (NumRenderedFrame != GFrameCounterRenderThread)
		{
			NumRenderedFrame = GFrameCounterRenderThread;
			NumRendered = 0;
		}

		for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
		{
			if (!(VisibilityMap & (1 << ViewIndex)))
			{
				continue;
			}

			ensure(
				Views[ViewIndex]->Family->EngineShowFlags.Collision ||
				Views[ViewIndex]->Family->EngineShowFlags.CollisionPawn ||
				Views[ViewIndex]->Family->EngineShowFlags.CollisionVisibility);

			const FMaterialRenderProxy* MaterialProxy = FVoxelRenderUtilities::CreateColoredRenderProxy(
				Collector,
				FColor(157, 149, 223, 255));

			VOXEL_SCOPE_LOCK(Data->CriticalSection);
			FVoxelInstancedCollisionDataImpl& DataImpl = Data->GetDataImpl_RequiresLock();

			const FVector ViewOrigin = Views[ViewIndex]->ViewMatrices.GetViewOrigin();

			VOXEL_SCOPE_LOCK(DataImpl.CriticalSection);
			for (int32 Index = 0; Index < DataImpl.AllBodyInstances_RequiresLock.Num(); Index++)
			{
				const TSharedPtr<FBodyInstance> BodyInstance = DataImpl.AllBodyInstances_RequiresLock[Index].Pin();
				if (!BodyInstance)
				{
					DataImpl.AllBodyInstances_RequiresLock.RemoveAtSwap(Index);
					Index--;
					continue;
				}

				if (NumRendered > 1000000)
				{
					VOXEL_MESSAGE(Error, "More than 1M foliage colliders being rendered in collision view, stopping");
					break;
				}
				NumRendered++;

				const FTransform Transform = BodyInstance->GetUnrealWorldTransform();

				if (FVector::DistSquared(Transform.GetLocation(), ViewOrigin) > FMath::Square(5000.f))
				{
					Collector.GetPDI(ViewIndex)->DrawPoint(
						Transform.GetLocation(),
						FLinearColor::Blue,
						5.f,
						SDPG_World);
					continue;
				}

				BodySetup->AggGeom.GetAggGeom(
					Transform,
					FColor(157, 149, 223, 255),
					MaterialProxy,
					false,
					true,
					DrawsVelocity(),
					ViewIndex,
					Collector);
			}
		}
	}

	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
	{
		FPrimitiveViewRelevance Result;
		Result.bDrawRelevance = true;
		Result.bRenderInMainPass = true;
		Result.bDynamicRelevance =
			View->Family->EngineShowFlags.Collision ||
			View->Family->EngineShowFlags.CollisionPawn ||
			View->Family->EngineShowFlags.CollisionVisibility;
		return Result;
	}

	virtual uint32 GetMemoryFootprint() const override
	{
		return sizeof(*this) + GetAllocatedSize();
	}

	virtual SIZE_T GetTypeHash() const override
	{
		static size_t UniquePointer;
		return reinterpret_cast<size_t>(&UniquePointer);
	}
	//~ End FPrimitiveSceneProxy Interface
};

FPrimitiveSceneProxy* UVoxelInstancedCollisionComponent::CreateSceneProxy()
{
	if (!GIsEditor ||
		!GVoxelFoliageShowInstancesCollisions ||
		!Data)
	{
		return nullptr;
	}

	return new FVoxelInstancedCollisionSceneProxy(*this);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void UVoxelInstancedCollisionComponent::OnCreatePhysicsState()
{
	// We want to avoid PrimitiveComponent base body instance at component location
	USceneComponent::OnCreatePhysicsState();
}

void UVoxelInstancedCollisionComponent::OnDestroyPhysicsState()
{
#if UE_ENABLE_DEBUG_DRAWING
	SendRenderDebugPhysics();
#endif

	// We want to avoid PrimitiveComponent base body instance at component location
	USceneComponent::OnDestroyPhysicsState();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool UVoxelInstancedCollisionComponent::TryGetPointHandle(
	const int32 ItemIndex,
	FVoxelPointHandle& OutHandle) const
{
	if (!ensure(Data))
	{
		return false;
	}

	VOXEL_SCOPE_LOCK(Data->CriticalSection);
	FVoxelInstancedCollisionDataImpl& DataImpl = Data->GetDataImpl_RequiresLock();

	if (!ensure(DataImpl.PointIds.IsValidIndex(ItemIndex)) ||
		// Will happen if the data was updated in-between us doing the trace & TryGetPointHandle being called
		!DataImpl.PointIds[ItemIndex].IsValid())
	{
		return false;
	}

	OutHandle.ChunkRef = Data->ChunkRef;
	OutHandle.PointId = DataImpl.PointIds[ItemIndex];
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void UVoxelInstancedCollisionComponent::SetData(const TSharedRef<FVoxelInstancedCollisionData>& NewData)
{
	ensure(!Data);
	Data = NewData;

	ensure(!OverrideChunk);
	OverrideChunk = FVoxelPointOverrideManager::Get(GetWorld())->FindOrAddChunk(NewData->ChunkRef);

	ensure(!OverrideChunkDelegatePtr);
	OverrideChunkDelegatePtr = MakeSharedVoid();

	{
		VOXEL_SCOPE_LOCK(OverrideChunk->CriticalSection);

		OverrideChunk->OnChanged_RequiresLock.Add(MakeWeakPtrDelegate(OverrideChunkDelegatePtr, [this](const TConstVoxelArrayView<FVoxelPointId> PointIds)
		{
			VOXEL_SCOPE_COUNTER("OnChanged");
			check(IsInGameThread());

			bool bShouldUpdate = false;
			{
				VOXEL_SCOPE_LOCK(Data->CriticalSection);
				FVoxelInstancedCollisionDataImpl& DataImpl = Data->GetDataImpl_RequiresLock();

				for (const FVoxelPointId& PointId : PointIds)
				{
					if (const int32* IndexPtr = DataImpl.PointIdToIndex.Find(PointId))
					{
						DataImpl.InstanceBodiesToUpdate.Add(*IndexPtr);
						bShouldUpdate = true;
					}
				}
			}

			if (bShouldUpdate)
			{
				Update();
			}
		}));
	}

	if (const UBodySetup* BodySetup = GetBodySetup())
	{
		if (!BodyInstance.GetOverrideWalkableSlopeOnInstance())
		{
			BodyInstance.SetWalkableSlopeOverride(BodySetup->WalkableSlopeOverride, false);
		}
	}
}

void UVoxelInstancedCollisionComponent::ReturnToPool()
{
	VOXEL_FUNCTION_COUNTER();

	NumInstances = 0;

	if (!Data)
	{
		return;
	}

	{
		VOXEL_SCOPE_LOCK(Data->CriticalSection);
		FVoxelInstancedCollisionDataImpl& DataImpl = Data->GetDataImpl_RequiresLock();
		VOXEL_SCOPE_COUNTER_FORMAT("Num=%d", DataImpl.InstanceBodies.Num());

		for (const TSharedPtr<FBodyInstance>& Body : DataImpl.InstanceBodies)
		{
			if (Body)
			{
				Body->TermBody();
			}
		}
		DataImpl.InstanceBodies.Empty();

		DataImpl.FlushInstanceBodiesToDelete();

		{
			VOXEL_SCOPE_LOCK(DataImpl.CriticalSection);
			for (const TWeakPtr<FBodyInstance>& WeakBodyInstance : DataImpl.AllBodyInstances_RequiresLock)
			{
				checkVoxelSlow(!WeakBodyInstance.IsValid());
			}
		}

		DataImpl = {};
	}

	Data = {};
	OverrideChunk = {};
	OverrideChunkDelegatePtr = {};

	MarkRenderStateDirty();
}

void UVoxelInstancedCollisionComponent::Update()
{
	VOXEL_FUNCTION_COUNTER();

	if (!ensure(Data) ||
		!ensure(OverrideChunk))
	{
		return;
	}

	VOXEL_SCOPE_LOCK(Data->CriticalSection);
	FVoxelInstancedCollisionDataImpl& DataImpl = Data->GetDataImpl_RequiresLock();
	ensure(DataImpl.PointIds.Num() == DataImpl.Transforms.Num());
	ensure(DataImpl.PointIds.Num() == DataImpl.InstanceBodies.Num());

	NumInstances = DataImpl.PointIdToIndex.Num();

	if (DataImpl.InstanceBodiesToUpdate.Num() == 0 &&
		DataImpl.InstanceBodiesToDelete.Num() == 0)
	{
		return;
	}

	UBodySetup* BodySetup = GetBodySetup();
	FPhysScene* PhysicsScene = GetWorld()->GetPhysicsScene();
	if (!ensure(BodySetup) ||
		!ensure(PhysicsScene))
	{
		return;
	}

	{
		VOXEL_SCOPE_COUNTER_FORMAT("InstanceBodiesToUpdate.Num=%d", DataImpl.InstanceBodiesToUpdate.Num());
		VOXEL_SCOPE_LOCK(DataImpl.CriticalSection);
		VOXEL_SCOPE_LOCK(OverrideChunk->CriticalSection);

		TCompatibleVoxelArray<FBodyInstance*> ValidBodyInstances;
		TCompatibleVoxelArray<FTransform> ValidTransforms;
		ValidBodyInstances.Reserve(DataImpl.InstanceBodiesToUpdate.Num());
		ValidTransforms.Reserve(DataImpl.InstanceBodiesToUpdate.Num());

		for (const int32 Index : DataImpl.InstanceBodiesToUpdate)
		{
			const FVoxelPointId PointId = DataImpl.PointIds[Index];
			const FTransform Transform(DataImpl.Transforms[Index]);
			TSharedPtr<FBodyInstance>& Body = DataImpl.InstanceBodies[Index];

			bool bShouldDelete = false;
			if (Transform.GetScale3D().IsNearlyZero())
			{
				bShouldDelete = true;
			}

			if (OverrideChunk->PointIdsToHide_RequiresLock.Contains(PointId))
			{
				bShouldDelete = true;
			}

			if (bShouldDelete)
			{
				if (Body)
				{
					Body->TermBody();
				}
				Body.Reset();
				continue;
			}

			if (Body)
			{
				ensure(Body->InstanceBodyIndex == Index);
				Body->SetBodyTransform(
					Transform,
					ETeleportType::TeleportPhysics);
				continue;
			}

			Body = MakeBodyInstance(Index);

			checkVoxelSlow(DataImpl.CriticalSection.IsLocked());
			DataImpl.AllBodyInstances_RequiresLock.Add(Body);

			ValidBodyInstances.Add(Body.Get());
			ValidTransforms.Add(Transform);

		}
		DataImpl.InstanceBodiesToUpdate.Reset();

		check(ValidBodyInstances.Num() == ValidTransforms.Num());
		if (ValidBodyInstances.Num() > 0)
		{
			FBodyInstance::InitStaticBodies(ValidBodyInstances, ValidTransforms, BodySetup, this, PhysicsScene);
		}
	}

	DataImpl.FlushInstanceBodiesToDelete();

	MarkRenderStateDirty();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedRef<FBodyInstance> UVoxelInstancedCollisionComponent::MakeBodyInstance(const int32 Index) const
{
	const TSharedRef<FBodyInstance> Body = TSharedPtr<FBodyInstance>(
		new (GVoxelMemory) FBodyInstance(),
		[](const FBodyInstance* InBodyInstance)
		{
			check(!InBodyInstance || !InBodyInstance->IsValidBodyInstance());
			FVoxelMemory::Delete(InBodyInstance);
		}).ToSharedRef();

	FVoxelGameUtilities::CopyBodyInstance(*Body, BodyInstance);
	Body->bAutoWeld = false;
	Body->bSimulatePhysics = false;
	Body->InstanceBodyIndex = Index;
	return Body;
}