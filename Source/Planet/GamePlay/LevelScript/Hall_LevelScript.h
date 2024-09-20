// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "CoreMinimal.h"

#include "Engine/LevelScriptActor.h"

#include "UI/UIManagerSubSystem.h"

#include "Hall_LevelScript.generated.h"

/**
 *
 */
UCLASS()
class PLANET_API AHall_LevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()

public:

	virtual void BeginPlay()override;

};
