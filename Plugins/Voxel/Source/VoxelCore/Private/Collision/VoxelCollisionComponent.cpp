// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Collision/VoxelCollisionComponent.h"
#include "Collision/VoxelTriangleMeshCollider.h"

#include "Engine/Engine.h"
#include "SceneManagement.h"
#include "PrimitiveSceneProxy.h"
#include "Materials/Material.h"
#include "PhysicsEngine/BodySetup.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

void UVoxelCollisionComponent::SetBodyInstance(const FBodyInstance& NewBodyInstance)
{
	FVoxelGameUtilities::CopyBodyInstance(
		BodyInstance,
		NewBodyInstance);
}

void UVoxelCollisionComponent::SetCollider(const TSharedPtr<const FVoxelCollider>& NewCollider)
{
	VOXEL_FUNCTION_COUNTER();

	Collider = NewCollider;

	if (Collider)
	{
		Collider->UpdateStats();

		static TMap<TWeakObjectPtr<UPhysicalMaterial>, TWeakObjectPtr<UMaterial>> MaterialCache;
		for (auto It = MaterialCache.CreateIterator(); It; ++It)
		{
			if (!It.Key().IsValid() ||
				!It.Value().IsValid())
			{
				It.RemoveCurrent();
			}
		}

		for (const TWeakObjectPtr<UPhysicalMaterial>& PhysicalMaterial : Collider->GetPhysicalMaterials())
		{
			TWeakObjectPtr<UMaterial>& Material = MaterialCache.FindOrAdd(PhysicalMaterial);
			if (!Material.IsValid())
			{
				Material = NewObject<UMaterial>();
				Material->PhysMaterial = PhysicalMaterial.Get();
			}
			Materials.Add(Material.Get());
		}
	}
	else
	{
		Materials.Reset();
	}

	if (BodySetup)
	{
		BodySetup->ClearPhysicsMeshes();
	}
	else
	{
		BodySetup = NewObject<UBodySetup>(this);
		BodySetup->bGenerateMirroredCollision = false;
		BodySetup->CollisionTraceFlag = CTF_UseComplexAsSimple;
	}

	if (Collider)
	{
		ensure(BodySetup->ChaosTriMeshes.Num() == 0);
		Collider->AddToBodySetup(*BodySetup);
		BodySetup->bCreatedPhysicsMeshes = true;
	}

	BodyInstance.OwnerComponent = this;
	BodyInstance.UpdatePhysicalMaterials();

	RecreatePhysicsState();
	MarkRenderStateDirty();
}

void UVoxelCollisionComponent::ReturnToPool()
{
	VOXEL_FUNCTION_COUNTER();

	SetBodyInstance(FBodyInstance());
	SetCollider(nullptr);
}

bool UVoxelCollisionComponent::ShouldCreatePhysicsState() const
{
	return Collider.IsValid();
}

FBoxSphereBounds UVoxelCollisionComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	const FBox LocalBounds = Collider.IsValid() ? Collider->GetLocalBounds().ToFBox() : FBox(FVector::ZeroVector, FVector::ZeroVector);
	ensure(!LocalBounds.Min.ContainsNaN());
	ensure(!LocalBounds.Max.ContainsNaN());
	return LocalBounds.TransformBy(LocalToWorld);
}

void UVoxelCollisionComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	VOXEL_FUNCTION_COUNTER();

	Super::OnComponentDestroyed(bDestroyingHierarchy);

	// Clear memory
	Collider.Reset();

	if (BodySetup)
	{
		BodySetup->ClearPhysicsMeshes();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class FVoxelCollisionSceneProxy : public FPrimitiveSceneProxy
{
public:
	const TSharedRef<const FVoxelCollider> Collider;
	mutable TSharedPtr<IVoxelColliderRenderData> RenderData;

	explicit FVoxelCollisionSceneProxy(const UVoxelCollisionComponent& Component)
		: FPrimitiveSceneProxy(&Component)
		, Collider(Component.Collider.ToSharedRef())
	{
	}

	//~ Begin FPrimitiveSceneProxy Interface
	virtual void DestroyRenderThreadResources() override
	{
		VOXEL_FUNCTION_COUNTER();
		RenderData.Reset();
	}
	virtual void GetDynamicMeshElements(
		const TArray<const FSceneView*>& Views,
		const FSceneViewFamily& ViewFamily,
		const uint32 VisibilityMap,
		FMeshElementCollector& Collector) const override
	{
		VOXEL_FUNCTION_COUNTER();

		if (!RenderData)
		{
			RenderData = Collider->GetRenderData();
		}
		if (!RenderData)
		{
			return;
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

			FMeshBatch& MeshBatch = Collector.AllocateMesh();
			MeshBatch.MaterialRenderProxy = FVoxelRenderUtilities::CreateColoredRenderProxy(Collector, GetWireframeColor());
			MeshBatch.ReverseCulling = IsLocalToWorldDeterminantNegative();
			MeshBatch.bDisableBackfaceCulling = true;
			MeshBatch.DepthPriorityGroup = SDPG_World;
			RenderData->Draw_RenderThread(*this, MeshBatch);

			Collector.AddMesh(ViewIndex, MeshBatch);
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

FPrimitiveSceneProxy* UVoxelCollisionComponent::CreateSceneProxy()
{
	if (!GIsEditor ||
		!Collider)
	{
		return nullptr;
	}

	return new FVoxelCollisionSceneProxy(*this);
}

int32 UVoxelCollisionComponent::GetNumMaterials() const
{
	return Materials.Num();
}

UMaterialInterface* UVoxelCollisionComponent::GetMaterial(int32 ElementIndex) const
{
	if (Materials.Num() == 0)
	{
		return nullptr;
	}

	if (!ensure(Materials.IsValidIndex(ElementIndex)))
	{
		return nullptr;
	}

	return Materials[ElementIndex];
}