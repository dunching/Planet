// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "Widgets/SVoxelGraphSearch.h"

class SVoxelGraphSearch;

class FVoxelGraphSearchManager
{
public:
	static FName GlobalSearchTabId;

	FVoxelGraphSearchManager();

	static FVoxelGraphSearchManager& Get();

private:
	void Initialize();
	TSharedRef<SDockTab> SpawnGlobalSearchWidgetTab(const FSpawnTabArgs& SpawnTabArgs);

public:
	TSharedPtr<SVoxelGraphSearch> OpenGlobalSearch() const;
	TMap<UObject*, UVoxelGraph*> GetAllLookupGraphs() const;

private:
	TWeakPtr<SVoxelGraphSearch> GlobalSearchWidget;
	TMap<const UClass*, const FProperty*> AssetClasses;
};