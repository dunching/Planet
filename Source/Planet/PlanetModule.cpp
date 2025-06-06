// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "PlanetModule.h"

#include "Modules/ModuleManager.h"

#include "GamePlay/GameInstance/PlanetGameInstance.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, Planet, "Planet" );

#if WITH_EDITOR
TArray<TWeakPtr<FBasicProxy>> TestGCProxyMap;
#endif