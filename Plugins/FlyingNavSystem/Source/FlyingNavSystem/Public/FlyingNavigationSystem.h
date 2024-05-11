// Copyright Ben Sutherland 2024. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "NavigationSystem.h"
#include "FlyingNavigationSystem.generated.h"

struct FAsyncPathFindingQueryMultiFrame;

/**
 *
 * Custom subclass of NavigationSystemV1 that allows pathfinding to be calculated over multiple frames instead of
 * flushed at frame end. This is achieved by creating a separate queue of queries that don't get flushed.
 * However, once a batch of queries are in-flight they can be aborted with PostponeMultiFrameAsyncQueries, which blocks
 * on the currently running query then postpones the remainder in the batch.
 *
 * Unfortunately, most of the code here is a very close duplicate of the UNavigationSystemV1 code, because there are no
 * virtual functions or delegates to speak of. The linkage of the FAsyncPathFindingQuery means I can't use the original
 * and it's entirely duplicated.
 */
UCLASS()
class FLYINGNAVSYSTEM_API UFlyingNavigationSystem : public UNavigationSystemV1
{
	GENERATED_BODY()

public:
	UFlyingNavigationSystem();
	
	// Allows pathfinding to be calculated over multiple frames instead of flushed at frame end.
	// Otherwise, identical to UNavigationSystemV1::FindPathAsync.
	uint32 FindPathAsyncMultiFrame(const FNavAgentProperties& AgentProperties,
	                               FPathFindingQuery Query,
	                               const FNavPathQueryDelegate& ResultDelegate,
	                               EPathFindingMode::Type Mode);
	void AbortAsyncFindPathRequestMultiFrame(uint32 AsyncPathQueryID);

	// Make PostponeAsyncQueries public and BP-exposed so we can call it anywhere.
	UFUNCTION(BlueprintCallable, Category = "Navigation")
	// ReSharper disable once CppHidingFunction CppUEBlueprintCallableFunctionUnused
	void PostponeMultiFrameAsyncQueries();

protected:
	//~Begin UNavigationSystemBase Interface
	virtual void Tick(float DeltaSeconds) override;
	//~End UNavigationSystemBase Interface

protected:
	void AddAsyncQueryMultiFrame(const FAsyncPathFindingQueryMultiFrame& Query);
	
	void PerformAsyncQueriesMultiFrame(TArray<FAsyncPathFindingQueryMultiFrame> AsyncPathFindingQueries);
	void TriggerAsyncQueriesMultiFrame(const TArray<FAsyncPathFindingQueryMultiFrame>& PathFindingQueries);

	static void DispatchAsyncQueriesResultsMultiFrame(const TArray<FAsyncPathFindingQueryMultiFrame>& PathFindingQueries);
	
	/** Queued async pathfinding queries to process in the next update. */
	TArray<FAsyncPathFindingQueryMultiFrame> AsyncPathFindingQueriesMultiFrame;
	FCriticalSection PendingQueriesLock;

	/** Queued async pathfinding results computed by the dedicated task in the last frame and ready to dispatch in the next update. */
	TArray<FAsyncPathFindingQueryMultiFrame> AsyncPathFindingCompletedQueriesMultiFrame;
	FCriticalSection CompletedQueriesLock;
	
	/** Flag used by main thread to ask the async pathfinding task to stop and postpone remaining queries, if any. */
	std::atomic<bool> bAbortAsyncQueriesRequestedMultiFrame;
	/** Graph event that the main thread will wait for to synchronize with the async pathfinding task, if any. */
	FGraphEventRef AsyncPathFindingTaskMultiFrame;
};
