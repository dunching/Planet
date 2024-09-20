// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "AIController.h"
#include "DetourCrowdAIController.h"

#include "GravityAIController.generated.h"

UCLASS(config = Game)
class GRAVITY_API AGravityAIController : public AAIController
{
	GENERATED_BODY()

public:

	AGravityAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void UpdateControlRotation(float DeltaTime, bool bUpdatePawn)override;

private:

};

