// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/PlayerController.h"

#include "PlayerharacterControllerBase.generated.h"

/**
 * 
 */
UCLASS()
class SHIYU_API APlayerCharacterControllerBase : public APlayerController
{
	GENERATED_BODY()

public:

	APlayerCharacterControllerBase(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void BeginPlay() override;

	virtual void PlayerTick(float DeltaTime)override;

	virtual void UpdateRotation(float DeltaTime)override;

	virtual void OnPossess(APawn* aPawn) override;

};
