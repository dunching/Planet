// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "CameraTrailHelper.generated.h"

class USplineComponent;
class APlayerController;

/**
 *
 */
UCLASS()
class PLANET_API ACameraTrailHelper : public AActor
{
	GENERATED_BODY()

public:

	ACameraTrailHelper(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TObjectPtr<USplineComponent> SplineComponentPtr;

protected:

	virtual void BecomeViewTarget(class APlayerController* PC)override;

	virtual void EndViewTarget(APlayerController* PC) override;

	virtual void CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult) override;

};
