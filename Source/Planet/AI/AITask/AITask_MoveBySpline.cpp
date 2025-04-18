#include "AITask_MoveBySpline.h"

#include "UObject/Package.h"
#include "TimerManager.h"
#include "AISystem.h"
#include "VisualLogger/VisualLogger.h"
#include "AIResources.h"
#include "GameplayTasksComponent.h"
#include "AIController.h"
#include "Components/SplineComponent.h"

#include "PlanetAIController.h"
#include "PlanetPlayerController.h"

#ifdef WITH_EDITOR
static TAutoConsoleVariable<int32> ITask_MoveBySpline(
	TEXT("ITask_MoveBySpline"),
	0,
	TEXT("")
	TEXT(" default: 0")
);
#endif

void UAITask_MoveBySpline::Activate()
{
	Super::Activate();
}

void UAITask_MoveBySpline::PerformMove()
{
	UPathFollowingComponent* PFComp = OwnerController ? OwnerController->GetPathFollowingComponent() : nullptr;
	if (PFComp == nullptr)
	{
		FinishMoveTask(EPathFollowingResult::Invalid);
		return;
	}

	ResetObservers();
	ResetTimers();

	auto Location = SPlinePtr->FindLocationClosestToWorldLocation(GetAvatarActor()->GetActorLocation(),ESplineCoordinateSpace::World);
	MoveRequest.UpdateGoalLocation(Location);

#ifdef WITH_EDITOR
	if (ITask_MoveBySpline.GetValueOnGameThread())
	{
		DrawDebugSphere(OwnerController->GetWorld(), Location, 20,20,FColor::Red, false, 10);
	}
#endif

	// start new move request
	FNavPathSharedPtr FollowedPath;
	FPathFollowingRequestResult ResultData;
	if (bIsReachedSPline)
	{
		auto PCPtr = Cast<APlanetAIController>(GetAIController());
		if (PCPtr)
		{
			ResultData = PCPtr->MoveAlongSPline(SPlinePtr, MoveRequest, &FollowedPath);
		}
	}
	else
	{
		ResultData = OwnerController->MoveTo(MoveRequest, &FollowedPath);
	}

	switch (ResultData.Code)
	{
	case EPathFollowingRequestResult::Failed:
		FinishMoveTask(EPathFollowingResult::Invalid);
		break;

	case EPathFollowingRequestResult::AlreadyAtGoal:
		MoveRequestID = ResultData.MoveId;
		OnRequestFinished(ResultData.MoveId,
		                  FPathFollowingResult(EPathFollowingResult::Success,
		                                       FPathFollowingResultFlags::AlreadyAtGoal));
		break;

	case EPathFollowingRequestResult::RequestSuccessful:
		MoveRequestID = ResultData.MoveId;
		PathFinishDelegateHandle = PFComp->OnRequestFinished.AddUObject(this, &ThisClass::OnRequestFinished);
		SetObservedPath(FollowedPath);

		if (IsFinished())
		{
			UE_VLOG(GetGameplayTasksComponent(), LogGameplayTasks, Error, TEXT("%s> re-Activating Finished task!"),
			        *GetName());
		}
		break;

	default:
		checkNoEntry();
		break;
	}
}

void UAITask_MoveBySpline::OnRequestFinished(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	if (RequestID == MoveRequestID)
	{
		bIsReachedSPline = true;
		UE_VLOG(GetGameplayTasksComponent(), LogGameplayTasks, Log, TEXT("%s> received OnRequestFinished and goal tracking is active! Moving again in next tick"), *GetName());
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::PerformMove);
	}
}
