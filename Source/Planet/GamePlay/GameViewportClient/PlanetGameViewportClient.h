// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Kismet/GameplayStatics.h"
#include "Slate/SGameLayerManager.h"
#include "Slate/SWorldWidgetScreenLayer.h"


#include "PlanetGameViewportClient.generated.h"

class UInputProcessorSubSystem_Imp;
class UWidgetComponent;
class UFocusIcon;
class UMainHUDLayout;

/**
 *
 */
UCLASS(BlueprintType,Blueprintable)
class PLANET_API UPlanetGameViewportClient : public UGameViewportClient
{
	GENERATED_BODY()

public:
	virtual void Init(
		struct FWorldContext& WorldContext,
		UGameInstance* OwningGameInstance,
		bool bCreateNewAudioDevice = true
	) override;
	
	virtual void NotifyPlayerAdded( int32 PlayerIndex, class ULocalPlayer* AddedPlayer ) override;
	
	virtual ULocalPlayer* SetupInitialLocalPlayer(FString& OutError) override;
	
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

private:
	
	UPROPERTY(Transient)
	UInputProcessorSubSystem_Imp* InputProcessorSubSystemPtr = nullptr;
};
