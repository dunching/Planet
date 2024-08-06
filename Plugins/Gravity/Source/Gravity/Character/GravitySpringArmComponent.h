// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include <GameFramework/SpringArmComponent.h>

#include "GravitySpringArmComponent.generated.h"

UCLASS(config = Game)
class GRAVITY_API UGravitySpringArmComponent : public USpringArmComponent
{
	GENERATED_BODY()

public:

protected:

#if USECUSTOMEGRAVITY
	virtual void UpdateDesiredArmLocation(
		bool bDoTrace, bool bDoLocationLag, bool bDoRotationLag, float DeltaTime
	)override;

	FQuat GetGravityToWorldTransform() const;
#endif
};
