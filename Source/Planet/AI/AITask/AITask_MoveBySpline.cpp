
#include "AITask_MoveBySpline.h"

#include "UObject/Package.h"
#include "TimerManager.h"
#include "AISystem.h"
#include "AIController.h"
#include "VisualLogger/VisualLogger.h"
#include "AIResources.h"
#include "GameplayTasksComponent.h"

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

	if (Pts.IsValidIndex(Index))
	{
		MoveRequest.UpdateGoalLocation(Pts[Index]);
	}
	else
	{
		FinishMoveTask(EPathFollowingResult::Invalid);
		return;
	}

	// start new move request
	FNavPathSharedPtr FollowedPath;
	const FPathFollowingRequestResult ResultData = OwnerController->MoveTo(MoveRequest, &FollowedPath);

	switch (ResultData.Code)
	{
	case EPathFollowingRequestResult::Failed:
		FinishMoveTask(EPathFollowingResult::Invalid);
		break;

	case EPathFollowingRequestResult::AlreadyAtGoal:
		MoveRequestID = ResultData.MoveId;
		OnRequestFinished(ResultData.MoveId, FPathFollowingResult(EPathFollowingResult::Success, FPathFollowingResultFlags::AlreadyAtGoal));
		break;

	case EPathFollowingRequestResult::RequestSuccessful:
		MoveRequestID = ResultData.MoveId;
		PathFinishDelegateHandle = PFComp->OnRequestFinished.AddUObject(this, &ThisClass::OnRequestFinished);
		SetObservedPath(FollowedPath);

		if (IsFinished())
		{
			UE_VLOG(GetGameplayTasksComponent(), LogGameplayTasks, Error, TEXT("%s> re-Activating Finished task!"), *GetName());
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
// 		if (MyMoveTaskCompletedSignature.IsBound())
// 		{
// 			MyMoveTaskCompletedSignature.Execute(Result.Code);
// 		}
		Index++;
		if (Index >= Pts.Num())
		{
			Index = 0;
		}
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UAITask_MoveBySpline::PerformMove);
	}
	else if (IsActive())
	{
		UE_VLOG(GetGameplayTasksComponent(), LogGameplayTasks, Warning, TEXT("%s> received OnRequestFinished with not matching RequestID!"), *GetName());
	}
}
// 
// FMyMoveTaskCompletedSignature UAITask_MoveBySpline::GetMoveTaskCompletedSignature()
// {
// 	return MyMoveTaskCompletedSignature;
// }
// 
