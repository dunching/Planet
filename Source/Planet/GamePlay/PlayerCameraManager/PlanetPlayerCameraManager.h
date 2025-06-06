// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PlanetPlayerCameraManager.generated.h"

/**
 *
 */
UCLASS()
class PLANET_API APlanetPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:
	APlanetPlayerCameraManager(
		const FObjectInitializer& ObjectInitializer
		);

	virtual void BeginPlay() override;

	virtual void UpdateCamera(
		float DeltaTime
		) override;
};
