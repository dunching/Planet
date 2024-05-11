// Copyright Ben Sutherland 2024. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AI/NavDataGenerator.h"
#include "FlyingNavigationData.h"
#include "FlyingNavSystemTypes.h"
#include "HAL/ThreadSafeBool.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
#include "Math/GenericOctree.h" // for FBoxCenterAndExtent

class UNavigationSystemV1;
struct FNavigationRelevantData;
class FSVOGeneratorTask;
class FFlyingNavigationDataGenerator;

/**
* Stores triangle soup for a navigation element
*/
struct FLYINGNAVSYSTEM_API FSVORawGeometryElement
{
	// Instance geometry, in unreal coords
	TArray<FCoord>		GeomCoords;
	TArray<int32>		GeomIndices;
	// Tight AABB around geometry element
	FBox Bounds;

	FSVORawGeometryElement(TArray<FCoord>&& InGeomCoords, TArray<int32>&& InGeomIndices, const FBox& InBounds):
		GeomCoords(MoveTemp(InGeomCoords)), GeomIndices(MoveTemp(InGeomIndices)), Bounds(InBounds)
	{ }

#if WITH_EDITOR
	// Draws triangle mesh
	void DrawElement(UWorld* World, const FColor& Colour) const
	{
		const int32 NumTris = GeomIndices.Num() / 3;
		for (int32 i = 0; i < NumTris; i++)
		{
			const FVector& V1 = *reinterpret_cast<const FVector*>(&(GeomCoords[3 * GeomIndices[3 * i + 0]]));
			const FVector& V2 = *reinterpret_cast<const FVector*>(&(GeomCoords[3 * GeomIndices[3 * i + 1]]));
			const FVector& V3 = *reinterpret_cast<const FVector*>(&(GeomCoords[3 * GeomIndices[3 * i + 2]]));

			// I'm fully aware that calling this from a separate thread is not supported, but it only crashes occasionally
			DrawDebugLine(World, V1, V2, Colour, true);
			DrawDebugLine(World, V2, V3, Colour, true);
			DrawDebugLine(World, V3, V1, Colour, true);
		}
	}
#endif // WITH_EDITOR
};

/**
* Rasterises subvolume against scene geometry
*/
class FLYINGNAVSYSTEM_API FRasteriseWorker final : public FNoncopyable
{
public:
	friend class FRasteriseWorkerTask;
	
	FRasteriseWorker(FSVOGenerator& ParentGenerator,
					const FBox& GenerationBounds,
					const int32 WorkerIdx,
					const int32 Divisions);
	
	void DoWork();

	void Cancel() { bCancelled = true; }

#if WITH_EDITOR
	void GatherAndDrawGeometry();
	void DrawGeometry();
#endif // WITH_EDITOR
	
protected:

	//----------------------------------------------------------------------//
	// SVO Generation
	//----------------------------------------------------------------------//

	/*
	* Deferred geometry gathering
	* Call PrepareGeometrySources() from main thread,
	* then call GatherGeometryFromSources() from separate thread
	*/
	void PrepareGeometrySources();
	void GatherGeometryFromSources();

	/*
	* Checks and adds geometry to RawGeometry
	*/
	void ValidateAndAppendGeometry(const FNavigationRelevantData& ElementData);
	void AppendCollisionData(const TNavStatArray<uint8>& RawCollisionData,
	                         const FBox& ElementBounds,
	                         const bool bConvertFromRecast,
	                         const TArray<FTransform>& PerInstanceTransforms = TArray<FTransform>());
	
	void DumpAsyncData();
	
	/*
	* Initial rasterisation at 8x detail size, at LayerOne
	*/ 
	void RasteriseLayerOne();
	/*
	* From list of LayerOne nodes, generates corresponding leaf nodes and rasterises them
	*/ 
	void RasteriseLeafLayer();

	/*
	* Checks single leaf node against scene geometry, fills in leaf's VoxelGrid
	*/
	void RasteriseLeafNode(FSVOLeafNode& Leaf,
	                       const FVector& LeafCentre,
	                       const FCoord SubNodeOffset,
	                       const FVector& SubNodeExtent);
	
	/*
	* Checks if a voxel intersects with any geometry saved in RawGeometry
	*/
	bool DoesVoxelOverlapGeometry(const FVector& VoxelCentre, const FVector VoxelExtent);
	
	/*
	* Checks if a voxel intersects with the given raw geometry element data (static)
	*/
	static bool DoesVoxelOverlapWithRawGeometryData(const FSVORawGeometryElement& Geometry,
	                                                const FVector& VoxelCentre,
	                                                const FVector& VoxelExtent);

	
	// Layer accessors: Layer 0 = Leaf Layer, uses different structure
	TArray<FSVOLayer>& GetLayers() const { return NavData->Layers; }
	FSVOLayer& GetLayer(const int32 Layer) const { return NavData->GetLayer(Layer); }
	FSVOLeafLayer& GetLeafLayer() const { return NavData->LeafLayer; }
	
	FThreadSafeBool bCancelled;
	bool ShouldAbort() const { return bCancelled; }

public:
	// Counts all memory currently used by worker
	uint32 GetAllocatedSize() const;

protected:
	
	// Total bounds
	FBox GenerationBounds;
	// Used for landscape slicing
	FBox GenerationBoundsExpandedForAgent;

	// Array of bounds to actually include
	TArray<FBoxCenterAndExtent> InclusionBounds;
	
	// Index of this worker task
	const int32 WorkerIdx;
	// How many times the total bounds cube is divided, based on number of workers
	const int32 Divisions;

	// Thread-safe references to geometry data
	TNavStatArray<TSharedRef<FNavigationRelevantData, ESPMode::ThreadSafe> > NavigationRelevantData;
	
	// Geometry to use to build SVO
	TArray<FSVORawGeometryElement> RawGeometry;
	
	// LayerOne morton codes to be rasterised at the leaf level
	TArray<morton_t> SortedMortonCodes;
	
	FSVOLeafLayer GeneratedLeafLayer;
	FSVOLayer GeneratedLayerOne;

	FSVOGenerator& ParentGeneratorRef; // Reference to ParentGenerator. If it gets deleted, so does FRasteriseWorker
	FSVODataRef NavData;

	TWeakObjectPtr<UNavigationSystemV1> NavSys;
};

/**
* FRunnable for FRasteriseWorker
*/
class FLYINGNAVSYSTEM_API FRasteriseWorkerTask final : public FRunnable
{
	TUniquePtr<FRasteriseWorker> RasteriseWorker;
	
	/** Thread to run the worker FRunnable on */
	TUniquePtr<FRunnableThread> Thread;
	
public:
	// Thread safe finish check
	FThreadSafeBool bFinished;

	FRasteriseWorkerTask(FSVOGenerator& ParentGenerator,
						 const FBox& GenerationBounds,
						 const int32 WorkerIdx,
						 const int32 Divisions):
        RasteriseWorker(
        	new FRasteriseWorker(
		        ParentGenerator,
		        GenerationBounds,
		        WorkerIdx,
		        Divisions)
        ),
        Thread(FRunnableThread::Create(this, *FString::Printf(TEXT("FRasteriseWorkerTask %d"), WorkerIdx), 0, TPri_Normal))
	{}

	bool IsFinished() const { return bFinished; }
	
	// Block thread until completion
	void EnsureCompletion() const { Thread->WaitForCompletion(); }
	
	// Begin FRunnable interface.
	virtual uint32 Run() override
	{
		bFinished = false;
		RasteriseWorker->DoWork();
		bFinished = true;
	
		return 0;
	}
	
	void Cancel() { RasteriseWorker->Cancel(); }
	
	virtual void Stop() override { Cancel(); }
	// End FRunnable interface

	// Access output
	FSVOLeafLayer&	GeneratedLeafLayer() const { return RasteriseWorker->GeneratedLeafLayer; }
	FSVOLayer&		GeneratedLayerOne()  const { return RasteriseWorker->GeneratedLayerOne; }

	
	uint32 GetAllocatedSize() const
	{
		return RasteriseWorker ? RasteriseWorker->GetAllocatedSize() : 0;
	}
};

typedef TSharedRef<FRasteriseWorkerTask, ESPMode::ThreadSafe> FWorkerTaskRef;

/**
* Spawns rasterise workers and generates octree on separate thread
*/
class FLYINGNAVSYSTEM_API FSVOGenerator final : public FNoncopyable
{
	friend FSVOGeneratorTask;
	friend FRasteriseWorker;
	
public:
	explicit FSVOGenerator(FFlyingNavigationDataGenerator& ParentGenerator);
	~FSVOGenerator();
	
	//----------------------------------------------------------------------//
	// SVO Generation
	//----------------------------------------------------------------------//

	/*
	 * Perform rasterisation
	 */
	// Called from main thread, possibly blocking until all workers have been dispatched
	void RasteriseTick(const bool bBlockThread = false);

	// Called from async thread
	void CollateRasterData();
	
	/*
	 * Assuming that Layer-1 has been generated, generates the next layer of nodes
	 */
	void GenerateSVOLayer(const int32 LayerNum);
	
	/*
     * Finds index of node in layer from morton code
	 */
	int32 FindNodeInLayer(const int32 LayerNum, const morton_t& NodeMorton) const;
	
	/*
	 * Generate neighbour links for one layer, given GetLayer(Layer) is valid
	 */
	void GenerateNeighbourLinks(const int32 LayerNum);
	
	/*
	 * Exclude nodes outside bounds (if DestFlyingNavData->bUseExclusiveBounds == true)
	 */
	bool ShouldNodeBeExcluded(const FSVOLink NodeLink);
	void ExcludeNodesOutsideBounds();
	
	/*
	 * Generate neighbour links for one layer, given GetLayer(Layer) is valid
	 */
	void FindConnectedComponents();
	
	void AddPlaceholderRoot();

	//----------------------------------------------------------------------//
	// Async
	//----------------------------------------------------------------------//
	void DoWork();

	void BuildAsync();
	
	void DumpAsyncData();

	void OnRasteriserWorkerFinished();

	void Cancel()
	{
		bCancelled = true;

		{
			// Stop all WorkerTasks
			FScopeLock Lock(&WorkerTaskLock);
			for (int32 i = WorkerTasks.Num()-1; i >= 0; i--)
			{
				WorkerTasks[i]->Cancel();
			}
		}

		// So CollateRasterData() doesn't block
		AllWorkersDispatchedEvent->Trigger();
	}
	bool ShouldAbort() const { return bCancelled; }

	//----------------------------------------------------------------------//
	// Helpers
	//----------------------------------------------------------------------//

	class UWorld* GetWorld() const { return DestFlyingNavData ? DestFlyingNavData->GetWorld() : nullptr; }

	// Layer accessors: Layer 0 = Leaf Layer, uses different structure
	TArray<FSVOLayer>& GetLayers() const { return SVOData->Layers; }
	FSVOLayer& GetLayer(const int32 Layer) const { return SVOData->GetLayer(Layer); }
	FSVOLeafLayer& GetLeafLayer() const { return SVOData->LeafLayer; }

	// Convenience accessors
	FCoord GetSideLengthForLayer(const int32 Layer) const { return SVOData->GetSideLengthForLayer(Layer); }
	FCoord GetOffset(const FCoord SystemSideLength, const FCoord VoxelSideLength) const { return SVOData->GetOffset(SystemSideLength, VoxelSideLength); }
	FCoord GetNodeOffset(const int32 Layer) const { return SVOData->GetNodeOffsetForLayer(Layer); }
	FCoord GetSubNodeOffset() const { return SVOData->GetSubNodeOffset(); }

	//----------------------------------------------------------------------//
	// Debug Drawing
	//----------------------------------------------------------------------//
#if WITH_EDITOR
	static void DebugDrawSubNodes(UWorld* World, const FSVOLink LeafLink, const FVector& LeafCentre, const FSVOData& NavigationData);
	static void DebugDrawNeighbours(UWorld* World, const FSVOLink NodeLink, const FVector& VoxelCentre, const FSVOData& NavigationData);
	static void DebugDraw(UWorld* World, const FSVOData& NavigationData);
#endif // WITH_EDITOR

	uint32 GetAllocatedSize() const
	{
		uint32 MemUsed = sizeof(FSVOGenerator) + InclusionBounds.GetAllocatedSize();
		
		{
			FScopeLock Lock(&WorkerTaskLock);
			for (const FWorkerTaskRef& WorkerTask : WorkerTasks)
			{
				MemUsed += WorkerTask->GetAllocatedSize();
			}
		}
		
		return MemUsed;
	}
	
protected:
	//----------------------------------------------------------------------//
	// Properties
	//----------------------------------------------------------------------//

	// Triggered when a FRasteriseWorker thread finishes
	FEvent* WorkerFinishedEvent;
	// Required to make sure RasteriseTick() runs before CollateRasterData()
	FEvent* AllWorkersDispatchedEvent;
	FThreadSafeBool bAllWorkersDispatched;
	
	/** Thread safe cancel flag */
	FThreadSafeBool bCancelled;

	/** Thread safe thread counters */
	FThreadSafeCounter NumRunningThreads;
	FThreadSafeCounter NumRemainingTasks;

	/** Flying nav data that owns this generator */
	AFlyingNavigationData* DestFlyingNavData;

	// Shared reference to NavData
	FSVODataRef SVOData;

	// Rasterise worker data
	TArray<FWorkerTaskRef> WorkerTasks;
	mutable FCriticalSection WorkerTaskLock;
	
	int32 NumThreadSubdivisions;
	int32 Divisions;
	int32 NumThreads;
	
	// Build Values:
	/** Total bounding box that includes all volumes, in unreal units. */
	FBox TotalBounds;
	TArray<FBox> InclusionBounds;
	// Exported geometry for rasterising precise bounds
	TNavStatArray<uint8> PreciseBoundsCollisionData;

	uint32 bMultithreaded: 1;
	int32 MaxThreads;
	uint32 bUseAgentRadius: 1;
	
	// Thread safe finish check
	FThreadSafeBool bFinished;
};

/**
 * FRunnable for FSVOGenerator
 */
class FLYINGNAVSYSTEM_API FSVOGeneratorTask : public FRunnable
{
protected:
	TUniquePtr<FSVOGenerator> SVOGenerator;
	
	// Thread to run the worker FRunnable on
	TUniquePtr<FRunnableThread> Thread;

public:
	explicit FSVOGeneratorTask(FFlyingNavigationDataGenerator& NavDataGenerator):
		SVOGenerator(new FSVOGenerator(NavDataGenerator)),
        Thread(FRunnableThread::Create(this, TEXT("FSVOGeneratorTask"), 0, TPri_Normal))
	{}

	bool IsFinishedRasterising() const { return SVOGenerator->bAllWorkersDispatched; }
	bool IsFinished() const { return SVOGenerator->bFinished; }

	// Makes sure this thread has stopped properly
	void EnsureCompletion() const { Thread->WaitForCompletion(); }

	int32 GetNumRunningTasks() const
	{
		return SVOGenerator->NumRunningThreads.GetValue();
	}
	int32 GetNumRemainingTasks() const
	{
		return SVOGenerator->NumRemainingTasks.GetValue();
	}
	
	// Begin FRunnable interface.
	virtual uint32 Run() override
	{
		SVOGenerator->DoWork();
		return 0;
	}

	virtual void Stop() override
	{
		SVOGenerator->Cancel();
	}
	// End FRunnable interface

	void RasteriseTick(const bool bBlockThread = false)
	{
		SVOGenerator->RasteriseTick(bBlockThread);
	}
	
	uint32 GetAllocatedSize() const
	{
		return SVOGenerator ? SVOGenerator->GetAllocatedSize() : 0;
	}
};

/**
 * Class that handles generation of the Sparse Voxel Octree (SVO) for flying navigation.
 */
class FLYINGNAVSYSTEM_API FFlyingNavigationDataGenerator: public FNavDataGenerator
{
	friend AFlyingNavigationData;
	friend FSVOGenerator;
	
public:
	FFlyingNavigationDataGenerator(AFlyingNavigationData& InDestFlyingNavData);
	
private:
	/** Prevent copying. */
	// ReSharper disable once CppPossiblyUninitializedMember
	FFlyingNavigationDataGenerator(FFlyingNavigationDataGenerator const& NoCopy) { check(false); }
	FFlyingNavigationDataGenerator& operator=(FFlyingNavigationDataGenerator const& NoCopy) { check(false); return *this; }
	
public:
	// Build on thread
	void SyncBuild();
	
	//~ Begin FNavDataGenerator Interface
	
	/** Rebuilds all known navigation data */
	virtual bool RebuildAll() override;

	/** Blocks until build is complete */
	virtual void EnsureBuildCompletion() override;

	/** Cancels build, may block until current running async tasks are finished */
	virtual void CancelBuild() override;

	/** Runs every tick to check up on async tasks  */
	virtual void TickAsyncBuild(float DeltaSeconds) override;
	
	virtual void OnNavigationBoundsChanged() override;

	/** Asks generator to update navigation affected by DirtyAreas */
	virtual void RebuildDirtyAreas(const TArray<FNavigationDirtyArea>& DirtyAreas) override;

	virtual bool IsBuildInProgressCheckDirty() const override;
	
	/** Returns number of remaining tasks till build is complete */
	virtual int32 GetNumRemaningBuildTasks() const override;

	/** Returns number of currently running tasks */
	virtual int32 GetNumRunningBuildTasks() const override;
	
	//----------------------------------------------------------------------//
	// debug
	//----------------------------------------------------------------------//
	virtual uint32 LogMemUsed() const override;

#if ENABLE_VISUAL_LOG
	virtual void GrabDebugSnapshot(struct FVisualLogEntry* Snapshot,
									const FBox& BoundingBox,
									const FName& CategoryName,
									ELogVerbosity::Type Verbosity) const override;
#endif // ENABLE_VISUAL_LOG
	
	//~ End FNavDataGenerator Interface

	void GatherAndDrawGeometry();
	
	/*
	* Adds all registered navigation bounds to TotalBounds and InclusionBounds 
	*/
	void UpdateNavigationBounds();

	void ClearNavigationData();
	
	const FBox& GetTotalBounds() const { return TotalBounds; }
	const TNavStatArray<FBox>& GetInclusionBounds() const { return InclusionBounds; }

	bool bIsPendingBuild;
	bool bIsBuilding;

	class UWorld* GetWorld() const { return DestFlyingNavData ? DestFlyingNavData->GetWorld() : nullptr; }
	
protected:
	//--- accessors --- //
	FSVOData& GetBuildingNavigationData() const { return DestFlyingNavData->GetBuildingSVOData(); }

	//----------------------------------------------------------------------//
	// Properties
	//----------------------------------------------------------------------//

	/** Total bounding box that includes all volumes, in unreal units. */
	FBox TotalBounds;
	
	/** Bounding geometry definition. */
	TNavStatArray<FBox> InclusionBounds;

	/** Flying nav data that owns this generator */
	AFlyingNavigationData* DestFlyingNavData;

	/** Pointer to async task */
	TUniquePtr<FSVOGeneratorTask> GeneratorTask;
};
