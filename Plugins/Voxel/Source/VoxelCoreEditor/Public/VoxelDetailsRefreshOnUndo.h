// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelCoreEditorMinimal.h"
#include "VoxelEditorDetailsUtilities.h"

class VOXELCOREEDITOR_API FVoxelDetailsRefreshOnUndo : public FEditorUndoClient
{
public:
	FSimpleMulticastDelegate Delegate;

	FVoxelDetailsRefreshOnUndo();

	template<typename T>
	explicit FVoxelDetailsRefreshOnUndo(T& Object)
		: FVoxelDetailsRefreshOnUndo()
	{
		Delegate.Add(FVoxelEditorUtilities::MakeRefreshDelegate(Object));
	}
	virtual ~FVoxelDetailsRefreshOnUndo() override;

	//~ Begin FEditorUndoClient Interface
	virtual void PostUndo(const bool bSuccess) override;
	virtual void PostRedo(const bool bSuccess) override;
	//~ End FEditorUndoClient Interface
};