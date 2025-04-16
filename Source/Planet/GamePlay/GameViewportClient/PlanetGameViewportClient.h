// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "CoreMinimal.h"

#include "PlanetGameViewportClient.generated.h"

class UInputProcessorSubSystem;

/**
 *
 */
UCLASS()
class PLANET_API UPlanetGameViewportClient : public UGameViewportClient
{
	GENERATED_BODY()

public:
	
	 virtual void Init(struct FWorldContext& WorldContext, UGameInstance* OwningGameInstance, bool bCreateNewAudioDevice = true);

	virtual bool InputKey(
		const FInputKeyEventArgs& EventArgs
	) override;

	virtual bool InputAxis(
		FViewport* Viewport,
		FInputDeviceId InputDevice,
		FKey Key,
		float Delta,
		float DeltaTime,
		int32 NumSamples = 1,
		bool bGamepad = false
	) override;

	UPROPERTY(Transient)
	UInputProcessorSubSystem*InputProcessorSubSystemPtr = nullptr;
};
