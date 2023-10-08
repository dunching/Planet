// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "ShiYu.h"
#include "Modules/ModuleManager.h"

#include "GamePlay/GameInstance/ShiYuGameInstance.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, ShiYu, "ShiYu" );

UWorld* GetWorldImp()
{
#if WITH_EDITOR
	UWorld* Result = GGameInstancePtr ? GGameInstancePtr->GetWorld() : GEditor->GetEditorWorldContext().World();
	return Result;
#else 
	return GGameInstancePtr->GetWorld();
#endif
}
