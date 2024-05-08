// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Planet.h"
#include "Modules/ModuleManager.h"

#include "GamePlay/GameInstance/PlanetGameInstance.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, Planet, "Planet" );

UWorld* GetWorldImp()
{
#if WITH_EDITOR
	UWorld* Result = GGameInstancePtr ? GGameInstancePtr->GetWorld() : GEditor->GetEditorWorldContext().World();
	return Result;
#else 
	return GGameInstancePtr->GetWorld();
#endif
}
