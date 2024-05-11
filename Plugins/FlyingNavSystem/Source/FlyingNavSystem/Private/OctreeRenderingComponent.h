// Copyright Ben Sutherland 2024. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"
#include "DebugRenderSceneProxy.h"
#include "FlyingNavSystemTypes.h"
#include "StaticMeshResources.h"
#include "Math/GenericOctree.h"

#include "OctreeRenderingComponent.generated.h"

struct FSVOGraph;
class AFlyingNavigationData;

// Struct to hold a coloured cube
struct FSVOBox
{
	FVector Centre;
	FCoord Extent;
	FColor Colour;
	
	FSVOBox(const FVector& Centre = FVector::ZeroVector, const float Extent = 0, const FColor& Colour = FColor::White):
		Centre(Centre), Extent(Extent), Colour(Colour)
	{}
	FSVOBox(const FVector& Centre, const FVector& Extent, const FColor& Colour):
        Centre(Centre), Extent(Extent.X), Colour(Colour)
	{}
	
	FSVOBox(const FBoxCenterAndExtent& Dimensions, const FColor& Colour):
		Centre(Dimensions.Center),
		Extent(Dimensions.Extent.X),
		Colour(Colour)
	{}
};

enum class EFlyingDataDisplayFlags : uint8
{
	NodeBoxes,
	SubNodeBoxes,
	OnlyOverlappedSubNodes,
	NeighbourConnections,
	SimplifiedConnections,
	ColourConnected
};

struct FOctreeSceneProxyData: TSharedFromThis<FOctreeSceneProxyData, ESPMode::ThreadSafe>
{
	TArray<FDebugRenderSceneProxy::FSphere> NodeSpheres;
	TArray<FDebugRenderSceneProxy::FDebugLine> NeighbourLines;

	// Processed rendering data
	TArray<FDynamicMeshVertex> Vertices;
	TArray<uint32> Indices;
	
	uint32 bDataGathered: 1;
	uint32 bNeedsNewData: 1;
	int32 NavDetailFlags;
	int32 GatheredNavDataHash;

	FOctreeSceneProxyData():
		bDataGathered(false),
		bNeedsNewData(true),
		NavDetailFlags(0),
		GatheredNavDataHash(0)
	{}

	void Reset();
	//void Serialize(FArchive& Ar);
	uint32 GetAllocatedSize() const;

#if !UE_BUILD_SHIPPING && !UE_BUILD_TEST
	static int32 GetDetailFlags(const AFlyingNavigationData* FlyingNavData);
	void GatherNeighbourLines(const FSVOLink NodeLink,
			const FVector& VoxelCentre,
			const bool bSimplified,
			const float CentreRadius,
			const FColor& Color,
			const FSVOData& NavData,
			const FSVOGraph& NavGraph
		);

	void GatherData(const AFlyingNavigationData* FlyingNavData, int32 InNavDetailFlags);
#endif // !UE_BUILD_SHIPPING && !UE_BUILD_TEST
};

/*
 * Rendering component for Sparse Voxel Octree used for flying pathfinding
 * @see Based on FNavMeshSceneProxy
 */
UCLASS(hidecategories=(Object,LOD, Physics, Collision), editinlinenew, meta=(BlueprintSpawnableComponent), ClassGroup=Rendering)
class UOctreeRenderingComponent : public UMeshComponent
{
	GENERATED_BODY()

	friend class FOctreeSceneProxy;
public:
	UOctreeRenderingComponent();
	
	//~ Begin UPrimitiveComponent Interface
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual void OnRegister()  override;
	virtual void OnUnregister()  override;
	//~ End UPrimitiveComponent Interface

	static bool IsNavigationShowFlagSet(const UWorld* World);

	//~ Begin USceneComponent Interface
	virtual FBoxSphereBounds CalcBounds(const FTransform &LocalToWorld) const override;
	//~ End USceneComponent Interface

	void ForceUpdate() { bForceUpdate = true; }
	bool IsForcingUpdate() const { return bForceUpdate; }

	void SetWireThickness(const float Thickness);

	bool bGatherData = true;
	
protected:
	//~ Begin UMeshComponent Interface.
	virtual int32 GetNumMaterials() const override;
	//~ End UMeshComponent Interface.

#if !UE_BUILD_SHIPPING && !UE_BUILD_TEST
	static void GatherData(const AFlyingNavigationData& NavData, FOctreeSceneProxyData& OutProxyData);
#endif // !UE_BUILD_SHIPPING && !UE_BUILD_TEST
	
	void TimerFunction();

	UPROPERTY()
	UMaterialInstanceDynamic* WireMaterial;

	// Gathered proxy data
	FOctreeSceneProxyData ProxyData;
	
	uint32 bCollectNavigationData : 1;
	uint32 bForceUpdate : 1;
	FTimerHandle TimerHandle;
};
