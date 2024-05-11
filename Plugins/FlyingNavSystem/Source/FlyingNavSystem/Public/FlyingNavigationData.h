// Copyright Ben Sutherland 2024. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "NavigationData.h"
#include "FlyingNavSystemTypes.h"
#include "SVOQuerySettings.h"
#include "SVOGraph.h"
#include "Engine/LatentActionManager.h"
#include "HAL/ThreadSafeBool.h"

#include "FlyingNavigationData.generated.h"

class FFlyingNavigationDataGenerator;
class FSVOGenerator;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFlyingNavGenerationFinishedEvent);

struct FLYINGNAVSYSTEM_API FFlyingNavigationPath : FNavigationPath
{
	typedef FNavigationPath Super;

	FFlyingNavigationPath();
	virtual ~FFlyingNavigationPath() override;

	void ApplyFlags(int32 NavDataFlags);

	// Used for accessing pawn nav properties
    TWeakObjectPtr<const APawn> GoalPawn;
	
	// Update path asynchronously when observed
	bool bAsyncObservationUpdate;
	// Only to be accessed on game thread
	bool bCurrentlyBeingUpdated;
	// Path Result
	ENavigationQueryResult::Type QueryResult;

	int32 DrawIndex;

	static const FNavPathType Type;
};

/**
 * Actor to store navigation data for flying agents
 * Stores single octree
 */
UCLASS(config=Engine, defaultconfig, hidecategories=(Input,Rendering,Tags,"Utilities|Transformation",Actor,Layers,Replication), notplaceable)
class FLYINGNAVSYSTEM_API AFlyingNavigationData : public ANavigationData
{
	GENERATED_BODY()

public:
	AFlyingNavigationData();
	
	// Size in Unreal Units (cm) of the smallest details that can be captured.
	UPROPERTY(EditAnywhere, Category = Generation, Config, Meta = (ClampMin = "1", UIMin = "1"))
	double MaxDetailSize;

#if WITH_EDITORONLY_DATA
	// Number of layers the Sparse Voxel Octree will generate, including the SubNode layers. Display only.
	UPROPERTY(VisibleAnywhere, Category = Generation, Transient)
	int32 Subdivisions;

	// Actual side length of the smallest voxels. Display only.
	UPROPERTY(VisibleAnywhere, Category = Generation, Transient)
	double ActualVoxelSize;

	// Shows if the cached nav data can be used for pathfinding. Display only.
	UPROPERTY(VisibleAnywhere, Category = Generation, Transient)
	uint32 bCurrentlyBuilt: 1;
#endif // WITH_EDITOR
	
	// Build on multiple threads (highly recommended).
	UPROPERTY(EditAnywhere, Category = Generation, Config, AdvancedDisplay)
	uint32 bMultithreaded: 1;

	// How many times to initially subdivide the generation volume. Runs each subdivision on a separate thread. In powers of 8. Increases minimum subdivisions.
	UPROPERTY(EditAnywhere, Category = Generation, Config, AdvancedDisplay, Meta = (EditCondition="bMultithreaded"))
	EThreadSubdivisions ThreadSubdivisions;
	
	// Maximum number of threads to spawn when rasterising the level. If set to >= available CPU threads, can lock your computer for a while.
	UPROPERTY(EditAnywhere, Category = Generation, Config, AdvancedDisplay, Meta = (EditCondition="bMultithreaded", ClampMin = "1"))
	int32 MaxThreads;
	
	// Whether to expand the voxel collision test by the NavDataConfig AgentRadius (See Project Settings -> Navigation System).
	UPROPERTY(EditAnywhere, Category = Generation, Config, AdvancedDisplay)
	uint32 bUseAgentRadius: 1;

	// Whether to allow pathfinding outside of navigation bounds
	UPROPERTY(EditAnywhere, Category = Generation, Config, AdvancedDisplay)
	uint32 bUseExclusiveBounds: 1;
	
	// Add extra geometry to precisely ensure no pathfinding outside of navigation bounds (requires bounds be Axis Aligned Boxes)
	UPROPERTY(EditAnywhere, Category = Generation, Config, AdvancedDisplay, meta=(EditCondition="bUseExclusiveBounds"))
	uint32 bUsePreciseExclusiveBounds: 1;
	
	// When using RuntimeGeneration = Dynamic, whether to build once on BeginPlay. WARNING: large performance hit if used with small detail size or large scene.
	UPROPERTY(EditAnywhere, Category = Generation, Config, meta=(EditCondition="RuntimeGeneration == ERuntimeGenerationType::Dynamic"))
	uint32 bBuildOnBeginPlay: 1;
	
	// If not defined by a FlyingObject, use these pathfinding query settings.
	UPROPERTY(EditAnywhere, Category = Pathfinding, Config)
	FSVOQuerySettings DefaultQuerySettings;

	// How far (in leaf size units) to check around a target point for a free voxel to be used as a pathfinding target.
	// Default checks voxels one step away, but can be increased for compatibility with bUseAgentRadius.
	UPROPERTY(EditAnywhere, Category = Pathfinding, Config, meta=(ClampMin=1, UIMin=1, ClampMax=5, UIMax=5))
	uint8 ProjectPointExtent = 1;

#if WITH_EDITOR
	// Clears lines from viewport.
	UFUNCTION(CallInEditor, Category = Geometry, meta=(DevelopmentOnly))
    void ClearGeometryDrawing() const;
	
	// Draw triangles the flying navigation system will use to rasterise octree. Useful for checking functionality of bUseAgentRadius, which expands geometry to prevent clipping.
	UFUNCTION(CallInEditor, Category = Geometry, meta=(DevelopmentOnly))
    void DrawGeometry();
#endif // WITH_EDITOR
	
	// Whether to draw the Nodes of the Octree.
	UPROPERTY(EditAnywhere, Category = Display, meta=(EditCondition="bEnableDrawing", DisplayAfter="bEnableDrawing"))
	uint32 bDrawOctreeNodes: 1;

	// Whether to draw the SubNodes of the Octree.
	UPROPERTY(EditAnywhere, Category = Display, meta=(EditCondition="bEnableDrawing", DisplayAfter="bEnableDrawing"))
	uint32 bDrawOctreeSubNodes: 1;

	// Draw only SubNodes that overlap geometry.
	UPROPERTY(EditAnywhere, Category = Display, meta=(EditCondition="bEnableDrawing && bDrawOctreeSubNodes", DisplayAfter="bEnableDrawing"))
	uint32 bDrawOnlyOverlappedSubNodes: 1;

	// Colours nodes based on which are connected sections.
	UPROPERTY(EditAnywhere, Category = Display, meta=(EditCondition="bEnableDrawing && bDrawOctreeSubNodes || bDrawOctreeNodes || bDrawNeighbourConnections", DisplayAfter="bEnableDrawing"))
	uint32 bColourByConnected: 1;
	
	// Amount to shrink display voxel extent by, to make it easier to read (absolute measure).
	UPROPERTY(EditAnywhere, Category = Display, meta=(EditCondition="bEnableDrawing && bDrawOctreeSubNodes || bDrawOctreeNodes", ClampMin = "0", DisplayAfter="bEnableDrawing"))
	float NodeMargin;

	// Thickness of wire in octree visualisation, relative to box size. 1.0 is a solid box. WARNING: Don't use as a slider because the viewport can lag if the octree is built with a small detail size. Enter a discrete value instead.
	UPROPERTY(EditAnywhere, Category = Display, meta=(EditCondition="bEnableDrawing && bDrawOctreeSubNodes || bDrawOctreeNodes", ClampMin = "0", UIMin = "0", ClampMax = "1", UIMax = "1", DisplayAfter="bEnableDrawing"))
	float WireThickness;
	
	// Whether to draw neighbour lines between Nodes and SubNodes. WARNING: Uses slow drawing, do not use for high resolution visualisation.
	UPROPERTY(EditAnywhere, Category = Display, meta=(EditCondition="bEnableDrawing", DisplayAfter="bEnableDrawing"))
	uint32 bDrawNeighbourConnections: 1;

	// Whether to only draw node connections for performance and clarity. Disable to show all connections, but not recommended.
	UPROPERTY(EditAnywhere, Category = Display, meta=(EditCondition="bEnableDrawing && bDrawNeighbourConnections", DisplayAfter="bEnableDrawing"))
	uint32 bDrawSimplifiedConnections: 1;

	// Radius of the sphere used to indicate each node centre.
	UPROPERTY(EditAnywhere, Category = Display, meta=(EditCondition="bEnableDrawing && bDrawNeighbourConnections", DisplayAfter="bEnableDrawing"))
	float NodeCentreRadius;

#if WITH_EDITORONLY_DATA
	// Draw NavPath when queried. Editor only.
	UPROPERTY(EditAnywhere, Category = Display, meta=(DisplayAfter="bEnableDrawing"))
	uint32 bDrawDebugPaths: 1;

	// Allow octree visualisation in PIE or Game World. WARNING: Causes performance hit when dynamically rebuilding
	UPROPERTY(EditAnywhere, Category = Display, meta=(DisplayAfter="bEnableDrawing"))
	uint32 bAllowDrawingInGameWorld: 1;
#endif // WITH_EDITORONLY_DATA
	
	// Event to broadcast when finished building
	UPROPERTY(BlueprintAssignable, Category = Navigation)
	FFlyingNavGenerationFinishedEvent OnFlyingNavGenerationFinished;

#if WITH_EDITOR
	// Clear cached Navigation Data.
	UFUNCTION(CallInEditor, Category = Generation)
    void ClearNavigationData();
#endif // WITH_EDITOR

	// Rebuild cached Navigation Data. Requires Update Navigation Automatically to be enabled in Editor Preferences (otherwise, use Build->Build Paths)
	UFUNCTION(BlueprintCallable, CallInEditor, Category = Generation)
    void RebuildNavigationData() { RebuildAll(); }

	// Cancels rebuild of cached Navigation Data
	UFUNCTION(BlueprintCallable, CallInEditor, Category = Generation, meta=(KeyWords="Cancel"))
    void StopRebuild() { CancelBuild(); }

	// Checks if navigation data is available for pathfinding or raycasting
	UFUNCTION(BlueprintCallable, Category = Generation)
    bool IsNavigationDataBuilt() const { return SVOData->bValid; }

	// Returns voxel size of currently built navigation data. Returns 0 if not built
	UFUNCTION(BlueprintCallable, Category = Generation)
    float CurrentlyBuiltVoxelSize() const { return SVOData->bValid ? SVOData->SubNodeSideLength : 0.f; }

#if WITH_EDITORONLY_DATA
	virtual void UpdateSubdivisions(const float OctreeSideLength);
#endif // WITH_EDITOR
	
protected:
	//~ Begin AActor overrides
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;
	static void AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector);
	//~ End AActor overrides

	FNavPathQueryDelegate PathfindingResultDelegate;
	// Called on completion of async pathfinding
	void AsyncPathfindingDelegate(const uint32 QueryID, const ENavigationQueryResult::Type Result, const FNavPathSharedPtr NavPath);

public:
	//~ Begin UObject overrides
	virtual void Serialize(FArchive& Ar) override;
	
	virtual void PostRegisterAllComponents() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty( struct FPropertyChangedEvent& PropertyChangedEvent) override;
	//~ End UObject overrides

	// FlyingNavigationData instances are dynamically spawned and should not be copied
	virtual bool ShouldExport() override { return false; }
#endif // WITH_EDITOR

	// Builds Data on game thread (for debugging purposes). Requires RuntimeGeneration = Dynamic
	void SyncBuild();

	/** 
	* Rebuild separate thread.  Calling again while building will be ignored
	* 
	* @param WorldContextObject	World context.
	* @param LatentInfo 	The latent action.
	* @param NavData		FlyingNavigationData to rebuild
	*/
	UFUNCTION(BlueprintCallable, Category="Utilities|FlowControl", meta=(Latent, WorldContext="WorldContextObject", LatentInfo="LatentInfo"))
    void RebuildFlyingNavigation(UObject* WorldContextObject, FLatentActionInfo LatentInfo);
	
#if WITH_EDITOR
	void DrawDebug() const;
#endif // WITH_EDITOR
	
protected:
	/** Create a new FFlyingNavigationDataGenerator instance. */
	virtual FFlyingNavigationDataGenerator* CreateGeneratorInstance();

	class UOctreeRenderingComponent* GetRenderingComponent() const;
	
public:
	
	//~ Begin ANavigationData overrides
	virtual void CleanUp() override;
	virtual bool NeedsRebuild() const override;
	virtual bool SupportsRuntimeGeneration() const override;
	virtual bool SupportsStreaming() const override;
	virtual void OnNavigationBoundsChanged() override;
	virtual void ConditionalConstructGenerator() override;

	// Random point in navigable space
	virtual FNavLocation GetRandomPoint(FSharedConstNavQueryFilter Filter = nullptr, const UObject* Querier = nullptr) const override;
	// Random *reachable* point in radius
	virtual bool GetRandomReachablePointInRadius(const FVector& Origin, float Radius, FNavLocation& OutResult, FSharedConstNavQueryFilter Filter = nullptr, const UObject* Querier = nullptr) const override;
	// Random point in radius
	virtual bool GetRandomPointInNavigableRadius(const FVector& Origin, float Radius, FNavLocation& OutResult, FSharedConstNavQueryFilter Filter = nullptr, const UObject* Querier = nullptr) const override;

	// Projects point to neighbouring empty space if point is inside blocked area. 'Extent' has been renamed to ConnectedComponentPoint, for sorting projection points based on a position
	virtual bool ProjectPoint(const FVector& Point, FNavLocation& OutLocation, const FVector& ConnectedComponentPoint, FSharedConstNavQueryFilter Filter = nullptr, const UObject* Querier = nullptr) const override;
	virtual bool IsNodeRefValid(NavNodeRef NodeRef) const override;

	/** Project batch of points. 'Extent' has been renamed to ConnectedComponentPoint, for sorting projection points based on a position */
	virtual void BatchProjectPoints(TArray<FNavigationProjectionWork>& Workload, const FVector& ConnectedComponentPoint, FSharedConstNavQueryFilter Filter = nullptr, const UObject* Querier = nullptr) const override;

	/** Project batch of points using shared search filter. This version is not requiring user to pass in Extent, 
	 *	and is instead relying on FNavigationProjectionWork.ProjectionLimit.
	 *	@note function will assert if item's FNavigationProjectionWork.ProjectionLimit is invalid */
	virtual void BatchProjectPoints(TArray<FNavigationProjectionWork>& Workload, FSharedConstNavQueryFilter Filter = nullptr, const UObject* Querier = nullptr) const override;

	// NOTE: Path cost and path length are the same.
	virtual ENavigationQueryResult::Type CalcPathCost(const FVector& PathStart, const FVector& PathEnd, FPathLengthType& OutPathCost, FSharedConstNavQueryFilter Filter = nullptr, const UObject* Querier = nullptr) const override;
	virtual ENavigationQueryResult::Type CalcPathLength(const FVector& PathStart, const FVector& PathEnd, FPathLengthType& OutPathLength, FSharedConstNavQueryFilter QueryFilter = nullptr, const UObject* Querier = nullptr) const override;
	virtual ENavigationQueryResult::Type CalcPathLengthAndCost(const FVector& PathStart, const FVector& PathEnd, FPathLengthType& OutPathLength, FPathLengthType& OutPathCost, FSharedConstNavQueryFilter QueryFilter = nullptr, const UObject* Querier = nullptr) const override;

	// Checks if a given NavNodeRef (converted to FSVOLink) contains the given location.
	virtual bool DoesNodeContainLocation(NavNodeRef NodeRef, const FVector& WorldSpaceLocation) const override;

	virtual UPrimitiveComponent* ConstructRenderingComponent() override;

	/** Returns bounding box for the flying volume. */
	virtual FBox GetBounds() const override { return GetFlyingBounds(); }

	/** Called on world origin changes **/
	virtual void ApplyWorldOffset(const FVector& InOffset, bool bWorldShift) override;

	virtual void BatchRaycast(TArray<FNavigationRaycastWork>& Workload, FSharedConstNavQueryFilter QueryFilter, const UObject* Querier = nullptr) const override;

	// TODO: NavAreas not supported
	//virtual void OnNavAreaAdded(const UClass* NavAreaClass, int32 AgentIndex) override {}
	//virtual void OnNavAreaRemoved(const UClass* NavAreaClass) override {}
	
	// TODO: Level streaming
	//virtual void OnStreamingLevelAdded(ULevel* InLevel, UWorld* InWorld) override;
	//virtual void OnStreamingLevelRemoved(ULevel* InLevel, UWorld* InWorld) override;
	//virtual void AttachNavMeshDataChunk(URecastNavMeshDataChunk& NavDataChunk);
	//virtual void DetachNavMeshDataChunk(URecastNavMeshDataChunk& NavDataChunk);
	
	/** @return Navmesh data chunk that belongs to this actor */
	// URecastNavMeshDataChunk* GetNavigationDataChunk(ULevel* InLevel) const;

	// Returns bounding box for the whole flying volume.
	FBox GetFlyingBounds() const;

	// Marks the render state dirty (must be run on the game thread)
	void UpdateDrawing();

	// Creates a task to be executed on GameThread calling UpdateDrawing
	void RequestDrawingUpdate(bool bForce = false);

	// Called from generator once building has completed
	void OnOctreeGenerationFinished();

	// Returns point more likely to be in unblocked space. Prefers unblocked space accessible by ConnectedPoint
	FVector ModifyPathEndpoints(const FVector& TargetPoint, const FVector& ConnectedPoint, const float AgentHalfHeight) const;

	// Pathfinding overrides

	// If Query.Owner implements FFlyingObjectInterface, then DefaultQuerySettings will be overridden
	static FPathFindingResult FindPath(const FNavAgentProperties& AgentProperties, const FPathFindingQuery& Query);
	// Precomputed in octree building
	static bool TestPath(const FNavAgentProperties& AgentProperties, const FPathFindingQuery& Query, int32* NumVisitedNodes = nullptr);

	// Fast raycast against the octree
	static bool OctreeRaycast(const ANavigationData* Self, const FVector& RayStart, const FVector& RayEnd, FVector& HitLocation, FSharedConstNavQueryFilter QueryFilter, const UObject* Querier);

	// Fast raycast against the octree
	UFUNCTION(BlueprintCallable, Category=Raycast)
	bool OctreeRaycast(const FVector& RayStart, const FVector& RayEnd, FVector& HitLocation) const;
	
	virtual uint32 LogMemUsed() const override;

	// SVO Data accessors, make sure to use SVODataLock if using threading
	FORCEINLINE FSVOData& GetSVOData() { return SVOData.Get(); }
	FORCEINLINE const FSVOData& GetSVOData() const { return SVOData.Get(); }
	FORCEINLINE FSVOData& GetBuildingSVOData() { return BuildingSVOData.Get(); }
	FORCEINLINE const FSVOData& GetBuildingSVOData() const { return BuildingSVOData.Get(); }

	// Read-Write lock on SVOData
	mutable FRWLock SVODataLock;

	// Just for neighbour information
	FORCEINLINE const FSVOGraph* GetNeighbourGraph() const { return NeighbourGraph.Get(); }
	// Use this Navigation Graph on the game thread
	FORCEINLINE FSVOPathfindingGraph* GetSyncPathfindingGraph() const { return SyncPathfindingGraph.Get(); }
	// Use this Navigation Graph on any thread other than the game thread. Remember to lock AsyncGraphCriticalSection when using
	FORCEINLINE FSVOPathfindingGraph* GetAsyncPathfindingGraph() const { return AsyncPathfindingGraph.Get(); }
	
	mutable FCriticalSection AsyncGraphCriticalSection;
	
	// Finds the Side length of the SVO cube
	float GetOctreeSideLength() const;

protected:
	// Navigation data to use for pathfinding 
	FSVODataRef SVOData;
	// Navigation data currently being built (for dynamic generation)
	FSVODataRef BuildingSVOData;

	// Swap SVOData and BuildingSVOData
	void UpdateCurrentNavData();

	// SVOData versioning.
	uint32 SVODataVersion;

	// Neighbour calculations (thread safe)
	TUniquePtr<const FSVOGraph> NeighbourGraph;
	// Navigation Graph for Game Thread operations
	TUniquePtr<FSVOPathfindingGraph> SyncPathfindingGraph;
	// Navigation Graph for Async queries
	TUniquePtr<FSVOPathfindingGraph> AsyncPathfindingGraph;
	
	// Casted reference to Nav Generator
	TSharedPtr<FFlyingNavigationDataGenerator, ESPMode::ThreadSafe> FlyingNavGenerator;

	// Paths that are currently being calculated
	TMap<uint32, ENavPathUpdateType::Type> CurrentlyCalculatingPaths;

	// Triggered when async task finishes. Often nullptr. Only used for EndPlay behaviour.
	FEvent* AsyncTaskCompleteEvent;
	FThreadSafeBool bDisablePathfinding;
	mutable FThreadSafeCounter NumCalculatingAsyncPaths;

#if WITH_EDITORONLY_DATA
	// Line batcher for drawing debug paths
	UPROPERTY(Transient)
	TObjectPtr<class UPathBatchComponent> PathBatcher;

	friend FFlyingNavigationPath;
#endif // WITH_EDITORONLY_DATA
	
	// Needs to be const for using in a reference to FPathFindingQuery.NavData
	void IncrementCalculatingPaths() const { NumCalculatingAsyncPaths.Increment(); }
	void DecrementCalculatingPaths() const
	{
		NumCalculatingAsyncPaths.Decrement();
		
		if (AsyncTaskCompleteEvent)
		{
			AsyncTaskCompleteEvent->Trigger();
		}
	}

	friend class UFlyingNavFunctionLibrary;
};
