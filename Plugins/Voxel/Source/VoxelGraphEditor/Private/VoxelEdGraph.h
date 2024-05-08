// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "VoxelEdGraph.generated.h"

class SGraphEditor;
struct FVoxelGraphToolkit;
struct FVoxelGraphDelayOnGraphChangedScope;

UCLASS()
class UVoxelEdGraph : public UEdGraph
{
	GENERATED_BODY()

public:
	void SetToolkit(const TSharedRef<FVoxelGraphToolkit>& Toolkit);
	TSharedPtr<FVoxelGraphToolkit> GetGraphToolkit() const;

	void MigrateIfNeeded();
	void MigrateAndReconstructAll();

	//~ Begin UObject interface
	virtual void PreEditChange(FProperty* PropertyAboutToChange) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	//~ End UObject interface

private:
	TWeakPtr<FVoxelGraphToolkit> WeakToolkit;
	TArray<TSharedPtr<FVoxelGraphDelayOnGraphChangedScope>> DelayOnGraphChangedScopeStack;

private:
	using FVersion = DECLARE_VOXEL_VERSION
	(
		FirstVersion,
		SplitInputSetterAndRemoveLocalVariablesDefault
	);

	UPROPERTY()
	int32 Version = FVersion::FirstVersion;
};