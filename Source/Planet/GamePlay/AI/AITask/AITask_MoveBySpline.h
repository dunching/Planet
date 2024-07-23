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

UCLASS()
class PLANET_API UAITask_MoveBySpline : public UAITask_MoveTo
{
	GENERATED_BODY()

public:
	
	virtual void PerformMove()override;

};
