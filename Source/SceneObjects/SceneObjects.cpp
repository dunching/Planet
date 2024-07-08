// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "SceneObjects.h"

#include "Modules/ModuleManager.h"

class FSceneObjectsGameModule : public FDefaultGameModuleImpl
{
	virtual void StartupModule() override
	{
	}

	virtual void ShutdownModule() override
	{
	}
};

IMPLEMENT_PRIMARY_GAME_MODULE(FSceneObjectsGameModule, SceneObjects, "SceneObjects" );
