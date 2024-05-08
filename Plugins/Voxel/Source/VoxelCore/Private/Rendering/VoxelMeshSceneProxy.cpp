// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Rendering/VoxelMeshSceneProxy.h"
#include "Rendering/VoxelMesh.h"
#include "Rendering/VoxelMeshComponent.h"
#include "Engine/Engine.h"
#include "RayTracingInstance.h"
#include "Materials/Material.h"

// Needed to cancel motion blur when reusing proxies
#include "ScenePrivate.h"

VOXEL_CONSOLE_VARIABLE(
	VOXELCORE_API, bool, GVoxelShowMeshSections, false,
	"voxel.mesh.ShowMeshSections",
	"If true, will assign a unique color to each mesh section");

VOXEL_CONSOLE_VARIABLE(
	VOXELCORE_API, bool, GVoxelDisableStaticPath, false,
	"voxel.mesh.DisableStaticPath",
	"");

VOXEL_CONSOLE_VARIABLE(
	VOXELCORE_API, bool, GVoxelDisableOcclusionCulling, false,
	"voxel.mesh.DisableOcclusionCulling",
	"");

FVoxelMeshSceneProxy::FVoxelMeshSceneProxy(const UVoxelMeshComponent& Component)
	: FPrimitiveSceneProxy(&Component)
	, MaterialRelevance(Component.GetMaterialRelevance(GetScene().GetFeatureLevel()))
	, Mesh(Component.Mesh.Get().ToSharedRef())
	, bShouldDrawVelocity(Mesh->ShouldDrawVelocity())
	, bOnlyDrawIfSelected(Component.bOnlyDrawIfSelected)
	, OwnerForSelectionCheck(Component.GetOwner())
{
	// Mesh can't be deformed, required for VSM caching
	bHasDeformableMesh = false;
	bVisibleInLumenScene = Mesh->GetCardRepresentationData() != nullptr;
	bSupportsDistanceFieldRepresentation = MaterialRelevance.bOpaque && !MaterialRelevance.bUsesSingleLayerWaterMaterial;

	EnableGPUSceneSupportFlags();

	// Hack to cancel motion blur when mesh components are reused in the same frame
	VOXEL_ENQUEUE_RENDER_COMMAND(UpdateTransformCommand)(
		[this, PreviousLocalToWorld = Component.GetRenderMatrix()](FRHICommandListImmediate& RHICmdList)
		{
			FScene& Scene = static_cast<FScene&>(GetScene());
			Scene.VelocityData.OverridePreviousTransform(GetPrimitiveComponentId(), PreviousLocalToWorld);
		});
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelMeshSceneProxy::CreateRenderThreadResources()
{
	VOXEL_FUNCTION_COUNTER();
	check(IsInRenderingThread());

	Mesh->CallInitialize_RenderThread(
		FRHICommandListExecutor::GetImmediateCommandList(),
		GetScene().GetFeatureLevel());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelMeshSceneProxy::DrawStaticElements(FStaticPrimitiveDrawInterface* PDI)
{
	VOXEL_FUNCTION_COUNTER();

	const TSharedRef<FVoxelMaterialRef> Material = Mesh->GetMaterialSafe();

	// Make sure to keep a ref to the material
	// Otherwise, if the Mesh updates it, the cached static mesh commands might have a dangling ref and crash
	MaterialsToKeepAlive.Enqueue(Material);

	const UMaterialInterface* MaterialObject = Material->GetMaterial();
	if (!MaterialObject)
	{
		// Will happen in force delete
		return;
	}
	const FMaterialRenderProxy* MaterialProxy = MaterialObject->GetRenderProxy();

	FMeshBatch MeshBatch;
	if (!DrawMesh(MeshBatch, MaterialProxy, false))
	{
		return;
	}

	// Else the virtual texture check fails in RuntimeVirtualTextureRender.cpp:338
	// and the static mesh isn't rendered at all
	MeshBatch.LODIndex = 0;

	if (!GVoxelDisableStaticPath)
	{
		PDI->DrawMesh(MeshBatch, FLT_MAX);
	}

	if (RuntimeVirtualTextureMaterialTypes.Num() > 0)
	{
		// Runtime virtual texture mesh elements.
		MeshBatch.CastShadow = 0;
		MeshBatch.bUseAsOccluder = 0;
		MeshBatch.bUseForDepthPass = 0;
		MeshBatch.bUseForMaterial = 0;
		MeshBatch.bDitheredLODTransition = 0;
		MeshBatch.bRenderToVirtualTexture = 1;

		for (ERuntimeVirtualTextureMaterialType MaterialType : RuntimeVirtualTextureMaterialTypes)
		{
			MeshBatch.RuntimeVirtualTextureMaterialType = uint32(MaterialType);
			PDI->DrawMesh(MeshBatch, FLT_MAX);
		}
	}
}

void FVoxelMeshSceneProxy::GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const
{
	VOXEL_FUNCTION_COUNTER();

	const FEngineShowFlags& EngineShowFlags = ViewFamily.EngineShowFlags;

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	// Render bounds
	{
		VOXEL_SCOPE_COUNTER("Render Bounds");
		for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
		{
			if (VisibilityMap & (1 << ViewIndex))
			{
				RenderBounds(Collector.GetPDI(ViewIndex), EngineShowFlags, GetBounds(), IsSelected());
			}
		}
	}
#endif

	if (EngineShowFlags.Collision ||
		EngineShowFlags.CollisionPawn ||
		EngineShowFlags.CollisionVisibility)
	{
		return;
	}

	if (ShouldUseStaticPath(ViewFamily))
	{
		return;
	}

	VOXEL_SCOPE_COUNTER("Render Mesh");

	const TSharedRef<FVoxelMaterialRef> Material = Mesh->GetMaterialSafe();
	const UMaterialInterface* MaterialObject = Material->GetMaterial();
	if (!MaterialObject)
	{
		// Will happen in force delete
		return;
	}

	const FMaterialRenderProxy* MaterialProxy = MaterialObject->GetRenderProxy();

	if (GVoxelShowMeshSections)
	{
		const uint32 Hash = FVoxelUtilities::MurmurHash64(uint64(&Mesh.Get()));
		MaterialProxy = FVoxelRenderUtilities::CreateColoredRenderProxy(
			Collector,
			ReinterpretCastRef<const FColor>(Hash));
	}

	for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
	{
		if (!(VisibilityMap & (1 << ViewIndex)))
		{
			continue;
		}

		FMeshBatch& MeshBatch = Collector.AllocateMesh();
		if (DrawMesh(MeshBatch, MaterialProxy, EngineShowFlags.Wireframe))
		{
			VOXEL_SCOPE_COUNTER("Collector.AddMesh");
			Collector.AddMesh(ViewIndex, MeshBatch);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#if RHI_RAYTRACING
void FVoxelMeshSceneProxy::GetDynamicRayTracingInstances(FRayTracingMaterialGatheringContext& Context, TArray<FRayTracingInstance>& OutRayTracingInstances)
{
	VOXEL_FUNCTION_COUNTER();

	FMeshBatch MeshBatch;
	const FRayTracingGeometry* RayTracingGeometry = Mesh->DrawRaytracing_RenderThread(*this, MeshBatch);
	if (!RayTracingGeometry)
	{
		return;
	}

	const TSharedRef<FVoxelMaterialRef> Material = Mesh->GetMaterialSafe();
	const UMaterialInterface* MaterialObject = Material->GetMaterial();
	if (!MaterialObject)
	{
		// Will happen in force delete
		return;
	}

	MeshBatch.SegmentIndex = 0;
	MeshBatch.MaterialRenderProxy = MaterialObject->GetRenderProxy();
	MeshBatch.ReverseCulling = IsLocalToWorldDeterminantNegative();

	FRayTracingInstance RayTracingInstance;
	RayTracingInstance.Geometry = RayTracingGeometry;
	RayTracingInstance.InstanceTransforms.Add(GetLocalToWorld());
	RayTracingInstance.Materials.Add(MeshBatch);
	Context.BuildInstanceMaskAndFlags(RayTracingInstance, *this);

	OutRayTracingInstances.Add(RayTracingInstance);
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FPrimitiveViewRelevance FVoxelMeshSceneProxy::GetViewRelevance(const FSceneView* View) const
{
	const FEngineShowFlags& EngineShowFlags = View->Family->EngineShowFlags;

	FPrimitiveViewRelevance Result;
	Result.bDrawRelevance = IsShown(View);
	Result.bShadowRelevance = IsShadowCast(View);

#if WITH_EDITOR
	if (bOnlyDrawIfSelected)
	{
		Result.bDrawRelevance =
			!EngineShowFlags.Game &&
			FVoxelGameUtilities::IsActorSelected_AnyThread(OwnerForSelectionCheck);
	}
#endif

	if (ShouldUseStaticPath(*View->Family))
	{
		Result.bStaticRelevance = true;
		Result.bDynamicRelevance = false;
	}
	else
	{
		Result.bStaticRelevance = false;
		Result.bDynamicRelevance = true;
	}

	if (EngineShowFlags.Bounds)
	{
		Result.bDynamicRelevance = true;
	}

	Result.bRenderInMainPass = ShouldRenderInMainPass();
	Result.bUsesLightingChannels = GetLightingChannelMask() != GetDefaultLightingChannelMask();
	Result.bRenderCustomDepth = ShouldRenderCustomDepth();
	Result.bTranslucentSelfShadow = bCastVolumetricTranslucentShadow;
	Result.bVelocityRelevance = bShouldDrawVelocity && IsMovable() && Result.bOpaque && Result.bRenderInMainPass;

	MaterialRelevance.SetPrimitiveViewRelevance(Result);

	return Result;
}

bool FVoxelMeshSceneProxy::CanBeOccluded() const
{
	return !GVoxelDisableOcclusionCulling && !MaterialRelevance.bDisableDepthTest;
}

uint32 FVoxelMeshSceneProxy::GetMemoryFootprint() const
{
	return sizeof(*this) + GetAllocatedSize();
}

SIZE_T FVoxelMeshSceneProxy::GetTypeHash() const
{
	static size_t UniquePointer;
	return reinterpret_cast<size_t>(&UniquePointer);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelMeshSceneProxy::GetDistanceFieldAtlasData(const FDistanceFieldVolumeData*& OutDistanceFieldData, float& SelfShadowBias) const
{
	OutDistanceFieldData = Mesh->GetDistanceFieldVolumeData();
}

void FVoxelMeshSceneProxy::GetDistanceFieldInstanceData(TArray<FRenderTransform>& ObjectLocalToWorldTransforms) const
{
	if (FVoxelMeshSceneProxy::HasDistanceFieldRepresentation())
	{
		ObjectLocalToWorldTransforms.Add(FRenderTransform::Identity);
	}
}

const FCardRepresentationData* FVoxelMeshSceneProxy::GetMeshCardRepresentation() const
{
	return Mesh->GetCardRepresentationData();
}

bool FVoxelMeshSceneProxy::HasDistanceFieldRepresentation() const
{
	if (!CastsDynamicShadow() ||
		!AffectsDistanceFieldLighting())
	{
		return false;
	}

	return Mesh->GetDistanceFieldVolumeData() != nullptr;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool FVoxelMeshSceneProxy::HasDynamicIndirectShadowCasterRepresentation() const
{
	return bCastsDynamicIndirectShadow && FVoxelMeshSceneProxy::HasDistanceFieldRepresentation();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool FVoxelMeshSceneProxy::ShouldUseStaticPath(const FSceneViewFamily& ViewFamily) const
{
	return
		!IsRichView(ViewFamily) &&
		!GVoxelDisableStaticPath &&
		!GVoxelShowMeshSections;
}

bool FVoxelMeshSceneProxy::DrawMesh(
	FMeshBatch& MeshBatch,
	const FMaterialRenderProxy* MaterialRenderProxy,
	const bool bWireframe) const
{
	VOXEL_FUNCTION_COUNTER();

	check(MaterialRenderProxy);

	MeshBatch.MaterialRenderProxy = MaterialRenderProxy;
	MeshBatch.ReverseCulling = IsLocalToWorldDeterminantNegative();
	MeshBatch.DepthPriorityGroup = SDPG_World;
	MeshBatch.bUseWireframeSelectionColoring = IsSelected() && bWireframe; // Else mesh LODs view is messed up when actor is selected
	MeshBatch.bCanApplyViewModeOverrides = true;

	return Mesh->Draw_RenderThread(*this, MeshBatch);
}