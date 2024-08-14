// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Planet_Tools.h"

bool bIsExiting = true;

UWorld* GetWorldImp()
{
#if WITH_EDITOR
	if (GEngine && GEditor && GEditor->IsPlayingSessionInEditor() && !bIsExiting)
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
