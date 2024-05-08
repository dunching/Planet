// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelEditorMinimal.h"

FVoxelDetailsRefreshOnUndo::FVoxelDetailsRefreshOnUndo()
{
	GEditor->RegisterForUndo(this);
}

FVoxelDetailsRefreshOnUndo::~FVoxelDetailsRefreshOnUndo()
{
	GEditor->UnregisterForUndo(this);
}

void FVoxelDetailsRefreshOnUndo::PostUndo(const bool bSuccess)
{
	Delegate.Broadcast();
}

void FVoxelDetailsRefreshOnUndo::PostRedo(const bool bSuccess)
{
	Delegate.Broadcast();
}