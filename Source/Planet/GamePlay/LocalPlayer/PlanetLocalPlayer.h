// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "CoreMinimal.h"

#include "PlanetLocalPlayer.generated.h"

/**
 *
 */
UCLASS()
class PLANET_API UPlanetLocalPlayer : public ULocalPlayer
{
	GENERATED_BODY()

public:

	 virtual void ReceivedPlayerController(APlayerController* NewController) override;
	
	virtual bool SpawnPlayActor(const FString& URL,FString& OutError, UWorld* InWorld)override;
	
	virtual void PlayerAdded(class UGameViewportClient* InViewportClient, FPlatformUserId InUserId) override;
	
#pragma region RPC

#pragma endregion

};
