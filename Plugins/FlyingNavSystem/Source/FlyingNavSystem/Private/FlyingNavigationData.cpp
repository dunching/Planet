// Copyright Ben Sutherland 2024. All rights reserved.

#include "FlyingNavigationData.h"
#include "FlyingNavFunctionLibrary.h"
#include "FlyingNavigationDataGenerator.h"
#include "FlyingNavSystemModule.h"
#include "FlyingNavSystemTypes.h"
#include "FlyingObjectInterface.h"
#include "LatentActions.h"
#include "NavigationSystem.h"
#include "OctreeRenderingComponent.h"
#include "SVOGraph.h"
#include "SVORaycast.h"
#include "Engine/Engine.h"
#include "Engine/LatentActionManager.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Misc/ScopeExit.h"
#include "VisualLogger/VisualLogger.h"

#if WITH_EDITOR
#include "PathBatchComponent.h"
#include "DrawDebugHelpers.h"
#endif // WITH_EDITOR

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlyingNavigationData)

// Invalid FSVOLink Configuration, only Layer 0 nodes have SubNodes
// NULL_LINK = 0x10000001 = 268435457
const FSVOLink FSVOLink::NULL_LINK = FSVOLink(1, 0, 1); 

//----------------------------------------------------------------------//
// FFlyingNavigationPath Implementation
//----------------------------------------------------------------------//
FFlyingNavigationPath::FFlyingNavigationPath()
	: bAsyncObservationUpdate(false),
	  bCurrentlyBeingUpdated(false),
	  QueryResult(ENavigationQueryResult::Invalid),
	  DrawIndex(INDEX_NONE)
{
	PathType = Type;
}

FFlyingNavigationPath::~FFlyingNavigationPath()
{
#if WITH_EDITORONLY_DATA
	AFlyingNavigationData* NavData = Cast<AFlyingNavigationData>(NavigationDataUsed.Get());

	if (NavData && NavData->PathBatcher)
	{
		// Remove path when deleted
		NavData->PathBatcher->RemovePath(DrawIndex);
	}
#endif // WITH_EDITORONLY_DATA
}

void FFlyingNavigationPath::ApplyFlags(int32 NavDataFlags)
{
	if (NavDataFlags & EFlyingNavPathFlags::AsyncObservationUpdate)
	{
		bAsyncObservationUpdate = true;
	}
}

const FNavPathType FFlyingNavigationPath::Type;

//----------------------------------------------------------------------//
// FRebuildAction Implementation
//----------------------------------------------------------------------//
class FRebuildAction final : public FPendingLatentAction
{
public:
	FName ExecutionFunction;
	int32 OutputLink;
	FWeakObjectPtr CallbackTarget;
	TSharedPtr<FNavDataGenerator, ESPMode::ThreadSafe> Generator;
	bool bFinished;

	FRebuildAction(AFlyingNavigationData* NavData, const FLatentActionInfo& LatentInfo):
        ExecutionFunction(LatentInfo.ExecutionFunction),
        OutputLink(LatentInfo.Linkage),
        CallbackTarget(LatentInfo.CallbackTarget),
        bFinished(true)
	{
		const UWorld* World = NavData->GetWorld();
		if (NavData->SupportsRuntimeGeneration() || (World && !World->IsGameWorld()))
		{
			NavData->RebuildAll();
			if (NavData->GetGenerator())
			{
				Generator = NavData->GetGenerator()->AsShared();
				bFinished = false;
			}
		}
	}

	virtual void UpdateOperation(FLatentResponse& Response) override
	{
		if (Generator)
		{
#if ENGINE_MINOR_VERSION >= 26 || UE5
			bFinished = !Generator->IsBuildInProgressCheckDirty();
#else
			bFinished = !Generator->IsBuildInProgress(true);
#endif
		}
		Response.FinishAndTriggerIf(bFinished, ExecutionFunction, OutputLink, CallbackTarget);
	}

#if WITH_EDITOR
	// Returns a human readable description of the latent operation's current state
	virtual FString GetDescription() const override
	{
		return FText::Format(NSLOCTEXT("FRebuildAction", "RebuildActionNavDataFmt", "Finished: {})"), FText::FromString(bFinished ? TEXT("True") : TEXT("False"))).ToString();
	}
#endif // WITH_EDITOR
};

//----------------------------------------------------------------------//
// AFlyingNavigationData Implementation
//----------------------------------------------------------------------//

AFlyingNavigationData::AFlyingNavigationData():
	MaxDetailSize(256.f),
#if WITH_EDITORONLY_DATA
	Subdivisions(MIN_LAYERS),
	ActualVoxelSize(256.f),
	bCurrentlyBuilt(false),
#endif
	bMultithreaded(true),
	ThreadSubdivisions(EThreadSubdivisions::One),
	MaxThreads(FPlatformMisc::NumberOfCoresIncludingHyperthreads()-1),
	bUseAgentRadius(false),
	bUseExclusiveBounds(false),
	bUsePreciseExclusiveBounds(false),
	bBuildOnBeginPlay(false),
	bDrawOctreeNodes(false),
	bDrawOctreeSubNodes(true),
	bDrawOnlyOverlappedSubNodes(true),
	bColourByConnected(true),
	NodeMargin(0.f),
	WireThickness(0.2f),
	bDrawNeighbourConnections(false),
	bDrawSimplifiedConnections(true),
	NodeCentreRadius(100.f),
#if WITH_EDITORONLY_DATA
	bDrawDebugPaths(false),
	bAllowDrawingInGameWorld(false),
#endif
	SVOData(new FSVOData),
	BuildingSVOData(new FSVOData),
	SVODataVersion(SVODATA_VER_LATEST),
	AsyncTaskCompleteEvent(nullptr),
	bDisablePathfinding(false)
{
	bEnableDrawing = true;
	
	if (HasAnyFlags(RF_ClassDefaultObject) == false)
	{
		// Hierarchical query doesn't mean anything at the moment
		FindPathImplementation = FindPath;
		FindHierarchicalPathImplementation = FindPath;

		TestPathImplementation = TestPath;
		TestHierarchicalPathImplementation = TestPath;
		
		RaycastImplementation = OctreeRaycast;
		
		// Bind async pathfinding delegate
		PathfindingResultDelegate.BindUObject(this, &AFlyingNavigationData::AsyncPathfindingDelegate);
		
		// Setup Navigation data storage and pathfinding objects
		NeighbourGraph = MakeUnique<const FSVOGraph>(SVOData.Get());
		SyncPathfindingGraph = MakeUnique<FSVOPathfindingGraph>(*NeighbourGraph);
		AsyncPathfindingGraph = MakeUnique<FSVOPathfindingGraph>(*NeighbourGraph);
		
		// Dummy filter
		DefaultQueryFilter->SetFilterType<FFlyingQueryFilter>();
		
#if WITH_EDITORONLY_DATA
		// Create line batcher component
        PathBatcher = CreateEditorOnlyDefaultSubobject<UPathBatchComponent>(TEXT("FlyingNavPathBatcher"), true);

		if (PathBatcher)
		{
			PathBatcher->bCalculateAccurateBounds = false;
		}
#endif
	}
}

void AFlyingNavigationData::AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector)
{
#if WITH_EDITOR
	AFlyingNavigationData* This = CastChecked<AFlyingNavigationData>(InThis);
	Collector.AddReferencedObject(This->PathBatcher, This);
#endif // WITH_EDITOR
	
	Super::AddReferencedObjects(InThis, Collector);
}

#if WITH_EDITOR
void AFlyingNavigationData::ClearNavigationData()
{
	{
		FRWScopeLock Lock(SVODataLock, SLT_Write);
		SVOData->Clear();
	}
	
	bCurrentlyBuilt = SVOData->bValid;
	RequestDrawingUpdate();
}
#endif // WITH_EDITORONLY_DATA

#if WITH_EDITOR
void AFlyingNavigationData::ClearGeometryDrawing() const
{
	FlushPersistentDebugLines(GetWorld());
}

void AFlyingNavigationData::DrawGeometry()
{
	ConditionalConstructGenerator(); //recreate generator
	
	if (NavDataGenerator.IsValid())
	{
		FlyingNavGenerator->GatherAndDrawGeometry();
	}
}
#endif // WITH_EDITOR

#if WITH_EDITORONLY_DATA
void AFlyingNavigationData::UpdateSubdivisions(const float OctreeSideLength)
{
	const int32 NumThreadSubdivisions = FlyingNavSystem::GetThreadSubdivisions(ThreadSubdivisions, bMultithreaded);
	Subdivisions = FlyingNavSystem::GetNumLayers(OctreeSideLength, MaxDetailSize, NumThreadSubdivisions);
	ActualVoxelSize = OctreeSideLength / static_cast<float>(1 << Subdivisions);

	if (Subdivisions != SVOData->GetSubdivisions())
	{
		bCurrentlyBuilt = false;
	} else
	{
		bCurrentlyBuilt = SVOData->bValid;
	}
}
#endif // WITH_EDITORONLY_DATA

void AFlyingNavigationData::BeginPlay()
{
	Super::BeginPlay();
	
	if (SupportsRuntimeGeneration() && bBuildOnBeginPlay)
	{
		RebuildAll();
	}
}

void AFlyingNavigationData::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// PROBLEM: Async pathfinding thread can take a while, and can be left behind on shutdown.
	// To stop it accessing dead memory, we need to pause shutdown before garbage collection.
	// AsyncTaskCompleteEvent is only used here to wait for queries to complete
	AsyncTaskCompleteEvent = FPlatformProcess::GetSynchEventFromPool(true);
	bDisablePathfinding = true;
	
	// HACK: Need pending async queries processed, should be quick enough
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (NavSys)
	{
		NavSys->Tick(0.f);
	}
	

	// Make sure all pathfinding queries are done to prevent crash
	while (NumCalculatingAsyncPaths.GetValue() > 0)
	{
		constexpr uint32 ASYNC_WAIT_INTERVAL = 1000; // Milliseconds
		
		AsyncTaskCompleteEvent->Wait(ASYNC_WAIT_INTERVAL);
	}

	// Cleanup the FEvent
	FGenericPlatformProcess::ReturnSynchEventToPool(AsyncTaskCompleteEvent);
	AsyncTaskCompleteEvent = nullptr;
}

void AFlyingNavigationData::TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction& ThisTickFunction)
{
	AActor::TickActor(DeltaTime, TickType, ThisTickFunction);

	// See ANavigationData::TickActor
	PurgeUnusedPaths();

	if (NextObservedPathsTickInSeconds >= 0.f)
	{
		NextObservedPathsTickInSeconds -= DeltaTime;
		if (NextObservedPathsTickInSeconds <= 0.f)
		{
			RepathRequests.Reserve(ObservedPaths.Num());

			for (int32 PathIndex = ObservedPaths.Num() - 1; PathIndex >= 0; --PathIndex)
			{
				if (ObservedPaths[PathIndex].IsValid())
				{
					FNavPathSharedPtr SharedPath = ObservedPaths[PathIndex].Pin();
					FNavigationPath* Path = SharedPath.Get();
					EPathObservationResult::Type Result = Path->TickPathObservation();
					switch (Result)
					{
					case EPathObservationResult::NoLongerObserving:
						ObservedPaths.RemoveAtSwap(PathIndex, 1, /*bAllowShrinking=*/false);
						break;

					case EPathObservationResult::NoChange:
						// do nothing
						break;

					case EPathObservationResult::RequestRepath:
						RepathRequests.Add(FNavPathRecalculationRequest(SharedPath, ENavPathUpdateType::GoalMoved));
						break;
					
					default:
						check(false && "unhandled EPathObservationResult::Type in ANavigationData::TickActor");
						break;
					}
				}
				else
				{
					ObservedPaths.RemoveAtSwap(PathIndex, 1, /*bAllowShrinking=*/false);
				}
			}

			if (ObservedPaths.Num() > 0)
			{
				NextObservedPathsTickInSeconds = ObservedPathsTickInterval;
			}
		}
	}

	if (RepathRequests.Num() > 0)
	{
		const float TimeStamp = GetWorldTimeStamp();

		constexpr int32 MaxProcessedRequests = 1000;

		// make a copy of path requests and reset (remove up to MaxProcessedRequests) from navdata's array
		// this allows storing new requests in the middle of loop (e.g. used by meta path corrections)

		TArray<FNavPathRecalculationRequest> WorkQueue(RepathRequests);
		if (WorkQueue.Num() > MaxProcessedRequests)
		{
			UE_VLOG(this, LogFlyingNavSystem, Error, TEXT("Too many repath requests! (%d/%d)"), WorkQueue.Num(), MaxProcessedRequests);

			WorkQueue.RemoveAt(MaxProcessedRequests, WorkQueue.Num() - MaxProcessedRequests);
			RepathRequests.RemoveAt(0, MaxProcessedRequests);
		}
		else
		{
			RepathRequests.Reset();
		}

		UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
		
		for (int32 Idx = 0; Idx < WorkQueue.Num(); Idx++)
		{
			FNavPathRecalculationRequest& RecalcRequest = WorkQueue[Idx];

			// check if it can be updated right now
			FNavPathSharedPtr PinnedPath = RecalcRequest.Path.Pin();
			if (PinnedPath.IsValid() == false)
			{
				continue;
			}

			const UObject* PathQuerier = PinnedPath->GetQuerier();
			const INavAgentInterface* PathNavAgent = Cast<const INavAgentInterface>(PathQuerier);
			if (PathNavAgent && PathNavAgent->ShouldPostponePathUpdates())
			{
				RepathRequests.Add(RecalcRequest);
				continue;
			}

			const FPathFindingQuery Query(PinnedPath.ToSharedRef());

			// Async Observations.
			FFlyingNavigationPath* FlyingPath = PinnedPath->CastPath<FFlyingNavigationPath>();
			if (FlyingPath != nullptr && FlyingPath->bAsyncObservationUpdate)
			{
				// Ignore async repath requests when currently being processed (otherwise it mightn't ever finish).
				if (!FlyingPath->bCurrentlyBeingUpdated)
				{
					const uint32 QueryID = NavSys->FindPathAsync(FNavAgentProperties::DefaultProperties, Query, PathfindingResultDelegate, EPathFindingMode::Regular);
					if (QueryID != INVALID_NAVQUERYID)
					{
						CurrentlyCalculatingPaths.Add(QueryID, RecalcRequest.Reason);
						IncrementCalculatingPaths();
						FlyingPath->UpdateLastRepathGoalLocation();
						FlyingPath->bCurrentlyBeingUpdated = true;
					} else
					{
						PinnedPath->RePathFailed();
					}
				}
			} else
			{
				const FPathFindingResult Result = FindPath(Query.NavAgentProperties, Query);

				// update time stamp to give observers any means of telling if it has changed
				PinnedPath->SetTimeStamp(TimeStamp);

				// partial paths are still valid and can change to full path when moving goal gets back on navmesh
				if (Result.IsSuccessful() || Result.IsPartial())
				{
					PinnedPath->UpdateLastRepathGoalLocation();
					PinnedPath->DoneUpdating(RecalcRequest.Reason);
					if (RecalcRequest.Reason == ENavPathUpdateType::NavigationChanged)
					{
						RegisterActivePath(PinnedPath);
					}
				}
				else
				{
					PinnedPath->RePathFailed();
				}
			}
		}
	}
}

void AFlyingNavigationData::AsyncPathfindingDelegate(const uint32 QueryID, const ENavigationQueryResult::Type Result, const FNavPathSharedPtr NavPath)
{
	if (NavPath.IsValid() && !bDisablePathfinding)
	{
		FFlyingNavigationPath* const FlyingPath = NavPath->CastPath<FFlyingNavigationPath>();
		checkf(FlyingPath, TEXT("Only FlyingPaths can be processed asynchronously"));
		FlyingPath->bCurrentlyBeingUpdated = false;

		NavPath->SetTimeStamp(GetWorldTimeStamp());
	
		ENavPathUpdateType::Type Reason;
		if ((Result == ENavigationQueryResult::Success || FlyingPath->IsPartial()) && CurrentlyCalculatingPaths.RemoveAndCopyValue(QueryID, Reason))
		{
			FlyingPath->DoneUpdating(Reason);
			if (Reason == ENavPathUpdateType::NavigationChanged)
			{
				RegisterActivePath(NavPath);
			}
		} else
		{
			FlyingPath->RePathFailed();
		}
	}
}

void AFlyingNavigationData::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	Ar << SVODataVersion;

	// For compatibility, allows for updates to NavData without crashing
	uint32 SVODataSizeBytes = 0;
	const int64 SVODataSizePos = Ar.Tell();
	Ar << SVODataSizeBytes;
	
    if (Ar.IsLoading())
    {
    	if (SVODataVersion < SVODATA_VER_MIN_COMPATIBLE )
    	{
    		SVODataVersion = SVODATA_VER_LATEST;
    		
    		// Incompatible, just skip over this data. Needs rebuilding.
    		Ar.Seek(SVODataSizePos + SVODataSizeBytes);

    		return;
    	}
	}

	// Ignore undo stack
	if (!Ar.IsTransacting())
	{
		// All we need is the navigation data
		Ar << SVOData.Get();
	}

	// Save size of data
    if (Ar.IsSaving())
    {
    	const int64 CurPos = Ar.Tell();
    	SVODataSizeBytes = CurPos - SVODataSizePos;
    	Ar.Seek(SVODataSizePos);
    	Ar << SVODataSizeBytes;
    	Ar.Seek(CurPos);
    }

	// Update display only properties
#if WITH_EDITORONLY_DATA
	if (Ar.IsLoading())
	{
		bCurrentlyBuilt = SVOData->bValid;

		if (SVOData->bValid)
		{
			ActualVoxelSize = SVOData->SubNodeSideLength;
			
			if (!SVOData->IsEmptySpace())
			{
				Subdivisions = SVOData->GetSubdivisions();
			}
		}
	}
#endif // WITH_EDITORONLY_DATA
}

void AFlyingNavigationData::PostRegisterAllComponents()
{
	Super::PostRegisterAllComponents();
}

float AFlyingNavigationData::GetOctreeSideLength() const
{
	FRWScopeLock Lock(SVODataLock, SLT_ReadOnly);

	const UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	FBox BoundsSum(ForceInit);

	if (FlyingNavGenerator.IsValid())
	{
		return FlyingNavGenerator->GetTotalBounds().GetSize().GetMax();
	} else if (SVOData->bValid)
	{
		return SVOData->SideLength;
	} else if (NavSys)
	{
		TArray<FBox> SupportedBounds;
		NavSys->GetNavigationBoundsForNavData(*this, SupportedBounds);

		for (const FBox& Box : SupportedBounds)
		{
			BoundsSum += Box;
		}
	} else
	{
		return 1.f;
	}
	
	return BoundsSum.GetSize().GetMax();
}

void AFlyingNavigationData::UpdateCurrentNavData()
{
	// Swap references, clear but don't release resources
	{
		FRWScopeLock Lock(SVODataLock, SLT_Write);

		const FSVODataRef Temp = SVOData;
		SVOData = BuildingSVOData;
		BuildingSVOData = Temp;
		BuildingSVOData->Clear();

		// Redundant update of Neighbour Graph, but not frequent
		SyncPathfindingGraph->UpdateNavData(SVOData.Get());
		AsyncPathfindingGraph->UpdateNavData(SVOData.Get());
	}

#if WITH_EDITORONLY_DATA
	UpdateSubdivisions(GetOctreeSideLength());
	bCurrentlyBuilt =  SVOData->bValid;
#endif
}

#if WITH_EDITOR
void AFlyingNavigationData::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	const FName MemberName = PropertyChangedEvent.MemberProperty != nullptr ? PropertyChangedEvent.MemberProperty->GetFName() : FName();

	UOctreeRenderingComponent* OctreeRenderer = GetRenderingComponent();

	static const FName NAME_MaxDetailSize = GET_MEMBER_NAME_CHECKED(AFlyingNavigationData, MaxDetailSize);
	static const FName NAME_bMultithreaded = GET_MEMBER_NAME_CHECKED(AFlyingNavigationData, bMultithreaded);
	static const FName NAME_ThreadSubdivisions = GET_MEMBER_NAME_CHECKED(AFlyingNavigationData, ThreadSubdivisions);
	static const FName NAME_MaxThreads = GET_MEMBER_NAME_CHECKED(AFlyingNavigationData, MaxThreads);
	static const FName NAME_bUseAgentRadius = GET_MEMBER_NAME_CHECKED(AFlyingNavigationData, bUseAgentRadius);
	static const FName NAME_bBuildOnBeginPlay = GET_MEMBER_NAME_CHECKED(AFlyingNavigationData, bBuildOnBeginPlay);
	static const FName NAME_WireThickness = GET_MEMBER_NAME_CHECKED(AFlyingNavigationData, WireThickness);
	
	if (MemberName == NAME_MaxDetailSize || MemberName == NAME_bMultithreaded || MemberName == NAME_ThreadSubdivisions)
	{
		MaxDetailSize = FMath::Max(MaxDetailSize, MIN_SUBNODE_RESOLUTION);
		
		UpdateSubdivisions(GetOctreeSideLength());

		OctreeRenderer->bGatherData = false;
		
	} else if (MemberName == NAME_bBuildOnBeginPlay ||
			   MemberName == NAME_MaxThreads ||
			   MemberName == NAME_bUseAgentRadius)
	{
		OctreeRenderer->bGatherData = false;
	} else if (MemberName == NAME_WireThickness)
	{
		// Update rending component
		OctreeRenderer->SetWireThickness(WireThickness);
		OctreeRenderer->bGatherData = false;
	} 

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif // WITH_EDITOR

void AFlyingNavigationData::SyncBuild()
{
	// Create generator if it wasn't yet
	if (NavDataGenerator.Get() == nullptr)
	{
		ConditionalConstructGenerator();
	}
	if (FlyingNavGenerator)
	{
		FlyingNavGenerator->SyncBuild();
		UpdateCurrentNavData();
	} else
	{
		printw("Runtime Generation != Dynamic!")
	}
}

void AFlyingNavigationData::RebuildFlyingNavigation(UObject* WorldContextObject, FLatentActionInfo LatentInfo)
{
	if (GEngine)
	{
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
		
		if (World)
		{
			FLatentActionManager& LatentActionManager = World->GetLatentActionManager();
			if (LatentActionManager.FindExistingAction<FRebuildAction>(LatentInfo.CallbackTarget, LatentInfo.UUID) == nullptr)
			{
				LatentActionManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, new FRebuildAction(this, LatentInfo));
			}
		}
	}
}

void AFlyingNavigationData::CleanUp()
{
	Super::CleanUp();
	if (NavDataGenerator.IsValid())
	{
		NavDataGenerator->CancelBuild();
		NavDataGenerator.Reset();
	}
}

bool AFlyingNavigationData::NeedsRebuild() const
{
	const bool bNoData = !SVOData->bValid || SVOData->Layers.Num() == 0;
	if (NavDataGenerator.IsValid())
	{
		return bNoData || NavDataGenerator->GetNumRemaningBuildTasks() > 0;
	}
	return bNoData;
}

bool AFlyingNavigationData::SupportsRuntimeGeneration() const
{
	return (RuntimeGeneration == ERuntimeGenerationType::Dynamic);
}

bool AFlyingNavigationData::SupportsStreaming() const
{
	// TODO: Level Streaming
	return false;
}

void AFlyingNavigationData::OnNavigationBoundsChanged()
{
	Super::OnNavigationBoundsChanged();
}

void AFlyingNavigationData::ConditionalConstructGenerator()
{
	if (NavDataGenerator.IsValid())
	{
		NavDataGenerator->CancelBuild();
		NavDataGenerator.Reset();
	}

	UWorld* World = GetWorld();
	check(World);
	const bool bRequiresGenerator = SupportsRuntimeGeneration() || !World->IsGameWorld();
	if (bRequiresGenerator)
	{
		FFlyingNavigationDataGenerator* Generator = CreateGeneratorInstance();
		if (Generator)
		{
			FlyingNavGenerator = MakeShareable(Generator);
			NavDataGenerator = CopyTemp(FlyingNavGenerator);
		}
	}
}

FFlyingNavigationDataGenerator* AFlyingNavigationData::CreateGeneratorInstance()
{
	return new FFlyingNavigationDataGenerator(*this);
}

UOctreeRenderingComponent* AFlyingNavigationData::GetRenderingComponent() const
{
	return Cast<UOctreeRenderingComponent>(RenderingComp);	
}

FNavLocation AFlyingNavigationData::GetRandomPoint(FSharedConstNavQueryFilter Filter, const UObject* Querier) const
{
	FRWScopeLock Lock(SVODataLock, SLT_ReadOnly);

	FNavLocation RandPoint;
	
	if (!SVOData->bValid)
	{
		return RandPoint;
	}
	
	// Old implementation: random point in bounds
	/*
	RandPoint.Location = FMath::RandPointInBox(SVOData->Bounds);
	RandPoint.NodeRef = SVOData->GetNodeLinkForPosition(RandPoint, true).AsNavNodeRef();
	return RandPoint;
	*/

	// TODO: This is lazy, do it properly
	
	TArray<FSVOLink> FreeNodes;
	SVOData->GetAllChildlessNodes(FreeNodes);
	if (FreeNodes.Num() == 0)
	{
		return RandPoint;
	}
	
	FreeNodes.Sort();
	const int32 Idx = (FreeNodes.Num() - 1) * FMath::Pow(FMath::FRand(), 1.f / 5.f); // Weight toward larger volume nodes
	const FSVOLink& NodeLink = FreeNodes[Idx];
	RandPoint.Location = FMath::RandPointInBox(FBox::BuildAABB(SVOData->GetPositionForLink(NodeLink), SVOData->GetExtentForLayer(NodeLink.GetLayerIndex())));
	RandPoint.NodeRef = NodeLink.AsNavNodeRef();
	return RandPoint;
}

bool AFlyingNavigationData::GetRandomReachablePointInRadius(const FVector& Origin, float Radius, FNavLocation& OutResult, FSharedConstNavQueryFilter Filter, const UObject* Querier) const
{
	if (!SVOData->bValid)
	{
		return false;
	}
	
	FRWScopeLock Lock(SVODataLock, SLT_ReadOnly);

	return SVOData->RandomNavigablePointInRadius(Origin, Radius, OutResult, true);
}

bool AFlyingNavigationData::GetRandomPointInNavigableRadius(const FVector& Origin, float Radius, FNavLocation& OutResult, FSharedConstNavQueryFilter Filter, const UObject* Querier) const
{
	if (!SVOData->bValid)
	{
		return false;
	}
	
	FRWScopeLock Lock(SVODataLock, SLT_ReadOnly);

	return SVOData->RandomNavigablePointInRadius(Origin, Radius, OutResult, false);
}

bool AFlyingNavigationData::ProjectPoint(const FVector& Point, FNavLocation& OutLocation, const FVector& ConnectedComponentPoint, FSharedConstNavQueryFilter Filter, const UObject* Querier) const
{
	FRWScopeLock Lock(SVODataLock, SLT_ReadOnly);
	
	const FSVOLink NodeLink = SVOData->GetNodeLinkForPosition(Point);
	if (NodeLink.IsValid())
	{
		OutLocation.NodeRef = NodeLink.AsNavNodeRef();
		OutLocation.Location = Point;
		return true;
	} else
	{
		// Check all 26 DOF
		TArray<FDirection> Directions;
		GetNeighbourGraph()->GetAvailableDirections(Point, ConnectedComponentPoint, Directions, ProjectPointExtent);
		if (Directions.Num() > 0)
		{
			// Prefer Z direction
			const FVector DirectionVector = FMath::Min(Directions).ToVector(SVOData->SubNodeSideLength);
			const FVector VoxelCentre = SVOData->SnapPositionToVoxelGrid(Point);
			
			OutLocation.Location = Point + DirectionVector;
			OutLocation.NodeRef = SVOData->GetNodeLinkForPosition(VoxelCentre + DirectionVector).AsNavNodeRef();
			return true;
		}
	}
	return false;
}

bool AFlyingNavigationData::IsNodeRefValid(NavNodeRef NodeRef) const
{
	const FSVOLink NodeLink(NodeRef);
	return NodeLink.IsValid() && static_cast<int32>(NodeLink.GetLayerIndex()) <= SVOData->Layers.Num();
}

void AFlyingNavigationData::BatchProjectPoints(TArray<FNavigationProjectionWork>& Workload, const FVector& ConnectedComponentPoint, FSharedConstNavQueryFilter Filter, const UObject* Querier) const
{
	for (FNavigationProjectionWork& Work : Workload)
	{
		Work.bResult = ProjectPoint(Work.Point, Work.OutLocation, ConnectedComponentPoint, Filter, Querier);
	}
}

void AFlyingNavigationData::BatchProjectPoints(TArray<FNavigationProjectionWork>& Workload, FSharedConstNavQueryFilter Filter, const UObject* Querier) const
{
	for (FNavigationProjectionWork& Work : Workload)
	{
		Work.bResult = ProjectPoint(Work.Point, Work.OutLocation, SVOData->GetOctreeExtent(), Filter, Querier);
	}
}

ENavigationQueryResult::Type AFlyingNavigationData::CalcPathCost(const FVector& PathStart, const FVector& PathEnd, FPathLengthType& OutPathCost, FSharedConstNavQueryFilter Filter, const UObject* Querier) const
{
	// Cost = Length TODO: Modifiers
	return CalcPathLength(PathStart, PathEnd, OutPathCost, Filter, Querier);
}

ENavigationQueryResult::Type AFlyingNavigationData::CalcPathLength(const FVector& PathStart, const FVector& PathEnd, FPathLengthType& OutPathLength, FSharedConstNavQueryFilter QueryFilter, const UObject* Querier) const
{
	// Just find a path and calculate length
	FPathFindingQuery Query;
	Query.NavData = this;
	Query.QueryFilter = QueryFilter;
	Query.StartLocation = PathStart;
	Query.EndLocation = PathEnd;
	
	FNavAgentProperties AgentProps;
	const FPathFindingResult Result = FindPath(AgentProps, Query);
	OutPathLength = Result.Path.Get()->GetLength();
	
	return Result.Result;
}

ENavigationQueryResult::Type AFlyingNavigationData::CalcPathLengthAndCost(const FVector& PathStart, const FVector& PathEnd, FPathLengthType& OutPathLength, FPathLengthType& OutPathCost, FSharedConstNavQueryFilter QueryFilter, const UObject* Querier) const
{
	// Cost = Length TODO: Modifiers
	const ENavigationQueryResult::Type Result = CalcPathLength(PathStart, PathEnd, OutPathLength, QueryFilter, Querier);
	OutPathCost = OutPathLength;
	return Result;
}

bool AFlyingNavigationData::DoesNodeContainLocation(NavNodeRef NodeRef, const FVector& WorldSpaceLocation) const
{
	const FSVOLink NodeLink(NodeRef);

	if (NodeLink.IsValid())
	{
		FRWScopeLock Lock(SVODataLock, SLT_ReadOnly);
		return SVOData->GetNodeBoxForLink(NodeLink).IsInsideOrOn(WorldSpaceLocation);
	}

	return false;
}

UPrimitiveComponent* AFlyingNavigationData::ConstructRenderingComponent()
{
	return NewObject<UOctreeRenderingComponent>(this, TEXT("OctreeRenderingComp"), RF_Transient);
}

void AFlyingNavigationData::ApplyWorldOffset(const FVector& InOffset, bool bWorldShift)
{
	if (SVOData->bValid)
	{
		SVOData->ApplyWorldOffset(InOffset);
	}

#if WITH_EDITORONLY_DATA
	if (PathBatcher)
	{
		PathBatcher->ApplyWorldOffset(InOffset, bWorldShift);
	}
#endif

	Super::ApplyWorldOffset(InOffset, bWorldShift);
	RequestDrawingUpdate();
}

void AFlyingNavigationData::BatchRaycast(TArray<FNavigationRaycastWork>& Workload, FSharedConstNavQueryFilter QueryFilter, const UObject* Querier) const
{
	FRWScopeLock Lock(SVODataLock, SLT_ReadOnly);
	
	const FSVORaycast SVORaycast(SVOData.Get());
	
	for (FNavigationRaycastWork& Work: Workload)
	{
		if ((Work.RayEnd - Work.RayStart).IsNearlyZero())
		{
			continue;
		}
		Work.bDidHit = SVORaycast.Raycast(Work.RayStart, Work.RayEnd, Work.HitLocation.Location);
	}
}

FBox AFlyingNavigationData::GetFlyingBounds() const
{
	FBox Bbox(ForceInit);

	if (SVOData->bValid)
	{
		Bbox = SVOData->Bounds;
	}
	
	return Bbox;
}

#if WITH_EDITOR
void AFlyingNavigationData::DrawDebug() const
{
	FlushPersistentDebugLines(GetWorld());
    if (SVOData->bValid && GetWorld())
    {
    	FSVOGenerator::DebugDraw(GetWorld(), *SVOData);
    }
}
#endif // WITH_EDITOR

void AFlyingNavigationData::UpdateDrawing()
{
#if !UE_BUILD_SHIPPING
	UOctreeRenderingComponent* OctreeRenderComp = Cast<UOctreeRenderingComponent>(RenderingComp);
	if (OctreeRenderComp != nullptr && OctreeRenderComp->GetVisibleFlag() && (OctreeRenderComp->IsForcingUpdate() || UOctreeRenderingComponent::IsNavigationShowFlagSet(GetWorld())))
	{
		RenderingComp->MarkRenderStateDirty();
	}
#endif // !UE_BUILD_SHIPPING
}

void AFlyingNavigationData::RequestDrawingUpdate(bool bForce)
{
#if !UE_BUILD_SHIPPING
	if (bForce || UOctreeRenderingComponent::IsNavigationShowFlagSet(GetWorld()))
	{
		if (bForce)
		{
			UOctreeRenderingComponent* OctreeRenderComp = Cast<UOctreeRenderingComponent>(RenderingComp);
			if (OctreeRenderComp)
			{
				OctreeRenderComp->ForceUpdate();
			}
		}

		DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.Requesting Octree Redraw"),
			STAT_FSimpleDelegateGraphTask_RequestingOctreeRedraw,
			STATGROUP_TaskGraphTasks);

		FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
    FSimpleDelegateGraphTask::FDelegate::CreateUObject(this, &AFlyingNavigationData::UpdateDrawing),
				 GET_STATID(STAT_FSimpleDelegateGraphTask_RequestingOctreeRedraw), nullptr, ENamedThreads::GameThread);
	}
#endif // !UE_BUILD_SHIPPING
}

void AFlyingNavigationData::OnOctreeGenerationFinished()
{
	UWorld* World = GetWorld();

	if (World != nullptr && IsValid(World))
	{
		// Swaps building navdata into current navdata. I'm hoping 
		UpdateCurrentNavData();
		
		// TODO: Level streaming
		// Check ARecastNavMesh::OnNavMeshGenerationFinished()
		
#if WITH_EDITOR
		// Force navmesh drawing update
		RequestDrawingUpdate(true);		
#endif// WITH_EDITOR

		UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
		if (NavSys)
		{
			NavSys->OnNavigationGenerationFinished(*this);
		}

		OnFlyingNavGenerationFinished.Broadcast();
	}
}

FVector AFlyingNavigationData::ModifyPathEndpoints(const FVector& TargetPoint, const FVector& ConnectedPoint, const float AgentHalfHeight) const
{
	FRWScopeLock Lock(SVODataLock, SLT_ReadOnly);
	if (SVOData->IsPositionBlocked(TargetPoint))
	{
		// Try half height
		const FVector HalfHeightPoint = TargetPoint + FVector(0.f, 0.f, AgentHalfHeight);
		if (AgentHalfHeight != 0 && !SVOData->IsPositionBlocked(HalfHeightPoint))
		{
			// Shift up by half height
			return HalfHeightPoint;
		} 

		// Try projecting
		FNavLocation ProjectedPoint;
		if (ProjectPoint(TargetPoint, ProjectedPoint, ConnectedPoint))
		{
			return ProjectedPoint.Location;
		}
	}
	
	return TargetPoint;
}

FPathFindingResult AFlyingNavigationData::FindPath(const FNavAgentProperties& AgentProperties, const FPathFindingQuery& Query)
{
	const ANavigationData* Self = Query.NavData.Get();
	const AFlyingNavigationData* FlyingNavData = CastChecked<const AFlyingNavigationData>(Self);
	
	// Clean up async locks
	const bool bInAsyncThread = !IsInGameThread();
	ON_SCOPE_EXIT
	{
		if (bInAsyncThread && FlyingNavData != nullptr)
		{
			// Manually unlock critical section
			FlyingNavData->AsyncGraphCriticalSection.Unlock();

			// Make sure number of paths is kept up to date
			FlyingNavData->DecrementCalculatingPaths();
		}
	};

	if (Self == nullptr || FlyingNavData == nullptr || !FlyingNavData->SVOData->bValid || FlyingNavData->bDisablePathfinding)
	{
		return ENavigationQueryResult::Error;
	}
	
	// Access Navigation Graph
	FSVOPathfindingGraph* NavigationGraph = nullptr;
	
	if (bInAsyncThread)
	{
		if (FlyingNavData->AsyncPathfindingGraph)
		{
			// Manually lock critical section
			FlyingNavData->AsyncGraphCriticalSection.Lock();
			NavigationGraph = FlyingNavData->AsyncPathfindingGraph.Get();
		}
	} else
	{
		NavigationGraph = FlyingNavData->SyncPathfindingGraph.Get();
	}

	// Lock SVO Data
	FRWScopeLock Lock(FlyingNavData->SVODataLock, SLT_ReadOnly);
	const FSVOData& SVOData = FlyingNavData->SVOData.Get();

	// Make sure we have a navigation graph
	if (!ensureMsgf(NavigationGraph != nullptr, TEXT("Navigation Graph is invalid!")))
	{
		return ENavigationQueryResult::Error;
	}

	// Get FNavigationPath to fill
	FPathFindingResult Result(ENavigationQueryResult::Error);

	const bool bExistingPath = Query.PathInstanceToFill.IsValid();
	if (bExistingPath)
	{
		Result.Path = Query.PathInstanceToFill;
		// Not resetting for repath just yet: Defer until after pathfinding
	}
	else
	{
		Result.Path = Self->CreatePathInstance<FFlyingNavigationPath>(Query);
	}

	FFlyingNavigationPath* NavPath = Result.Path->CastPath<FFlyingNavigationPath>();
	
	if (NavPath != nullptr)
	{
		NavPath->ApplyFlags(Query.NavDataFlags);
		
		FSVOQuerySettings QuerySettings;

		// Override SVOQuerySettings
		const UObject* Owner = Query.Owner.Get();
		// Potentially slow call... it's this or enabling RTTI for the whole project
		if (Owner && Owner->Implements<UFlyingObjectInterface>())
		{
			QuerySettings = IFlyingObjectInterface::Execute_GetPathfindingQuerySettings(Owner);
		} else
		{
			QuerySettings = FlyingNavData->DefaultQuerySettings;
		}
		QuerySettings.SetNavData(SVOData);

		// PROBLEM: When using a Pawn as target or goal, the APawn::GetNavAgentLocation() will return the 'feet' of the pawn (using Pawn->BaseEyeHeight).
		// This is often inside blocked volume, so we need to do a test and compensate
		float StartAgentHalfHeight = AgentProperties.AgentHeight * 0.5f;
		
		// Try to get from pawn updated component, otherwise use AgentProperties. Owner could be pawn or controller
		const AController* Controller = Cast<const AController>(Owner);
		if (const APawn* Pawn = Controller ? Controller->GetPawn() : Cast<const APawn>(Owner))
		{
			const UNavMovementComponent* const MovementComponent = Pawn->GetMovementComponent();
			if (MovementComponent && MovementComponent->UpdatedComponent)
			{
				StartAgentHalfHeight = MovementComponent->UpdatedComponent->Bounds.BoxExtent.Z;
			}
		}

		// Get goal agent properties
		float GoalAgentHalfHeight = AgentProperties.AgentHeight * 0.5f;
		if (const APawn* GoalPawn = NavPath->GoalPawn.Get())
		{
			GoalAgentHalfHeight = GoalPawn->BaseEyeHeight;
		}

		// Modify start and end point if required
		// Bit of a catch 22, but it's most likely that the target will be in a blocked area.
		const FVector StartLocation = FlyingNavData->ModifyPathEndpoints(Query.StartLocation, Query.EndLocation, StartAgentHalfHeight); 
		const FVector EndLocation   = FlyingNavData->ModifyPathEndpoints(Query.EndLocation, StartLocation, GoalAgentHalfHeight);
		
		bool bPartialSolution = false;
		TArray<FNavPathPoint> PathPoints;
		PathPoints.Reserve(NavPath->GetPathPoints().Num());
		Result.Result = NavigationGraph->FindPath(StartLocation, EndLocation, QuerySettings, PathPoints, bPartialSolution);
		
		if (Result.Result == ENavigationQueryResult::Success)
		{
			NavPath->SetIsPartial(bPartialSolution);

			if (QuerySettings.bUsePawnCentreForPathFollowing)
			{
				// Translate path to compensate for default feet translation (see UNavMovementComponent::GetActorFeetLocation())
				for (FNavPathPoint& PathPoint : PathPoints)
				{
					PathPoint.Location.Z -= StartAgentHalfHeight;
				}
			} else
			{
				StartAgentHalfHeight = 0.f;
			}

			if (bExistingPath)
			{
				NavPath->ResetForRepath();
			}
			
			// Conditionally smooth path
			if (QuerySettings.bSmoothPath)
			{
				UFlyingNavFunctionLibrary::SmoothPathPoints(PathPoints);
			}
			
			NavPath->GetPathPoints() = MoveTemp(PathPoints);
			NavPath->MarkReady();

#if WITH_EDITORONLY_DATA
			if (FlyingNavData->bDrawDebugPaths && FlyingNavData->PathBatcher)
			{
				NavPath->DrawIndex = FlyingNavData->PathBatcher->AddPath(
					*NavPath,
					FVector(0.f, 0.f, StartAgentHalfHeight),
					QuerySettings.DebugPathColor.ToFColor(true),
					NavPath->DrawIndex);
			}
#endif // WITH_EDITORONLY_DATA
		}
	}
	
	return Result;
}

bool AFlyingNavigationData::TestPath(const FNavAgentProperties& AgentProperties, const FPathFindingQuery& Query, int32* NumVisitedNodes)
{
	const ANavigationData* Self = Query.NavData.Get();
	check(Cast<const AFlyingNavigationData>(Self));

	const AFlyingNavigationData* FlyingNavData = Cast<const AFlyingNavigationData>(Self);
	if (Self == nullptr || FlyingNavData == nullptr || !FlyingNavData->SVOData->bValid)
	{
		return false;
	}

	FRWScopeLock Lock(FlyingNavData->SVODataLock, SLT_ReadOnly);
	FSVOData& NavigationData = FlyingNavData->SVOData.Get();

	const FSVOLink StartLink = NavigationData.GetNodeLinkForPosition(Query.StartLocation);
	const FSVOLink EndLink = NavigationData.GetNodeLinkForPosition(Query.EndLocation);

	if (StartLink.IsValid() && EndLink.IsValid())
	{
		return NavigationData.IsConnected(StartLink, EndLink);
	}
	return false;
}

bool AFlyingNavigationData::OctreeRaycast(const ANavigationData* Self, const FVector& RayStart, const FVector& RayEnd, FVector& HitLocation, FSharedConstNavQueryFilter QueryFilter, const UObject* Querier)
{
	const AFlyingNavigationData* FlyingNavData = CastChecked<const AFlyingNavigationData>(Self);
	return FlyingNavData->OctreeRaycast(RayStart, RayEnd, HitLocation);
}

bool AFlyingNavigationData::OctreeRaycast(const FVector& RayStart, const FVector& RayEnd, FVector& HitLocation) const
{
	if ((RayEnd - RayStart).IsNearlyZero())
	{
		return false;
	}
	
	FRWScopeLock Lock(SVODataLock, SLT_ReadOnly);
	const FSVOData& NavData = SVOData.Get();

	return FSVORaycast(NavData).Raycast(RayStart, RayEnd, HitLocation);
}

uint32 AFlyingNavigationData::LogMemUsed() const
{
	const uint32 SuperMemUsed = Super::LogMemUsed();

	const uint32 MemUsed = SVOData->GetAllocatedSize() + BuildingSVOData->GetAllocatedSize();

	UE_LOG(LogFlyingNavSystem, Warning, TEXT("%s: AFlyingNavigationData: %u\n    self: %d"), *GetName(), MemUsed, sizeof(AFlyingNavigationData));	

	return MemUsed + SuperMemUsed;
}
