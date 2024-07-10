// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Planet_Tools.h"

UWorld* GetWorldImp()
{
#if WITH_EDITOR
	return GEngine->GetCurrentPlayWorld() ? GEngine->GetCurrentPlayWorld() : GEditor->GetEditorWorldContext().World();
#else 
	return GEngine->GetCurrentPlayWorld();
#endif
}
