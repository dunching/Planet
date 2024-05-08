// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "PrimitiveSceneProxy.h"

struct FVoxelMesh;
class UVoxelMeshComponent;

class FVoxelMeshSceneProxy : public FPrimitiveSceneProxy
{
public:
	explicit FVoxelMeshSceneProxy(const UVoxelMeshComponent& Component);

	//~ Begin FPrimitiveSceneProxy Interface
	virtual void CreateRenderThreadResources() override;

	virtual void DrawStaticElements(FStaticPrimitiveDrawInterface* PDI) override;
	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override;

#if RHI_RAYTRACING
	virtual bool IsRayTracingRelevant() const override { return true; }
	virtual void GetDynamicRayTracingInstances(FRayTracingMaterialGatheringContext& Context, TArray<FRayTracingInstance>& OutRayTracingInstances) override;
#endif

	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override;
	virtual bool CanBeOccluded() const override;
	virtual uint32 GetMemoryFootprint() const override;
	virtual SIZE_T GetTypeHash() const override;

	virtual void GetDistanceFieldAtlasData(const FDistanceFieldVolumeData*& OutDistanceFieldData, float& SelfShadowBias) const override;
	virtual void GetDistanceFieldInstanceData(TArray<FRenderTransform>& ObjectLocalToWorldTransforms) const override;

	virtual const FCardRepresentationData* GetMeshCardRepresentation() const override;
	virtual bool HasDistanceFieldRepresentation() const override;

	virtual bool HasDynamicIndirectShadowCasterRepresentation() const override;
	//~ End FPrimitiveSceneProxy Interface

private:
	const FMaterialRelevance MaterialRelevance;
	const TSharedRef<const FVoxelMesh> Mesh;
	const bool bShouldDrawVelocity;
	const bool bOnlyDrawIfSelected;
	const FObjectKey OwnerForSelectionCheck;

	TQueue<TSharedPtr<FVoxelMaterialRef>, EQueueMode::Mpsc> MaterialsToKeepAlive;

	bool ShouldUseStaticPath(const FSceneViewFamily& ViewFamily) const;

	bool DrawMesh(
		FMeshBatch& MeshBatch,
		const FMaterialRenderProxy* MaterialRenderProxy,
		bool bWireframe) const;
};