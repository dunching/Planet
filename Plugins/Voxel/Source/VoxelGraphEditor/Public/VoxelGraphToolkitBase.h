// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "VoxelGraph.h"
#include "VoxelToolkit.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"
#include "WorkflowOrientedApp/WorkflowUObjectDocuments.h"
#include "VoxelGraphToolkitBase.generated.h"

struct FVoxelGraphDelayOnGraphChangedScope
{
	FVoxelGraphDelayOnGraphChangedScope();
	~FVoxelGraphDelayOnGraphChangedScope();
};

USTRUCT()
struct VOXELGRAPHEDITOR_API FVoxelGraphToolkitBase : public FVoxelToolkit
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

	bool bIsReadOnly = false;

public:
	virtual void OnSelectedNodesChanged(const TSet<UObject*>& NewSelection) VOXEL_PURE_VIRTUAL();
	virtual void OnSpawnGraphNodeByShortcut(const FInputChord& Chord, const FVector2D& Position) {}
	virtual void FixupGraphParameters(UVoxelGraph::EParameterChangeType ChangeType) VOXEL_PURE_VIRTUAL();

	virtual void OnGraphChangedImpl(const UEdGraph* EdGraph) VOXEL_PURE_VIRTUAL();

	virtual void OnGraphEditorFocused(const TSharedRef<SGraphEditor>& TargetGraph) VOXEL_PURE_VIRTUAL();
	virtual void OnGraphEditorClosed(TSharedRef<SDockTab> TargetGraph);
	virtual FString GetGraphName(UEdGraph* EdGraph) const VOXEL_PURE_VIRTUAL({});
	virtual const FSlateBrush* GetGraphIcon(UEdGraph* EdGraph) const VOXEL_PURE_VIRTUAL({});

	virtual void SaveTabState(const FVoxelEditedDocumentInfo& EditedDocumentInfo) VOXEL_PURE_VIRTUAL();
	virtual void ClearSavedDocuments() VOXEL_PURE_VIRTUAL();

	void OnGraphChanged(const UEdGraph* EdGraph);

protected:
	TSharedRef<SGraphEditor> CreateGraphEditor(UEdGraph* Graph, bool bBindOnSelect);

public:
	//~ Begin FVoxelToolkit Interface
	virtual void Initialize() override;
	virtual void SaveDocuments() override;
	virtual void SetTabManager(const TSharedRef<FTabManager>& TabManager) override;
	//~ End FVoxelToolkit Interface

public:
	TSharedPtr<SGraphEditor> GetActiveGraphEditor() const
	{
		return WeakFocusedGraph.Pin();
	}
	UEdGraph* GetActiveEdGraph() const
	{
		const TSharedPtr<SGraphEditor> GraphEditor = GetActiveGraphEditor();
		if (!GraphEditor)
		{
			return nullptr;
		}
		return GraphEditor->GetCurrentGraph();
	}

	TSharedPtr<SGraphEditor> FindGraphEditor(const UEdGraph* EdGraph) const;

	TSharedPtr<SGraphEditor> OpenGraphAndBringToFront(const UEdGraph* EdGraph, bool bSetFocus) const;
	TSharedPtr<SDockTab> OpenDocument(const UObject* DocumentID, FDocumentTracker::EOpenDocumentCause Cause) const;
	void CloseGraph(UEdGraph* EdGraph);
	void CloseInvalidGraphs() const;

private:
	void OnNodeTitleCommitted(const FText& NewText, ETextCommit::Type CommitInfo, UEdGraphNode* NodeBeingChanged) const;
	void OnNodeDoubleClicked(UEdGraphNode* Node);
	FActionMenuContent OnCreateGraphActionMenu(UEdGraph* InGraph, const FVector2D& InNodePosition, const TArray<UEdGraphPin*>& InDraggedPins, bool bAutoExpand, SGraphEditor::FActionMenuClosed InOnMenuClosed) const;

public:
	TSet<UEdGraphNode*> GetSelectedNodes() const;

	void CreateComment() const;
	void DeleteNodes(const TArray<UEdGraphNode*>& Nodes);

	void OnSplitPin();
	void OnRecombinePin();

	void OnResetPinToDefaultValue();
	bool CanResetPinToDefaultValue() const;

	void DeleteSelectedNodes();
	bool CanDeleteNodes() const;

	void CutSelectedNodes();
	bool CanCutNodes() const;

	void CopySelectedNodes();
	bool CanCopyNodes() const;

	void PasteNodes();
	void PasteNodesHere(const FVector2D& Location);
	bool CanPasteNodes() const;

	void DuplicateNodes();
	bool CanDuplicateNodes() const;

public:
	bool bDisableOnGraphChanged = false;

private:
	bool bOnGraphChangedCalled = false;

	TSharedPtr<FDocumentTracker> DocumentManager;
	TWeakPtr<SGraphEditor> WeakFocusedGraph;
	TWeakObjectPtr<UEdGraph> GraphToBeClosed;

	friend struct FVoxelGraphEditorSummoner;
};

struct FVoxelGraphEditorSummoner : public FDocumentTabFactoryForObjects<UEdGraph>
{
public:
	const TWeakPtr<FVoxelGraphToolkitBase> WeakToolkit;

	explicit FVoxelGraphEditorSummoner(const TSharedRef<FVoxelGraphToolkitBase>& Toolkit);

	//~ Begin FDocumentTabFactoryForObjects Interface
	virtual void OnTabActivated(TSharedPtr<SDockTab> Tab) const override;
	virtual void SaveState(TSharedPtr<SDockTab> Tab, TSharedPtr<FTabPayload> Payload) const override;

	virtual TAttribute<FText> ConstructTabNameForObject(UEdGraph* DocumentID) const override;
	virtual TSharedRef<SWidget> CreateTabBodyForObject(const FWorkflowTabSpawnInfo& Info, UEdGraph* DocumentID) const override;
	virtual const FSlateBrush* GetTabIconForObject(const FWorkflowTabSpawnInfo& Info, UEdGraph* DocumentID) const override;
	virtual TSharedRef<FGenericTabHistory> CreateTabHistoryNode(TSharedPtr<FTabPayload> Payload) override;
	//~ End FDocumentTabFactoryForObjects Interface

	TSharedRef<SGraphEditor> GetGraphEditor(const TSharedPtr<SDockTab>& Tab) const;

};

struct FVoxelGraphTabHistory : public FGenericTabHistory
{
public:
	using FGenericTabHistory::FGenericTabHistory;

	//~ Begin FGenericTabHistory Interface
	virtual void EvokeHistory(TSharedPtr<FTabInfo> InTabInfo, bool bPrevTabMatches) override;
	virtual void SaveHistory() override;
	virtual void RestoreHistory() override;
	//~ End FGenericTabHistory Interface

private:
	TWeakPtr<SGraphEditor> WeakGraphEditor;
	FVector2D SavedLocation = FVector2D::ZeroVector;
	float SavedZoomAmount = -1.f;
	FGuid SavedBookmarkId;
};