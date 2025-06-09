// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Engine/SceneCapture2D.h"

#include "MinimapSceneCapture2D.generated.h"

/**
 *
 */
UCLASS()
class PLANET_API AMinimapSceneCapture2D : public ASceneCapture2D
{
	GENERATED_BODY()

public:

	
	AMinimapSceneCapture2D(
		const FObjectInitializer& ObjectInitializer
		);

	virtual void BeginPlay() override;

	virtual void Tick(
		float DeltaTime
		) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
	int32 Depth = 1000;

	TDelegate<void(float)>OnPlayerAngleChangedDelegate;
};
