// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include <GameFramework/PlayerController.h>
#include "AITypes.h"
#include <Navigation/PathFollowingComponent.h>
#include <NavigationSystemTypes.h>

#include "GravityPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMoveCompletedSignature, FAIRequestID, RequestID, EPathFollowingResult::Type, Result);

class UPathFollowingComponent;
class UNavigationQueryFilter;

UCLASS(config = Game)
class GRAVITY_API AGravityPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	AGravityPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

#if USECUSTOMEGRAVITY
	virtual void UpdateRotation(float DeltaTime)override;

	// ��¼Player��Yaw��Pitch
	FRotator ControlRotationWithoutGravityTrans = FRotator::ZeroRotator;
#endif

	EPathFollowingRequestResult::Type MoveToLocation(
		const FVector& Dest, 
		float AcceptanceRadius = -1, 
		bool bStopOnOverlap = true,
		bool bUsePathfinding = true,
		bool bProjectDestinationToNavigation = false,
		bool bCanStrafe = true,
		TSubclassOf<UNavigationQueryFilter> FilterClass = NULL, 
		bool bAllowPartialPath = true
	);

	virtual void StopMovement() override;

	/** Blueprint notification that we've completed the current movement request */
	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "MoveCompleted"))
	FMoveCompletedSignature ReceiveMoveCompleted;

protected:

	 virtual void OnPossess(APawn* InPawn) override;
	 virtual void OnUnPossess() override;

	virtual FPathFollowingRequestResult MoveTo(const FAIMoveRequest& MoveRequest, FNavPathSharedPtr* OutPath = nullptr);

	virtual FAIRequestID RequestMove(const FAIMoveRequest& MoveRequest, FNavPathSharedPtr Path);

	bool BuildPathfindingQuery(
		const FAIMoveRequest& MoveRequest, FPathFindingQuery& Query
	) const;

	virtual void FindPathForMoveRequest(
		const FAIMoveRequest& MoveRequest, FPathFindingQuery& Query, FNavPathSharedPtr& OutPath
	) const;

	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	TSubclassOf<UNavigationQueryFilter> DefaultNavigationFilterClass;

	/** Component used for moving along a path. */
	UPROPERTY(VisibleDefaultsOnly, Category = AI)
	TObjectPtr<UPathFollowingComponent> PathFollowingComponent;
	
	/** Is strafing allowed during movement? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	uint32 bAllowStrafe : 1;

};