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

	DECLARE_DELEGATE_TwoParams(FMoveCompletedSignature, FAIRequestID, EPathFollowingResult::Type);

	AGravityPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

#if USECUSTOMEGRAVITY
	virtual void UpdateRotation(float DeltaTime)override;

	// 记录Player的Yaw和Pitch
	FRotator ControlRotationWithoutGravityTrans = FRotator::ZeroRotator;
#endif

	UFUNCTION(NetMulticast, Reliable)
	void MoveToLocation_RPC(
		const FVector& Dest, 
		const AActor* InGoalActor,
		float AcceptanceRadius = -1, 
		bool bStopOnOverlap = true,
		bool bUsePathfinding = true,
		bool bProjectDestinationToNavigation = false,
		bool bCanStrafe = false,
		TSubclassOf<UNavigationQueryFilter> FilterClass = NULL, 
		bool bAllowPartialPath = true
	);
	
	virtual void StopMovement() override;

	UFUNCTION(NetMulticast, Reliable)
	void StopMovement_RPC();

	FMoveCompletedSignature ReceiveMoveCompleted;

protected:

	 virtual void BeginPlay() override;

	 virtual void OnRep_Pawn()override;

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

	void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	TSubclassOf<UNavigationQueryFilter> DefaultNavigationFilterClass;

	/** Component used for moving along a path. */
	// 这个组件在PlayerController上时网络复制的问题解决不掉 暂时不使用
 	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = AI)
 	TObjectPtr<UPathFollowingComponent> PathFollowingComponent;

	/** Is strafing allowed during movement? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	uint32 bAllowStrafe : 1;

};
