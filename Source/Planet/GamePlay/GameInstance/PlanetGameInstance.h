// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include <atomic>
#include <mutex>

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "PlanetGameInstance.generated.h"

class UAssetRefMap;

/**
 *
 */
UCLASS()
class PLANET_API UPlanetGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	virtual void Init()override;

	virtual void OnStart()override;

	virtual void Shutdown()override;

	virtual void FinishDestroy() override;

	virtual ~UPlanetGameInstance();

protected:

	std::mutex SingletonMutex;

};
