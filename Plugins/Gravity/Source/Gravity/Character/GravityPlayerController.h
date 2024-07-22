// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include <GameFramework/PlayerController.h>
#include "AITypes.h"
#include <Navigation/PathFollowingComponent.h>
#include <NavigationSystemTypes.h>

#include "GravityPlayerController.generated.h"

class UPathFollowingComponent;
class UNavigationQueryFilter;

UCLASS(config = Game)
class GRAVITY_API AGravityPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	AGravityPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void UpdateRotation(float DeltaTime)override;

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

	virtual void StopMovement() override;

	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	TSubclassOf<UNavigationQueryFilter> DefaultNavigationFilterClass;

	// ¼ÇÂ¼PlayerµÄYawºÍPitch
	FRotator ControlRotationWithoutGravityTrans = FRotator::ZeroRotator;
	
	/** Component used for moving along a path. */
	UPROPERTY(VisibleDefaultsOnly, Category = AI)
	TObjectPtr<UPathFollowingComponent> PathFollowingComponent;
	
	/** Is strafing allowed during movement? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	uint32 bAllowStrafe : 1;

};
