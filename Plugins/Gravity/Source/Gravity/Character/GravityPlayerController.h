// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include <GameFramework/PlayerController.h>

#include "GravityPlayerController.generated.h"

UCLASS(config = Game)
class GRAVITY_API AGravityPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	virtual void UpdateRotation(float DeltaTime)override;

protected:

	// ¼ÇÂ¼PlayerµÄYawºÍPitch
	FRotator ControlRotationWithoutGravityTrans = FRotator::ZeroRotator;

};
