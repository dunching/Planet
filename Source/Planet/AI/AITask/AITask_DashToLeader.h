// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "UObject/ObjectMacros.h"
#include "Engine/EngineTypes.h"
#include "AITypes.h"
#include "Navigation/PathFollowingComponent.h"
#include "Tasks/AITask.h"

#include "AITask_DashToLeader.generated.h"

class AAIController;

UCLASS()
class PLANET_API UAITask_DashToLeader : public UAITask
{
	GENERATED_BODY()

public:

	UAITask_DashToLeader(const FObjectInitializer& ObjectInitializer);

	void ConditionalPerformTask();

	bool WasMoveSuccessful() const;

	void SetUp(AAIController* Controller, const FVector& FVector);

protected:

	AAIController* ControllerPtr = nullptr;

	FVector GoalLocation = FVector::ZeroVector;

	bool bIsTransformPawn = false;

	float LerpDuration = .5f;
	
	float LerpTime = 0.f;

	virtual void Activate() override;

	virtual void TickTask(float DeltaTime)override;

	virtual void OnDestroy(bool bInOwnerFinished) override;

	virtual void PerformTask();

};
