// Copyright Ben Sutherland 2024. All rights reserved.

#include "FlyingNavFunctionLibrary.h"
#include "AIController.h"
#include "FlyingNavigationSystem.h"
#include "FlyingNavSystemModule.h"
#include "LatentActions.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Algo/BinarySearch.h"
#include "Engine/Engine.h"
#include "GameFramework/PawnMovementComponent.h"
#include "NavFilters/NavigationQueryFilter.h"
#include "Navigation/PathFollowingComponent.h"

#if ENABLE_DRAW_DEBUG
#include "DrawDebugHelpers.h"
#endif // ENABLE_DRAW_DEBUG

// Dummy latent action to return immediately
class FReturnAction final : public FPendingLatentAction
{
	FName ExecutionFunction;
	int32 OutputLink;
	FWeakObjectPtr CallbackTarget;

public:
	FReturnAction(const FLatentActionInfo& LatentInfo)
		: ExecutionFunction(LatentInfo.ExecutionFunction),
		  OutputLink(LatentInfo.Linkage),
		  CallbackTarget(LatentInfo.CallbackTarget)
	{
	}

	virtual void UpdateOperation(FLatentResponse& Response) override
	{
		Response.FinishAndTriggerIf(true, ExecutionFunction, OutputLink, CallbackTarget);
	}
};

#define LATENT_RETURN_NOW() LatentActionManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, new FReturnAction(LatentInfo));

//----------------------------------------------------------------------//
// FPathFindAction Implementation
//----------------------------------------------------------------------//
class FPathFindAction final : public FPendingLatentAction
{
	struct FDelegateHelper final : FGCObject
	{
		TWeakObjectPtr<AActor> GoalActor;
		TObjectPtr<UNavigationPath> Path;
		float TetherDistance;
		bool bFinished;

		explicit FDelegateHelper(UNavigationPath* NavPath,
		                         AActor* GoalActor,
		                         const float TetherDistance)
			: GoalActor(GoalActor),
			  Path(NavPath),
			  TetherDistance(TetherDistance),
			  bFinished(false)
		{
		}

		//~ Begin FGCObject Interface
		virtual void AddReferencedObjects(FReferenceCollector& Collector) override
		{
			// Make sure Path isn't deleted
			Collector.AddReferencedObject(Path);
		}

		virtual FString GetReferencerName() const override { return TEXT("FPathFindAction::FDelegateHelper"); }
		//~ End FGCObject Interface

		void PathfindingFinished(const uint32 QueryID,
		                         const ENavigationQueryResult::Type Result,
		                         const FNavPathSharedPtr NavPath)
		{
			if (Path)
			{
				Path->SetPath(NavPath);
				if (Path->GetPath())
				{
					FFlyingNavigationPath* FlyingPath = Path->GetPath()->CastPath<FFlyingNavigationPath>();
					if (FlyingPath)
					{
						FlyingPath->QueryResult = Result;
					}

					if ((Result == ENavigationQueryResult::Success || (NavPath && NavPath->IsPartial())) && GoalActor.
						IsValid())
					{
						// Actor observation updates
						Path->GetPath()->SetGoalActorObservation(*GoalActor.Get(), TetherDistance);
					}
				}
			}

			bFinished = true;
		}
	};

	FName ExecutionFunction;
	int32 OutputLink;
	FWeakObjectPtr CallbackTarget;
	TSharedRef<FDelegateHelper> Delegate;
	FAsyncPathQueryHandle AsyncQueryHandle = FAsyncPathQueryHandle::InvalidHandle;

public:
	FPathFindAction(UNavigationPath* NavPath,
	                FNavPathQueryDelegate& QueryDelegate,
	                const FLatentActionInfo& LatentInfo,
	                AActor* GoalActor = nullptr,
	                const float TetherDistance = 0)
		: ExecutionFunction(LatentInfo.ExecutionFunction),
		  OutputLink(LatentInfo.Linkage),
		  CallbackTarget(LatentInfo.CallbackTarget),
		  Delegate(new FDelegateHelper(NavPath, GoalActor, TetherDistance))
	{
		// Delegate called on the game thread
		QueryDelegate.BindSP(Delegate, &FDelegateHelper::PathfindingFinished);
	}

	virtual ~FPathFindAction() override
	{
		// Cleanup async query if not already started
		if (!Delegate.Get().bFinished && AsyncQueryHandle.IsValid() && Delegate->Path)
		{
			UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(Delegate->Path);

			if (NavSys)
			{
				UFlyingNavFunctionLibrary::AbortAsyncFindPathRequest(NavSys, AsyncQueryHandle);
			}
		}
	}

	void SetQueryHandle(const FAsyncPathQueryHandle& QueryHandle) { AsyncQueryHandle = QueryHandle; }

	virtual void UpdateOperation(FLatentResponse& Response) override
	{
		Response.FinishAndTriggerIf(Delegate.Get().bFinished, ExecutionFunction, OutputLink, CallbackTarget);
	}

#if WITH_EDITOR
	// Returns a human readable description of the latent operation's current state
	virtual FString GetDescription() const override
	{
		return FString::Printf(TEXT("FPathFindAction finished: %s"),
		                       Delegate.Get().bFinished ? TEXT("True") : TEXT("False"));
	}
#endif
};

//----------------------------------------------------------------------//
// UCatmullRomCurve Implementation
//----------------------------------------------------------------------//

bool UCatmullRomSpline::GenerateSpline(const TArray<FVector>& PathPoints)
{
	const int32 NumPathPoints = PathPoints.Num();

	PathSegmentsLength = 0.f;
	for (int32 i = 1; i < NumPathPoints; i++)
	{
		PathSegmentsLength += FVector::Dist(PathPoints[i], PathPoints[i - 1]);
	}

	if (FMath::IsNearlyZero(PathSegmentsLength))
	{
		return false;
	}

	PValues.Reset(NumPathPoints + 2);

	// Add Endpoints as straight extensions
	PValues.Add(2.f * PathPoints[0] - PathPoints[1]);
	PValues.Append(PathPoints);
	PValues.Add(2.f * PathPoints.Last() - PathPoints.Last(1));

	// Calculate T values
	TValues.SetNumUninitialized(PValues.Num());
	TValues[0] = 0.f;
	for (int32 i = 1; i < PValues.Num(); i++)
	{
		TValues[i] = TValues[i - 1] + FMath::Sqrt(FVector::Dist(PValues[i - 1], PValues[i]));

		if (!ensureMsgf(!FMath::IsNearlyZero(TValues[i]), TEXT("Path points too close together! Invalid curve.")))
		{
			return false;
		}
	}
	MaxT = TValues.Last(1);
	TScale = MaxT - TValues[1]; // T input needs to be between 0 and 1, from P1 to P(n-1)

	if (PValues.Num() >= 4)
	{
		bValid = true;
	}

	FillLUT();

	return bValid;
}

FVector UCatmullRomSpline::SampleSplineByParameter(const float T) const
{
	if (!ensureMsgf(bValid, TEXT("Curve is not valid.")))
	{
		return FVector::ZeroVector;
	}

	checkf(0.f <= T && T <= 1, TEXT("T must be in the range [0., 1.] to sample curve."));

	const float ScaledT = TValues[1] + T * TScale;
	// Find T2Idx such that TValues[T2Idx-1] <= ScaledT < TValues[T2Idx]
	const float T2Idx = Algo::UpperBound(TValues, ScaledT);

	// If T == 1, return the last point
	if (T2Idx + 1 == PValues.Num())
	{
		return PValues.Last(1);
	}

	const float T0 = TValues[T2Idx - 2] - ScaledT;
	const float T1 = TValues[T2Idx - 1] - ScaledT;
	const float T2 = TValues[T2Idx] - ScaledT;
	const float T3 = TValues[T2Idx + 1] - ScaledT;

	const FVector P0 = PValues[T2Idx - 2];
	const FVector P1 = PValues[T2Idx - 1];
	const FVector P2 = PValues[T2Idx];
	const FVector P3 = PValues[T2Idx + 1];

	// Modified from:
	// FMath::CubicCRSplineInterp(P0, P1, P2, P3, T0, T1, T2, T3, T);
	const float InvT1MinusT0 = 1.0f / (T1 - T0);
	const FVector L01 = P0 * T1 * InvT1MinusT0 - P1 * T0 * InvT1MinusT0;
	const float InvT2MinusT1 = 1.0f / (T2 - T1);
	const FVector L12 = P1 * T2 * InvT2MinusT1 - P2 * T1 * InvT2MinusT1;
	const float InvT3MinusT2 = 1.0f / (T3 - T2);
	const FVector L23 = P2 * T3 * InvT3MinusT2 - P3 * T2 * InvT3MinusT2;

	const float InvT2MinusT0 = 1.0f / (T2 - T0);
	const FVector L012 = L01 * T2 * InvT2MinusT0 - L12 * T0 * InvT2MinusT0;
	const float InvT3MinusT1 = 1.0f / (T3 - T1);
	const FVector L123 = L12 * T3 * InvT3MinusT1 - L23 * T1 * InvT3MinusT1;

	return L012 * T2 * InvT2MinusT1 - L123 * T1 * InvT2MinusT1;
}

float UCatmullRomSpline::FindParameterForDistance(float Distance) const
{
	const float ArcLength = DistanceLUT.Last();
	Distance = FMath::Clamp(Distance, 0.f, ArcLength);

	const int32 DistIdx = Algo::UpperBound(DistanceLUT, Distance);
	// DistanceLUT[DistIdx-1] <= Distance < DistanceLUT[DistIdx]

	if (DistIdx == DistanceLUT.Num())
	{
		return 1.f;
	}
	const float DistIdxF = static_cast<float>(DistIdx);

	const FVector2D DistanceRange(DistanceLUT[DistIdx - 1], DistanceLUT[DistIdx]);
	const FVector2D ParameterRange((DistIdxF - 1.f) * LUTParameterScale, DistIdx * LUTParameterScale);

	return FMath::GetMappedRangeValueUnclamped(DistanceRange, ParameterRange, Distance);
}

TArray<FVector> UCatmullRomSpline::EquidistantSamples(const float SampleLength) const
{
	if (!ensureMsgf(bValid, TEXT("Curve is not valid.")))
	{
		return TArray<FVector>();
	}

	const float ArcLength = DistanceLUT.Last();
	const int32 NumSegments = FMath::RoundToInt(ArcLength / SampleLength);
	const float ModifiedSampleLength = ArcLength / static_cast<float>(NumSegments);

	TArray<FVector> SamplePoints;
	SamplePoints.Reserve(NumSegments + 1);
	for (int32 SampleNum = 0; SampleNum <= NumSegments; SampleNum++)
	{
		SamplePoints.Add(SampleSplineByDistance(SampleNum * ModifiedSampleLength));
	}

	return SamplePoints;
}

void UCatmullRomSpline::FillLUT()
{
	if (!ensureMsgf(bValid, TEXT("Curve is not valid.")))
	{
		return;
	}

	// LUT Sampling rate
	constexpr float SamplesPerCm = 0.01f;

	const int32 TotalSamples = FMath::RoundToInt(PathSegmentsLength * SamplesPerCm);
	const float SampleInterval = 1.f / static_cast<float>(TotalSamples);

	// Fill distance LUT with accumulated distances
	DistanceLUT.Reset(TotalSamples + 1);
	DistanceLUT.Add(0.f);
	FVector LastPoint = PValues[1];
	for (float T = SampleInterval; T <= 1.f + SMALL_NUMBER; T += SampleInterval)
	{
		const FVector Sample = SampleSplineByParameter(T);
		DistanceLUT.Add(DistanceLUT.Last() + FVector::Distance(LastPoint, Sample));
		LastPoint = Sample;
	}

	LUTParameterScale = 1.f / static_cast<float>(DistanceLUT.Num() - 1);
}

//----------------------------------------------------------------------//
// FAsyncPathQueryHandle Implementation
//----------------------------------------------------------------------//

FAsyncPathQueryHandle FAsyncPathQueryHandle::InvalidHandle = FAsyncPathQueryHandle(INVALID_NAVQUERYID, nullptr);

bool FAsyncPathQueryHandle::IsValid() const
{
	return QueryID != INVALID_NAVQUERYID && FlyingNavData != nullptr;
}

//----------------------------------------------------------------------//
// UFlyingNavFunctionLibrary Implementation
//----------------------------------------------------------------------//

EPathfindingResult UFlyingNavFunctionLibrary::ConvertToPathfindingResult(const ENavigationQueryResult::Type Result)
{
	switch (Result)
	{
	case ENavigationQueryResult::Invalid:
		return EPathfindingResult::Invalid;
	case ENavigationQueryResult::Error:
		return EPathfindingResult::Error;
	case ENavigationQueryResult::Fail:
		return EPathfindingResult::Fail;
	case ENavigationQueryResult::Success:
		return EPathfindingResult::Success;
	}
	return EPathfindingResult::Error;
}

void UFlyingNavFunctionLibrary::RebuildAllFlyingNavigation(UWorld* World)
{
	// Iterate over NavDatas and rebuild all flying ones
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);

	if (NavSys)
	{
		for (ANavigationData* NavData : NavSys->NavDataSet)
		{
			AFlyingNavigationData* FlyingNavData = Cast<AFlyingNavigationData>(NavData);
			if (FlyingNavData)
			{
				FlyingNavData->RebuildAll();
			}
		}
	}
}

void UFlyingNavFunctionLibrary::RebuildAllFlyingNavigation(const UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (World)
	{
		RebuildAllFlyingNavigation(World);
	}
}

void UFlyingNavFunctionLibrary::DrawNavPath(
	UWorld* World,
	FNavPathSharedPtr NavPath,
	const FColor PathColor,
	const FVector& PathOffset,
	const bool bPersistent)
{
#if ENABLE_DRAW_DEBUG
	ANavigationData* NavData = NavPath->GetNavigationDataUsed();

	if (NavData)
	{
		// Copy to prevent crashes
		const TArray<FNavPathPoint> PathPoints = NavPath->GetPathPoints();
		const bool bGoalActor = (NavPath->GetGoalActor() != nullptr);

		// See FNavigationPath::DebugDraw
		static const FColor Grey(100, 100, 100);
		const int32 NumPathVerts = PathPoints.Num();

		for (int32 VertIdx = 0; VertIdx < NumPathVerts - 1; ++VertIdx)
		{
			// Draw box at each vertex
			FVector const VertLoc = PathPoints[VertIdx].Location + PathOffset;
			const FColor BoxColor = VertIdx < 0 ? Grey : PathColor;

			DrawDebugSolidBox(
				World,
				VertLoc,
				NavigationDebugDrawing::PathNodeBoxExtent,
				BoxColor,
				bPersistent);

			// Draw line to next location
			FVector const NextVertLoc = PathPoints[VertIdx + 1].Location + PathOffset;

			const FColor LineColor = VertIdx < -1 ? Grey : PathColor;
			DrawDebugLine(
				World,
				VertLoc,
				NextVertLoc,
				LineColor,
				bPersistent,
				-1.f,
				SDPG_World,
				NavigationDebugDrawing::PathLineThickness);
		}

		// Draw last vert
		if (NumPathVerts > 0)
		{
			DrawDebugBox(World,
			             PathPoints[NumPathVerts - 1].Location + PathOffset,
			             FVector(15.f),
			             PathColor,
			             bPersistent);
		}

		// if observing goal actor draw a radius and a line to the goal
		if (bGoalActor)
		{
			const FVector GoalLocation = NavPath->GetGoalLocation() + PathOffset;
			const FVector EndLocation = NavPath->GetEndLocation() + PathOffset;

			DrawDebugSphere(World, EndLocation, NavPath->GetGoalActorTetherDistance(), 16, PathColor, bPersistent);
			DrawDebugLine(World, EndLocation, GoalLocation, Grey, bPersistent);
		}
	}
#endif
}

void UFlyingNavFunctionLibrary::DrawNavPath(
	const UObject* WorldContextObject,
	UNavigationPath* NavPath,
	const FLinearColor PathColor,
	const FVector PathOffset,
	const bool bPersistent)
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);

	if (World && NavSys && NavPath && NavPath->GetPath().IsValid())
	{
		DrawNavPath(World, NavPath->GetPath(), PathColor.ToFColor(true), PathOffset, bPersistent);
	}
#endif // !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
}

UNavigationPath* UFlyingNavFunctionLibrary::SetNavigationPathPoints(
	const UObject* WorldContextObject,
	UNavigationPath* NavPath,
	const TArray<FVector>& PathPoints)
{
	UWorld* World = nullptr;
	if (WorldContextObject != nullptr)
	{
		World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	}
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
	ANavigationData* DefaultNavData = NavSys ? NavSys->GetDefaultNavDataInstance() : nullptr;
	const FNavPathSharedPtr OldPath = NavPath ? NavPath->GetPath() : nullptr;

	if (DefaultNavData != nullptr && OldPath != nullptr)
	{
		// Create a new path with the same query data
		FNavPathSharedPtr NewPath = NavSys->GetDefaultNavDataInstance()->CreatePathInstance<FNavigationPath>(
			OldPath->GetQueryData());
		TArray<FNavPathPoint>& NavPathPoints = NewPath->GetPathPoints();
		const uint32 NumPathPoints = PathPoints.Num();
		NavPathPoints.SetNumUninitialized(NumPathPoints);
		for (uint32 i = 0; i < NumPathPoints; i++)
		{
			NavPathPoints[i] = PathPoints[i]; // This constructs FNavPathPoint objects
		}
		NewPath->MarkReady();
		NavPath->SetPath(NewPath);
	}

	return NavPath;
}

AFlyingNavigationData* UFlyingNavFunctionLibrary::GetFlyingNavigationData(const APawn* NavAgent)
{
	if (!NavAgent)
	{
		return nullptr;
	}

	// Iterate over NavDatas and rebuild all flying ones
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(NavAgent->GetWorld());
	if (!NavSys)
	{
		return nullptr;
	}

	const INavAgentInterface* AsNavAgent = CastChecked<INavAgentInterface>(NavAgent);
	const FNavAgentProperties& AgentProps = AsNavAgent->GetNavAgentPropertiesRef();
	ANavigationData* NavData = NavSys->GetNavDataForProps(AgentProps, AsNavAgent->GetNavAgentLocation());

	if (NavData == nullptr)
	{
		NavData = NavSys->GetDefaultNavDataInstance(FNavigationSystem::DontCreate);
	}

	return Cast<AFlyingNavigationData>(NavData);
}

FAsyncPathQueryHandle UFlyingNavFunctionLibrary::FindPathAsync(
	UNavigationSystemV1* NavSys,
	const FNavAgentProperties& AgentProperties,
	FPathFindingQuery& Query,
	const FNavPathQueryDelegate& ResultDelegate,
	EPathFindingMode::Type Mode)
{
	if (Query.NavData.IsValid() == false)
	{
		Query.NavData = NavSys->GetNavDataForProps(AgentProperties, Query.StartLocation);
	}

	if (const AFlyingNavigationData* FlyingNavData = Cast<AFlyingNavigationData>(Query.NavData.Get()))
	{
		uint32 QueryID;
		if (UFlyingNavigationSystem* FlyingNavSys = Cast<UFlyingNavigationSystem>(NavSys))
		{
			QueryID = FlyingNavSys->FindPathAsyncMultiFrame(AgentProperties, Query, ResultDelegate, Mode);
		}
		else
		{
			QueryID = NavSys->FindPathAsync(AgentProperties, Query, ResultDelegate, Mode);
		}
		

		if (QueryID != INVALID_NAVQUERYID)
		{
			FlyingNavData->IncrementCalculatingPaths();

			return FAsyncPathQueryHandle(QueryID, FlyingNavData);
		}
	}

	return FAsyncPathQueryHandle::InvalidHandle;
}

void UFlyingNavFunctionLibrary::AbortAsyncFindPathRequest(UNavigationSystemV1* NavSys,
                                                          const FAsyncPathQueryHandle Handle)
{
	// If the handle is valid, AbortAsyncFindPathRequest should always remove an async query
	if (Handle.IsValid())
	{
		Handle.FlyingNavData->DecrementCalculatingPaths();

		if (UFlyingNavigationSystem* FlyingNavSys = Cast<UFlyingNavigationSystem>(NavSys))
		{
			FlyingNavSys->AbortAsyncFindPathRequestMultiFrame(Handle.QueryID);
		}
		else
		{
			NavSys->AbortAsyncFindPathRequest(Handle.QueryID);
		}
	}
}

UNavigationPath* UFlyingNavFunctionLibrary::FindPathToLocationAsynchronously(
	const UObject* WorldContextObject,
	const FLatentActionInfo& LatentInfo,
	const FVector& PathStart,
	const FVector& PathEnd,
	AActor* const PathfindingContext,
	AActor* const GoalActor,
	const float TetherDistance,
	const TSubclassOf<UNavigationQueryFilter> FilterClass)
{
	// See: UNavigationSystemV1::FindPathToLocationSynchronously
	UWorld* World = nullptr;

	if (WorldContextObject != nullptr)
	{
		World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	}
	if (World == nullptr && PathfindingContext != nullptr)
	{
		World = GEngine->GetWorldFromContextObject(PathfindingContext, EGetWorldErrorMode::LogAndReturnNull);
	}
	if (World == nullptr)
	{
		printw("World == nullptr.")
		return nullptr;
	}

	FLatentActionManager& LatentActionManager = World->GetLatentActionManager();

	UNavigationPath* ResultPath = nullptr;

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);

	if (NavSys != nullptr && NavSys->GetDefaultNavDataInstance() != nullptr)
	{
		ResultPath = NewObject<UNavigationPath>(NavSys);
		bool bValidPathContext = false;
		ANavigationData* NavigationData = nullptr;

		if (PathfindingContext != nullptr)
		{
			const INavAgentInterface* NavAgent = Cast<INavAgentInterface>(PathfindingContext);

			if (NavAgent != nullptr)
			{
				const FNavAgentProperties& AgentProps = NavAgent->GetNavAgentPropertiesRef();
				NavigationData = NavSys->GetNavDataForProps(AgentProps, PathStart);
				bValidPathContext = true;
			}
			else if (Cast<ANavigationData>(PathfindingContext))
			{
				NavigationData = static_cast<ANavigationData*>(PathfindingContext);
				bValidPathContext = true;
			}
		}
		if (bValidPathContext == false)
		{
			// just use default
			NavigationData = NavSys->GetDefaultNavDataInstance();
		}

		check(NavigationData);

		const AFlyingNavigationData* FlyingNavData = Cast<AFlyingNavigationData>(NavigationData);
		if (FlyingNavData == nullptr)
		{
			printw("Async pathfinding query doesn't use Flying Navigation. Please use FindPathToLocationSynchronously instead for Recast queries.")
			LATENT_RETURN_NOW()
			return ResultPath;
		}

		FPathFindingQuery Query(PathfindingContext,
		                        *NavigationData,
		                        PathStart,
		                        PathEnd,
		                        UNavigationQueryFilter::GetQueryFilter(*NavigationData,
		                                                               PathfindingContext,
		                                                               FilterClass));
		// Flying Nav Data should ignore this anyway, but just in case...
		Query.bAllowPartialPaths = false;

		// Create NavPath early, to set GoalPawn
		if (GoalActor && GoalActor->IsA(APawn::StaticClass()))
		{
			const FNavPathSharedPtr NavPath = NavigationData->CreatePathInstance<FFlyingNavigationPath>(Query);
			static_cast<FFlyingNavigationPath*>(NavPath.Get())->GoalPawn = static_cast<APawn*>(GoalActor);
			Query.SetPathInstanceToUpdate(NavPath);
		}

		// Make sure subsequent updates are async as well
		Query.NavDataFlags = EFlyingNavPathFlags::AsyncObservationUpdate;

		// We need to bind to the result delegate before passing it to FindPathAsync (it get copied)
		FNavPathQueryDelegate ResultDelegate;
		FPathFindAction* PathFindAction = new FPathFindAction(ResultPath,
		                                                      ResultDelegate,
		                                                      LatentInfo,
		                                                      GoalActor,
		                                                      TetherDistance);
		
		const FAsyncPathQueryHandle QueryHandle = FindPathAsync(NavSys,
		                                                        FNavAgentProperties::DefaultProperties,
		                                                        Query,
		                                                        ResultDelegate);
		if (QueryHandle.IsValid())
		{
			PathFindAction->SetQueryHandle(QueryHandle);
			LatentActionManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, PathFindAction);
		}
		else
		{
			delete PathFindAction;
			printw("Request for Async Pathfinding failed on %s. Check navigation data is set up correctly.",
				PathfindingContext ? *PathfindingContext->GetName() : *NavigationData->GetName())
			// Run callback anyway, returning nullptr
			LATENT_RETURN_NOW()
		}
	}
	else
	{
		if (NavSys == nullptr)
		{
			printw("NavSys == nullptr.")
		}
		else
		{
			printw("NavSys->GetDefaultNavDataInstance() == nullptr. Check a valid default agent is set in Project Settings and a Nav Mesh Bounds Volume is present in the level.")
		}

		if (World)
		{
			// Run callback anyway, returning nullptr
			LATENT_RETURN_NOW()
		}
	}

	return ResultPath;
}


UNavigationPath* UFlyingNavFunctionLibrary::FindPathToActorAsynchronously(
	UObject* WorldContextObject,
	const FLatentActionInfo LatentInfo,
	const FVector& PathStart,
	AActor* GoalActor,
	float TetherDistance,
	AActor* PathfindingContext,
	TSubclassOf<UNavigationQueryFilter> FilterClass)
{
	if (GoalActor == nullptr)
	{
		printw("Can't find path to null actor")
		return nullptr;
	}

	INavAgentInterface* NavAgent = Cast<INavAgentInterface>(GoalActor);
	UNavigationPath* GeneratedPath = FindPathToLocationAsynchronously(
		WorldContextObject,
		LatentInfo,
		PathStart,
		NavAgent ? NavAgent->GetNavAgentLocation() : GoalActor->GetActorLocation(),
		PathfindingContext,
		GoalActor,
		TetherDistance,
		FilterClass
	);

	return GeneratedPath;
}

EPathfindingResult UFlyingNavFunctionLibrary::GetPathfindingResult(UNavigationPath* Path)
{
	if (Path && Path->GetPath())
	{
		if (const FFlyingNavigationPath* FlyingPath = Path->GetPath()->CastPath<FFlyingNavigationPath>())
		{
			return ConvertToPathfindingResult(FlyingPath->QueryResult);
		}
		return EPathfindingResult::RecastError;
	}
	return EPathfindingResult::Null;
}

bool UFlyingNavFunctionLibrary::IsPositionAValidEndpoint(const APawn* NavAgent,
                                                         const FVector& Position,
                                                         const bool bAllowBlocked)
{
	AFlyingNavigationData* FlyingNavData = GetFlyingNavigationData(NavAgent);
	if (FlyingNavData == nullptr)
	{
		printw("NavAgent doesn't use Flying Navigation")
		return false;
	}
	FRWScopeLock Lock(FlyingNavData->SVODataLock, SLT_ReadOnly);
	const FSVOData& SVOData = FlyingNavData->GetSVOData();
	return SVOData.GetNodeLinkForPosition(Position, bAllowBlocked).IsValid();
}

FAIRequestID UFlyingNavFunctionLibrary::RequestMove(UNavigationPath* PathToFollow,
                                                    AAIController* Controller)
{
	UPathFollowingComponent* PathFollowingComponent = Controller ? Controller->GetPathFollowingComponent() : nullptr;
	if (Controller && PathFollowingComponent && PathToFollow && PathToFollow->IsValid())
	{
		FAIMoveRequest MoveReq(PathToFollow->GetPath()->GetDestinationLocation());
		MoveReq.SetAllowPartialPath(false);
		MoveReq.SetAcceptanceRadius(200.f);
		return PathFollowingComponent->RequestMove(MoveReq, PathToFollow->GetPath());
	}
	else
	{
		printw("Valid Controller: %s, Valid PathToFollow: %s",
		       Controller != nullptr ? TEXT("True") : TEXT("False"),
		       (PathToFollow && PathToFollow->IsValid()) ? TEXT("True") : TEXT("False"))
		return FAIRequestID::InvalidRequest;
	}
}

FVector UFlyingNavFunctionLibrary::GetActorFeetLocation(const APawn* Pawn)
{
	if (Pawn == nullptr)
	{
		return FVector::ZeroVector;
	}
	const UPawnMovementComponent* PawnMovementComponent = Pawn->GetMovementComponent();
	if (PawnMovementComponent)
	{
		return PawnMovementComponent->GetActorFeetLocation();
	}
	return Pawn->GetActorLocation();
}

FVector UFlyingNavFunctionLibrary::GetActorFeetOffset(const APawn* Pawn)
{
	if (Pawn == nullptr)
	{
		return FVector::ZeroVector;
	}
	return Pawn->GetActorLocation() - GetActorFeetLocation(Pawn);
}

UNavigationPath* UFlyingNavFunctionLibrary::SmoothPath(const UObject* WorldContextObject,
                                                       UNavigationPath* Path,
                                                       const float SampleLength)
{
	if (Path == nullptr || !Path->IsValid())
	{
		return Path;
	}

	// Convert from TArray<FNavPathPoint> to TArray<FVector>
	TArray<FVector> PathPoints;
	PathPoints.Reserve(PathPoints.Num());
	for (const FNavPathPoint& NavPathPoint : Path->GetPath()->GetPathPoints())
	{
		PathPoints.Add(NavPathPoint);
	}

	return SetNavigationPathPoints(WorldContextObject, Path, SmoothPathPoints(PathPoints, SampleLength));
}

TArray<FVector> UFlyingNavFunctionLibrary::SmoothPathPoints(const TArray<FVector>& PathPoints,
                                                            const float SampleLength)
{
	TArray<FVector> SmoothedPoints;

	// Generate a spline and sample
	const UCatmullRomSpline* CRSpline = MakeCatmullRomSpline(PathPoints);
	if (CRSpline != nullptr)
	{
		SmoothedPoints = CRSpline->EquidistantSamples(SampleLength);
	}

	return SmoothedPoints;
}

void UFlyingNavFunctionLibrary::SmoothPathPoints(TArray<FNavPathPoint>& NavPathPoints,
                                                 const float SampleLength)
{
	// Convert from TArray<FNavPathPoint> to TArray<FVector>
	TArray<FVector> PathPoints;
	PathPoints.Reserve(NavPathPoints.Num());
	for (const FNavPathPoint& NavPathPoint : NavPathPoints)
	{
		PathPoints.Add(NavPathPoint);
	}

	const TArray<FVector> SmoothedPoints = SmoothPathPoints(PathPoints, SampleLength);

	// Convert from TArray<FVector> to TArray<FNavPathPoint>
	const uint32 NumPathPoints = SmoothedPoints.Num();
	NavPathPoints.SetNumUninitialized(NumPathPoints);
	for (uint32 i = 0; i < NumPathPoints; i++)
	{
		NavPathPoints[i] = SmoothedPoints[i]; // This constructs FNavPathPoint objects
	}
}
