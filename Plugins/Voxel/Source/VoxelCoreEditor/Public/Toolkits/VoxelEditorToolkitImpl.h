// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "VoxelToolkit.h"
#include "WorkflowOrientedApp/ApplicationMode.h"
#include "WorkflowOrientedApp/WorkflowCentricApplication.h"

class FVoxelEditorToolkitImpl;

class VOXELCOREEDITOR_API FVoxelToolkitApplicationMode
	: public FApplicationMode
	, public FGCObject
{
public:
	const FVoxelToolkit::FMode Mode;
	const TSharedRef<FExtender> MenuExtender = MakeVoxelShared<FExtender>();
	const TSharedRef<FUICommandList> Commands = MakeVoxelShared<FUICommandList>();

	const TSharedPtr<FVoxelToolkit>& GetToolkit() const
	{
		return Toolkit;
	}

	explicit FVoxelToolkitApplicationMode(const FVoxelToolkit::FMode& Mode);

	//~ Begin FApplicationMode Interface
	virtual void PostActivateMode() override;
	virtual void PreDeactivateMode() override;
	virtual void RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) override;
	//~ End FApplicationMode Interface

	//~ Begin FGCObject Interface
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FString GetReferencerName() const override { return "FVoxelToolkitApplicationMode"; }
	//~ End FGCObject Interface

private:
	TSharedPtr<FVoxelToolkit> Toolkit;
	TSharedPtr<const FExtensionBase> ToolbarExtension;
	TSharedPtr<const FExtensionBase> MenuExtension;
};

class VOXELCOREEDITOR_API FVoxelEditorToolkitImpl
	: public FWorkflowCentricApplication
	, public FGCObject
{
public:
	UScriptStruct* const ToolkitStruct;

	explicit FVoxelEditorToolkitImpl(UScriptStruct* ToolkitStruct)
		: ToolkitStruct(ToolkitStruct)
	{
	}

	// Will be null if modes are used
	const TSharedPtr<FVoxelToolkit>& GetToolkit() const
	{
		return Toolkit;
	}

	virtual void InitVoxelEditor(const TSharedPtr<IToolkitHost>& EditWithinLevelEditor, UObject* ObjectToEdit);

public:
	//~ Begin IToolkit Interface
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;
	virtual FText GetBaseToolkitName() const override { return ToolkitStruct->GetDisplayNameText(); }
	virtual FName GetToolkitFName() const override { return ToolkitStruct->GetFName(); }
	virtual FString GetWorldCentricTabPrefix() const override { return ToolkitStruct->GetName(); }
	virtual FLinearColor GetWorldCentricTabColorScale() const override { return FLinearColor(0.3f, 0.2f, 0.5f, 0.5f); }
	virtual void PostRegenerateMenusAndToolbars() override;
	virtual bool ProcessCommandBindings(const FKeyEvent& InKeyEvent) const override;
	virtual void OnClose() override;
	//~ End IToolkit Interface

	//~ Begin FGCObject Interface
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FString GetReferencerName() const override { return "FVoxelEditorToolkitImpl"; }
	//~ End FGCObject Interface

private:
	TWeakObjectPtr<UObject> Asset;
	TSharedPtr<FVoxelToolkit> Toolkit;
	TArray<FName> RegisteredTabIds;
	bool bClosed = false;

public:
	static UScriptStruct* GetToolkitStruct(const UClass* Class);
	static TSharedPtr<FVoxelEditorToolkitImpl> MakeToolkit(const UClass* Class);
};