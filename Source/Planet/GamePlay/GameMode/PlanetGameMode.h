// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/GameModeBase.h"

#include "ThreadHelper.h"

#include "UI/UIManagerSubSystem.h"

#include "PlanetGameMode.generated.h"

extern PLANET_API class AShiYuGameMode* GGameModePtr;

/**
 *
 */
UCLASS()
class PLANET_API APlanetGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	APlanetGameMode();

protected:

	virtual void OnConstruction(const FTransform& Transform)override;

	virtual void BeginPlay()override;

	virtual void BeginDestroy()override;

	UFUNCTION()
		void OnVoxelWorldGenerated();

	UFUNCTION()
		void OnVoxelWorldLoad();

private:

	void LoadGameImp();

	void SaveGameImp();

	FTimerHandle SaveGameTimer;

	FDelegateHandle Delegate;

};
