// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Tools.h"

#include "Modules/ModuleManager.h"

UTILS_API bool GIsExiting = true;

UWorld* GetWorldImp()
{
#if WITH_EDITOR
	if (GEngine && GEditor && GEditor->IsPlayingSessionInEditor() && !GIsExiting)
	{
		auto Ptr = GWorld;
		return GEngine->GetCurrentPlayWorld() ? GEngine->GetCurrentPlayWorld() : GEditor->PlayWorld.Get();
	}
	else if (GEngine)
	{
		auto Ptr = GWorld;
		return GWorld ? GWorld : GEditor->PlayWorld;
	}
	else
	{
		return nullptr;
	}
#else 
	return GEngine->GetCurrentPlayWorld();
#endif
}
