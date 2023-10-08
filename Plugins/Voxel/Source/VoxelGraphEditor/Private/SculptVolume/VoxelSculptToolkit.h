// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "Toolkits/BaseToolkit.h"

class AVoxelPreviewActor;

class FVoxelSculptToolkit : public FModeToolkit
{
public:
	FVoxelSculptToolkit() = default;

	//~ Begin FModeToolkit Interface
	virtual void Init(const TSharedPtr<IToolkitHost>& InitToolkitHost, TWeakObjectPtr<UEdMode> InOwningMode) override;

	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;

	virtual FText GetActiveToolDisplayName() const override;
	virtual FText GetActiveToolMessage() const override;

	virtual void GetToolPaletteNames(TArray<FName>& PaletteNames) const override;

	virtual bool HasIntegratedToolPalettes() const override;

	virtual void RequestModeUITabs() override;
	virtual TSharedPtr<SWidget> GetInlineContent() const override;

	virtual void OnToolStarted(UInteractiveToolManager* Manager, UInteractiveTool* Tool) override;
	virtual void OnToolEnded(UInteractiveToolManager* Manager, UInteractiveTool* Tool) override;
	//~ End FModeToolkit Interface

private:
	TWeakObjectPtr<AVoxelPreviewActor> WeakPreviewActor;
};