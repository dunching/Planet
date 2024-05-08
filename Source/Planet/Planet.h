// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

PLANET_API class UPlanetGameInstance* GGameInstancePtr = nullptr;

PLANET_API bool bIsExiting = false;

UWorld* GetWorldImp();

#define GENERATIONCLASSINFO(ThisClassType, ParentClassType) \
using ThisClass = ThisClassType; \
using Super = ParentClassType;

#define GENERATIONCLASSINFOONLYTHIS(ThisClassType) \
using ThisClass = ThisClassType; 
