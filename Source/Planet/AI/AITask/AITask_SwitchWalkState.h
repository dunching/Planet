// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "UObject/ObjectMacros.h"
#include "Engine/EngineTypes.h"
#include "AITypes.h"
#include "Navigation/PathFollowingComponent.h"
#include "Tasks/AITask.h"

#include "AITask_SwitchWalkState.generated.h"

class AAIController;

UCLASS()
class PLANET_API UAITask_SwitchWalkState : public UAITask
{
	GENERATED_BODY()

public:

	UAITask_SwitchWalkState(const FObjectInitializer& ObjectInitializer);

	void ConditionalPerformTask();

	bool WasMoveSuccessful() const;

	void SetUp(
		AAIController* Controller,
		bool bIsSwitchToRun,
		bool bIscontinueCheck
	);

protected:

	AAIController* ControllerPtr = nullptr;

	bool bIsSwitchToRun = false;

	bool bIscontinueCheck = true;

	virtual void Activate() override;

	virtual void TickTask(float DeltaTime)override;

	virtual void OnDestroy(bool bInOwnerFinished) override;

	virtual void PerformTask();

	bool SwitchWalkState();

};
