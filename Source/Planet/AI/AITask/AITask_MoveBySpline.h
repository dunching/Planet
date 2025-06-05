// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "UObject/ObjectMacros.h"
#include "Engine/EngineTypes.h"
#include "AITypes.h"
#include "Navigation/PathFollowingComponent.h"
#include "Tasks/AITask.h"
#include <Tasks/AITask_MoveTo.h>

#include "AITask_MoveBySpline.generated.h"

class AAIController;
class USplineComponent;

// namespace EPathFollowingResult { enum Type : int; }
// 
// using FMyMoveTaskCompletedSignature = TDelegate<void(TEnumAsByte<EPathFollowingResult::Type>)>;

UCLASS()
class PLANET_API UAITask_MoveBySpline : public UAITask_MoveTo
{
	GENERATED_BODY()

public:
	
	virtual void Activate() override;

	virtual void PerformMove()override;

	virtual void OnRequestFinished(FAIRequestID RequestID, const FPathFollowingResult& Result)override;
	
	/**
	 * 是否已经到了SPline上？
	 */
	bool bIsReachedSPline = false;

	UPROPERTY(Transient)
	USplineComponent* SPlinePtr = nullptr;
	
};
