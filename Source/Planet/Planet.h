// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class UPlanetGameInstance* GGameInstancePtr = nullptr;

bool bIsExiting = false;

PLANET_API UWorld* GetWorldImp();
