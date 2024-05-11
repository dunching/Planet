// Copyright Ben Sutherland 2024. All rights reserved.

#include "FlyingNavigationSystem.h"
#include "NavigationData.h"
#include "FlyingNavSystemModule.h"
#include "ProfilingDebugging/CsvProfiler.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlyingNavigationSystem)

// Stats
DECLARE_CYCLE_STAT(TEXT("Nav Tick: multi-frame async pathfinding"),
                   STAT_Navigation_TickMultiFrameAsyncPathfinding,
                   STATGROUP_Navigation);
DECLARE_CYCLE_STAT(TEXT("Sync requests for multi-frame async pathfinding"),
                   STAT_Navigation_RequestingMultiFrameAsyncPathfinding,
                   STATGROUP_Navigation);
DECLARE_CYCLE_STAT(TEXT("Multi-frame async pathfinding"),
                   STAT_Navigation_PathfindingMultiFrameAsync,
                   STATGROUP_Navigation);
DECLARE_CYCLE_STAT(TEXT("Nav Tick: dispatch multi-frame async pathfinding results"),
				   STAT_Navigation_DispatchMultiFrameAsyncPathfindingResults,
				   STATGROUP_Navigation);

CSV_DEFINE_CATEGORY(FlyingNavigationSystem, false);

static const uint32 INITIAL_ASYNC_QUERIES_SIZE = 32;

//----------------------------------------------------------------------//
// FAsyncPathFindingQueryMultiFrame
//----------------------------------------------------------------------//

// This is an exact copy of FAsyncPathFindingQuery from NavigationData.h, because the dll linkage doesn't let me call
// the constructor on the original, and I can't construct my own because the fields are const.
// Rubbish, I know.
struct FAsyncPathFindingQueryMultiFrame : FPathFindingQuery
{
	const uint32 QueryID;
	const FNavPathQueryDelegate OnDoneDelegate;
	const TEnumAsByte<EPathFindingMode::Type> Mode;
	FPathFindingResult Result;

	FAsyncPathFindingQueryMultiFrame()
		: QueryID(INVALID_NAVQUERYID),
		  Mode(EPathFindingMode::Regular)
	{
	}

	FAsyncPathFindingQueryMultiFrame(const UObject* InOwner,
	                                 const ANavigationData& InNavData,
	                                 const FVector& Start,
	                                 const FVector& End,
	                                 const FNavPathQueryDelegate& Delegate,
	                                 FSharedConstNavQueryFilter SourceQueryFilter,
	                                 const FVector::FReal CostLimit = TNumericLimits<FVector::FReal>::Max())
		: FPathFindingQuery(InOwner, InNavData, Start, End, SourceQueryFilter),
		  QueryID(GetUniqueID()),
		  OnDoneDelegate(Delegate),
		  Mode(EPathFindingMode::Regular)
	{
	}

	FAsyncPathFindingQueryMultiFrame(const FPathFindingQuery& Query,
	                                 const FNavPathQueryDelegate& Delegate,
	                                 const EPathFindingMode::Type QueryMode)
		: FPathFindingQuery(Query),
		  QueryID(GetUniqueID()),
		  OnDoneDelegate(Delegate),
		  Mode(QueryMode)
	{
	}

protected:
	FORCEINLINE static uint32 GetUniqueID()
	{
		return ++LastPathFindingUniqueID;
	}

	static uint32 LastPathFindingUniqueID;
};

uint32 FAsyncPathFindingQueryMultiFrame::LastPathFindingUniqueID = INVALID_NAVQUERYID;

//----------------------------------------------------------------------//
// UFlyingNavigationSystem
//----------------------------------------------------------------------//

UFlyingNavigationSystem::UFlyingNavigationSystem()
{
	if (HasAnyFlags(RF_ClassDefaultObject) == false)
	{
		// reserve some arbitrary size
		AsyncPathFindingQueriesMultiFrame.Reserve(INITIAL_ASYNC_QUERIES_SIZE);
	}
}

// Copied from UNavigationSystemV1::AddAsyncQuery()
void UFlyingNavigationSystem::AddAsyncQueryMultiFrame(const FAsyncPathFindingQueryMultiFrame& Query)
{
	check(IsInGameThread());
	FScopeLock Lock(&PendingQueriesLock);
	AsyncPathFindingQueriesMultiFrame.Add(Query);
}

void UFlyingNavigationSystem::PostponeMultiFrameAsyncQueries()
{
	if (AsyncPathFindingTaskMultiFrame.GetReference() && !AsyncPathFindingTaskMultiFrame->IsComplete())
	{
		bAbortAsyncQueriesRequestedMultiFrame = true;
		FTaskGraphInterface::Get().WaitUntilTaskCompletes(AsyncPathFindingTaskMultiFrame, ENamedThreads::GameThread);
		bAbortAsyncQueriesRequestedMultiFrame = false;
	}
}

// Copied from UNavigationSystemV1::FindPathAsync()
uint32 UFlyingNavigationSystem::FindPathAsyncMultiFrame(const FNavAgentProperties& AgentProperties,
                                                        FPathFindingQuery Query,
                                                        const FNavPathQueryDelegate& ResultDelegate,
                                                        EPathFindingMode::Type Mode)
{
	SCOPE_CYCLE_COUNTER(STAT_Navigation_RequestingMultiFrameAsyncPathfinding);

	if (Query.NavData.IsValid() == false)
	{
		Query.NavData = GetNavDataForProps(AgentProperties, Query.StartLocation);
	}

	if (Query.NavData.IsValid())
	{
		FAsyncPathFindingQueryMultiFrame AsyncQuery(Query, ResultDelegate, Mode);

		if (AsyncQuery.QueryID != INVALID_NAVQUERYID)
		{
			AddAsyncQueryMultiFrame(AsyncQuery);
		}

		return AsyncQuery.QueryID;
	}

	return INVALID_NAVQUERYID;
}

void UFlyingNavigationSystem::AbortAsyncFindPathRequestMultiFrame(uint32 AsyncPathQueryID)
{
	check(IsInGameThread());
	FScopeLock Lock(&PendingQueriesLock);
	FAsyncPathFindingQueryMultiFrame* Query = AsyncPathFindingQueriesMultiFrame.GetData();
	for (int32 Index = 0; Index < AsyncPathFindingQueriesMultiFrame.Num(); ++Index, ++Query)
	{
		if (Query->QueryID == AsyncPathQueryID)
		{
			AsyncPathFindingQueriesMultiFrame.RemoveAtSwap(Index);
			break;
		}
	}
}

void UFlyingNavigationSystem::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// NOTE: Copied from UNavigationSystemV1::Tick() with variable renames and added locks.
	{
		// In multithreaded configuration we can process async pathfinding queries
		// in dedicated task while dispatching completed queries results on the main thread.
		// The created task can start and append new result right away so we transfer
		// completed queries before to keep the list safe.
		TArray<FAsyncPathFindingQueryMultiFrame> AsyncPathFindingCompletedQueriesToDispatch;
		{
			FScopeLock Lock(&CompletedQueriesLock);
			Swap(AsyncPathFindingCompletedQueriesToDispatch, AsyncPathFindingCompletedQueriesMultiFrame);
		}

		{
			FScopeLock Lock(&PendingQueriesLock);

			// Trigger the async pathfinding queries (new ones and those that may have been postponed from last frame)
			if (AsyncPathFindingQueriesMultiFrame.Num() > 0)
			{
				SCOPE_CYCLE_COUNTER(STAT_Navigation_TickMultiFrameAsyncPathfinding);
				TriggerAsyncQueriesMultiFrame(AsyncPathFindingQueriesMultiFrame);
			}
			AsyncPathFindingQueriesMultiFrame.Reset();
		}

		// Dispatch async pathfinding queries results from last frame
		DispatchAsyncQueriesResultsMultiFrame(AsyncPathFindingCompletedQueriesToDispatch);
	}
}

// Copied from NavigationSystem.cpp
FAutoConsoleTaskPriority CPrio_TriggerAsyncQueries_MultiFrame(
	TEXT("TaskGraph.TaskPriorities.NavTriggerAsyncQueriesMultiFrame"),
	TEXT("Task and thread priority for UFlyingNavigationSystem::PerformAsyncQueriesMultiFrame."),
	// if we have background priority task threads, then use them...
	ENamedThreads::BackgroundThreadPriority,
	// .. at normal task priority
	ENamedThreads::NormalTaskPriority,
	// if we don't have background threads, then use normal priority threads at normal task priority instead
	ENamedThreads::NormalTaskPriority
);

void UFlyingNavigationSystem::TriggerAsyncQueriesMultiFrame(
	const TArray<FAsyncPathFindingQueryMultiFrame>& PathFindingQueries)
{
	DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.FlyingNavigationSystem batched async queries"),
	                   STAT_FSimpleDelegateGraphTask_FlyingNavigationSystemBatchedAsyncQueries,
	                   STATGROUP_TaskGraphTasks);

	AsyncPathFindingTaskMultiFrame = FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
		FSimpleDelegateGraphTask::FDelegate::CreateUObject(this,
		                                                   &UFlyingNavigationSystem::PerformAsyncQueriesMultiFrame,
		                                                   PathFindingQueries),
		GET_STATID(STAT_FSimpleDelegateGraphTask_FlyingNavigationSystemBatchedAsyncQueries),
		nullptr,
		CPrio_TriggerAsyncQueries_MultiFrame.Get());
}

// Also almost copied directly from UNavigationSystemV1::PerformAsyncQueries()
void UFlyingNavigationSystem::PerformAsyncQueriesMultiFrame(TArray<FAsyncPathFindingQueryMultiFrame> PathFindingQueries)
{
	SCOPE_CYCLE_COUNTER(STAT_Navigation_PathfindingMultiFrameAsync);
	CSV_SCOPED_TIMING_STAT(FlyingNavigationSystem, PathfindingAsync);

	if (PathFindingQueries.Num() == 0)
	{
		return;
	}

	int32 NumProcessed = 0;
	for (FAsyncPathFindingQueryMultiFrame& Query : PathFindingQueries)
	{
		// @todo this is not necessarily the safest way to use UObjects outside of main thread. 
		//	think about something else.
		const ANavigationData* NavData = Query.NavData.IsValid()
			                                 ? Query.NavData.Get()
			                                 : GetDefaultNavDataInstance(FNavigationSystem::DontCreate);

		// perform query
		if (NavData)
		{
			if (Query.Mode == EPathFindingMode::Hierarchical)
			{
				Query.Result = NavData->FindHierarchicalPath(Query.NavAgentProperties, Query);
			}
			else
			{
				Query.Result = NavData->FindPath(Query.NavAgentProperties, Query);
			}
		}
		else
		{
			Query.Result = ENavigationQueryResult::Error;
		}
		++NumProcessed;

		{
			FScopeLock Lock(&CompletedQueriesLock);
			// Append to list of completed queries to dispatch results in main thread
			AsyncPathFindingCompletedQueriesMultiFrame.Push(Query);
		}

		// Check for abort request from the main tread
		if (bAbortAsyncQueriesRequested)
		{
			break;
		}
	}

	const int32 NumQueries = PathFindingQueries.Num();
	const int32 NumPostponed = NumQueries - NumProcessed;

	// Queue remaining queries for next frame
	if (bAbortAsyncQueriesRequestedMultiFrame)
	{
		FScopeLock Lock(&PendingQueriesLock);
		AsyncPathFindingQueriesMultiFrame.Append(PathFindingQueries.GetData() + NumProcessed, NumPostponed);
	}

	UE_LOG(LogFlyingNavSystem,
	       Log,
	       TEXT("Flying async pathfinding queries: %d completed, %d postponed to next frame"),
	       NumProcessed,
	       NumPostponed);
}

void UFlyingNavigationSystem::DispatchAsyncQueriesResultsMultiFrame(const TArray<FAsyncPathFindingQueryMultiFrame>& PathFindingQueries)
{
	if (PathFindingQueries.Num() > 0)
	{
		SCOPE_CYCLE_COUNTER(STAT_Navigation_DispatchMultiFrameAsyncPathfindingResults);
		CSV_SCOPED_TIMING_STAT(FlyingNavigationSystem, AsyncNavQueryFinished);

		for (const FAsyncPathFindingQueryMultiFrame& Query : PathFindingQueries)
		{
			Query.OnDoneDelegate.ExecuteIfBound(Query.QueryID, Query.Result.Result, Query.Result.Path);
		}
	}
}