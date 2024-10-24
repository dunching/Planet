
#include "GravityPlayerController.h"

#include <Engine/Engine.h>
#include <GameFramework/Character.h>
#include <IXRTrackingSystem.h>
#include <IXRCamera.h>
#include <Kismet/KismetMathLibrary.h>
#include <Navigation/CrowdFollowingComponent.h>
#include <NavigationSystem.h>
#include <NavFilters/NavigationQueryFilter.h>

#include "GravityCharacter.h"
#include "GravityMovementComponent.h"

AGravityPlayerController::AGravityPlayerController(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/):
    Super(ObjectInitializer)
{
	// 这个在 “ROLE_AutonomousProxy”上不正确？
	// PathFollowingComponent = CreateOptionalDefaultSubobject<UCrowdFollowingComponent>(TEXT("PathFollowingComponent"));
	PathFollowingComponent = CreateOptionalDefaultSubobject<UPathFollowingComponent>(TEXT("PathFollowingComponent"));
	if (PathFollowingComponent)
	{
	PathFollowingComponent->OnRequestFinished.AddUObject(this, &ThisClass::OnMoveCompleted);
	}
}

#if USECUSTOMEGRAVITY
void AGravityPlayerController::UpdateRotation(float DeltaTime)
{
    APawn* const P = GetPawnOrSpectator();
    if (P)
    {
        FRotator ViewRotation = ControlRotationWithoutGravityTrans;

        // Calculate Delta to be applied on ViewRotation
        FRotator DeltaRot(RotationInput);

        if (PlayerCameraManager)
        {
            PlayerCameraManager->ProcessViewRotation(DeltaTime, ViewRotation, DeltaRot);
        }

        AActor* ViewTarget = GetViewTarget();
        if (!PlayerCameraManager || !ViewTarget || !ViewTarget->HasActiveCameraComponent() || ViewTarget->HasActivePawnControlCameraComponent())
        {
            if (IsLocalPlayerController() && GEngine->XRSystem.IsValid() && GetWorld() != nullptr && GEngine->XRSystem->IsHeadTrackingAllowedForWorld(*GetWorld()))
            {
                auto XRCamera = GEngine->XRSystem->GetXRCamera();
                if (XRCamera.IsValid())
                {
                    XRCamera->ApplyHMDRotation(this, ViewRotation);
                }
            }
        }

        ControlRotationWithoutGravityTrans = ViewRotation;

        ViewRotation = (P->GetGravityTransform() * ViewRotation.Quaternion()).Rotator();

        SetControlRotation(ViewRotation);

#if WITH_EDITOR
        RootComponent->SetWorldLocation(P->GetActorLocation());
#endif

        P->FaceRotation(ViewRotation, DeltaTime);
    }
}
#endif

void AGravityPlayerController::MoveToLocation/*_Implementation*/(
    const FVector& Dest,
	const AActor* InGoalActor,
    float AcceptanceRadius, 
    bool bStopOnOverlap, 
    bool bUsePathfinding, 
    bool bProjectDestinationToNavigation, 
    bool bCanStrafe, 
    TSubclassOf<UNavigationQueryFilter> FilterClass, 
    bool bAllowPartialPaths
)
{
//	// abort active movement to keep only one request running
//	if (PathFollowingComponent && PathFollowingComponent->GetStatus() != EPathFollowingStatus::Idle)
//	{
//		PathFollowingComponent->AbortMove(*this, FPathFollowingResultFlags::ForcedScript | FPathFollowingResultFlags::NewRequest
//			, FAIRequestID::CurrentRequest, EPathFollowingVelocityMode::Keep);
//	}
//
//	FAIMoveRequest MoveReq;
//	if (InGoalActor)
//	{
//		MoveReq.SetGoalActor(InGoalActor);
//	}
//	else
//	{
//		MoveReq.SetGoalLocation(Dest);
//	}
//
//	MoveReq.SetAcceptanceRadius(AcceptanceRadius);
//	MoveReq.SetUsePathfinding(bUsePathfinding);
//	MoveReq.SetAllowPartialPath(bAllowPartialPaths);
//	MoveReq.SetProjectGoalLocation(bProjectDestinationToNavigation);
////	MoveReq.SetNavigationFilter(*FilterClass ? FilterClass : DefaultNavigationFilterClass);
//	MoveReq.SetReachTestIncludesAgentRadius(bStopOnOverlap);
//	MoveReq.SetCanStrafe(bCanStrafe);
//
///*	return */MoveTo(MoveReq);

  	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
  	if (NavSys == nullptr || GetPawn() == nullptr)
  	{
  		return;
  	}
  
 	UPathFollowingComponent* PFollowComp = PathFollowingComponent;
  
  	if (PFollowComp == nullptr)
  	{
  		return;
  	}
  
  	if (!PFollowComp->IsPathFollowingAllowed())
  	{
  		return;
  	}
  
  	const bool bAlreadyAtGoal = InGoalActor ?
  		PFollowComp->HasReached(*InGoalActor, EPathFollowingReachMode::OverlapAgentAndGoal) :
  		PFollowComp->HasReached(Dest, EPathFollowingReachMode::OverlapAgentAndGoal);
  
  
  	// script source, keep only one move request at time
  	if (PFollowComp->GetStatus() != EPathFollowingStatus::Idle)
  	{
  		PFollowComp->AbortMove(*NavSys, FPathFollowingResultFlags::ForcedScript | FPathFollowingResultFlags::NewRequest
  			, FAIRequestID::AnyRequest, bAlreadyAtGoal ? EPathFollowingVelocityMode::Reset : EPathFollowingVelocityMode::Keep);
  	}
  
  	if (bAlreadyAtGoal)
  	{
  		PFollowComp->RequestMoveWithImmediateFinish(EPathFollowingResult::Success);
  	}
  	else
  	{
  		const FVector AgentNavLocation = GetNavAgentLocation();
  		const ANavigationData* NavData = NavSys->GetNavDataForProps(GetNavAgentPropertiesRef(), AgentNavLocation);
  		if (NavData)
  		{
  			if (InGoalActor)
  			{
  				FPathFindingQuery Query(this, *NavData, AgentNavLocation, InGoalActor->GetActorLocation());
  				FPathFindingResult Result = NavSys->FindPathSync(Query);
  				if (Result.IsSuccessful())
  				{
  					Result.Path->SetGoalActorObservation(*InGoalActor, 100.0f);
  					PFollowComp->RequestMove(FAIMoveRequest(InGoalActor), Result.Path);
  				}
  				else if (PFollowComp->GetStatus() != EPathFollowingStatus::Idle)
  				{
  					PFollowComp->RequestMoveWithImmediateFinish(EPathFollowingResult::Invalid);
  				}
  			}
  			else
  			{
  				FPathFindingQuery Query(this, *NavData, AgentNavLocation, Dest);
  				FPathFindingResult Result = NavSys->FindPathSync(Query);
  				if (Result.IsSuccessful())
  				{
  					PFollowComp->RequestMove(FAIMoveRequest(Dest), Result.Path);
  				}
  				else if (PFollowComp->GetStatus() != EPathFollowingStatus::Idle)
  				{
  					PFollowComp->RequestMoveWithImmediateFinish(EPathFollowingResult::Invalid);
  				}
  			}
  		}
  	}
}

void AGravityPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AGravityPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();

	if (GetLocalRole() < ROLE_Authority)
	{
		if (PathFollowingComponent)
		{
			PathFollowingComponent->Initialize();
		}
	}
}

void AGravityPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (GetPawn() == nullptr || InPawn == nullptr)
	{
		return;
	}

	if (GetLocalRole() == ROLE_Authority)
	{
		if (PathFollowingComponent)
		{
			PathFollowingComponent->Initialize();
		}
	}
}

void AGravityPlayerController::OnUnPossess()
{
	Super::OnUnPossess();

 	if (PathFollowingComponent)
 	{
 		PathFollowingComponent->Cleanup();
 	}
}

FPathFollowingRequestResult AGravityPlayerController::MoveTo(const FAIMoveRequest& MoveRequest, FNavPathSharedPtr* OutPath)
{
	// both MoveToActor and MoveToLocation can be called from blueprints/script and should keep only single movement request at the same time.
	// this function is entry point of all movement mechanics - do NOT abort in here, since movement may be handled by AITasks, which support stacking 

	FPathFollowingRequestResult ResultData;
	ResultData.Code = EPathFollowingRequestResult::Failed;

	if (MoveRequest.IsValid() == false)
	{
		return ResultData;
	}

	if (PathFollowingComponent == nullptr)
	{
		return ResultData;
	}

	bool bCanRequestMove = true;
	bool bAlreadyAtGoal = false;

	if (!MoveRequest.IsMoveToActorRequest())
	{
		if (MoveRequest.GetGoalLocation().ContainsNaN() || FAISystem::IsValidLocation(MoveRequest.GetGoalLocation()) == false)
		{
			bCanRequestMove = false;
		}

		// fail if projection to navigation is required but it failed
		if (bCanRequestMove && MoveRequest.IsProjectingGoal())
		{
			UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
			const FNavAgentProperties& AgentProps = GetNavAgentPropertiesRef();
			FNavLocation ProjectedLocation;

			if (NavSys && !NavSys->ProjectPointToNavigation(MoveRequest.GetGoalLocation(), ProjectedLocation, INVALID_NAVEXTENT, &AgentProps))
			{
				if (MoveRequest.IsUsingPathfinding())
				{
				}
				else
				{
				}

				bCanRequestMove = false;
			}

			MoveRequest.UpdateGoalLocation(ProjectedLocation.Location);
		}

		bAlreadyAtGoal = bCanRequestMove && PathFollowingComponent->HasReached(MoveRequest);
	}
	else
	{
		bAlreadyAtGoal = bCanRequestMove && PathFollowingComponent->HasReached(MoveRequest);
	}

	if (bAlreadyAtGoal)
	{
		ResultData.MoveId = PathFollowingComponent->RequestMoveWithImmediateFinish(EPathFollowingResult::Success);
		ResultData.Code = EPathFollowingRequestResult::AlreadyAtGoal;
	}
	else if (bCanRequestMove)
	{
		FPathFindingQuery PFQuery;

		const bool bValidQuery = BuildPathfindingQuery(MoveRequest, PFQuery);
		if (bValidQuery)
		{
			FNavPathSharedPtr Path;
			FindPathForMoveRequest(MoveRequest, PFQuery, Path);

			const FAIRequestID RequestID = Path.IsValid() ? RequestMove(MoveRequest, Path) : FAIRequestID::InvalidRequest;
			if (RequestID.IsValid())
			{
				bAllowStrafe = MoveRequest.CanStrafe();
				ResultData.MoveId = RequestID;
				ResultData.Code = EPathFollowingRequestResult::RequestSuccessful;

				if (OutPath)
				{
					*OutPath = Path;
				}
			}
		}
	}

	if (ResultData.Code == EPathFollowingRequestResult::Failed)
	{
		ResultData.MoveId = PathFollowingComponent->RequestMoveWithImmediateFinish(EPathFollowingResult::Invalid);
	}

	return ResultData;
}

FAIRequestID AGravityPlayerController::RequestMove(const FAIMoveRequest& MoveRequest, FNavPathSharedPtr Path)
{
	uint32 RequestID = FAIRequestID::InvalidRequest;
	if (PathFollowingComponent)
	{
		RequestID = PathFollowingComponent->RequestMove(MoveRequest, Path);
	}

	return RequestID;
}

bool AGravityPlayerController::BuildPathfindingQuery(const FAIMoveRequest& MoveRequest, FPathFindingQuery& Query) const
{
	bool bResult = false;

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	const ANavigationData* NavData = (NavSys == nullptr) ? nullptr :
		MoveRequest.IsUsingPathfinding() ? NavSys->GetNavDataForProps(GetNavAgentPropertiesRef(), GetNavAgentLocation()) :
		NavSys->GetAbstractNavData();

	if (NavData)
	{
		FVector GoalLocation = MoveRequest.GetGoalLocation();
		if (MoveRequest.IsMoveToActorRequest())
		{
			const INavAgentInterface* NavGoal = Cast<const INavAgentInterface>(MoveRequest.GetGoalActor());
			if (NavGoal)
			{
				const FVector Offset = NavGoal->GetMoveGoalOffset(this);
				GoalLocation = FQuatRotationTranslationMatrix(MoveRequest.GetGoalActor()->GetActorQuat(), NavGoal->GetNavAgentLocation()).TransformPosition(Offset);
			}
			else
			{
				GoalLocation = MoveRequest.GetGoalActor()->GetActorLocation();
			}
		}

		FSharedConstNavQueryFilter NavFilter = UNavigationQueryFilter::GetQueryFilter(*NavData, this, MoveRequest.GetNavigationFilter());
		Query = FPathFindingQuery(*this, *NavData, GetNavAgentLocation(), GoalLocation, NavFilter);
		Query.SetAllowPartialPaths(MoveRequest.IsUsingPartialPaths());
		Query.SetRequireNavigableEndLocation(MoveRequest.IsNavigableEndLocationRequired());
		if (MoveRequest.IsApplyingCostLimitFromHeuristic())
		{
			const float HeuristicScale = NavFilter->GetHeuristicScale();
			Query.CostLimit = FPathFindingQuery::ComputeCostLimitFromHeuristic(Query.StartLocation, Query.EndLocation, HeuristicScale, MoveRequest.GetCostLimitFactor(), MoveRequest.GetMinimumCostLimit());
		}

		if (PathFollowingComponent)
		{
			PathFollowingComponent->OnPathfindingQuery(Query);
		}

		bResult = true;
	}
	else
	{
		if (NavSys == nullptr)
		{
		}
		else
		{
		}
	}

	return bResult;
}

void AGravityPlayerController::FindPathForMoveRequest(const FAIMoveRequest& MoveRequest, FPathFindingQuery& Query, FNavPathSharedPtr& OutPath) const
{
	SCOPE_CYCLE_COUNTER(STAT_AI_Overall);

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (NavSys)
	{
		FPathFindingResult PathResult = NavSys->FindPathSync(Query);
		if (PathResult.Result != ENavigationQueryResult::Error)
		{
			if (PathResult.IsSuccessful() && PathResult.Path.IsValid())
			{
				if (MoveRequest.IsMoveToActorRequest())
				{
					PathResult.Path->SetGoalActorObservation(*MoveRequest.GetGoalActor(), 100.0f);
				}

				PathResult.Path->EnableRecalculationOnInvalidation(true);
				OutPath = PathResult.Path;
			}
		}
		else
		{
		}
	}
}

void AGravityPlayerController::StopMovement()
{
	Super::StopMovement();

// 	if (PathFollowingComponent)
// 	{
// 		PathFollowingComponent->AbortMove(*this, FPathFollowingResultFlags::MovementStop | FPathFollowingResultFlags::ForcedScript);
// 	}
}

void AGravityPlayerController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	ReceiveMoveCompleted.ExecuteIfBound(RequestID, Result.Code);
}
