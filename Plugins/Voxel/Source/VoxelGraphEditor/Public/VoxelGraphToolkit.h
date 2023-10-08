// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "VoxelGraphToolkitBase.h"
#include "VoxelGraphToolkit.generated.h"

class SVoxelGraphSearch;
class SVoxelGraphMembers;
class SVoxelGraphPreview;
class SVoxelGraphMessages;

USTRUCT()
struct VOXELGRAPHEDITOR_API FVoxelGraphToolkit : public FVoxelGraphToolkitBase
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

	UPROPERTY()
	TObjectPtr<UVoxelGraph> Asset;

	bool bIsMacroLibrary = false;

public:
	void FixupGraph(UVoxelGraph* Graph);

public:
	TSharedRef<SVoxelGraphMembers> GetGraphMembers() const
	{
		return GraphMembers.ToSharedRef();
	}
	TSharedRef<SVoxelGraphSearch> GetSearchWidget() const
	{
		return SearchWidget.ToSharedRef();
	}
	void AddNodeToReconstruct(UEdGraphNode* Node)
	{
		NodesToReconstruct.Add(Node);
	}

	//~ Begin FVoxelToolkit Interface
	virtual void Initialize() override;
	virtual void Tick() override;
	virtual void BuildMenu(FMenuBarBuilder& MenuBarBuilder) override;
	virtual void BuildToolbar(FToolBarBuilder& ToolbarBuilder) override;
	virtual TSharedPtr<FTabManager::FLayout> GetLayout() const override;
	virtual void RegisterTabs(FRegisterTab RegisterTab) override;
	virtual void LoadDocuments() override;

	virtual void OnGraphChangedImpl(const UEdGraph* EdGraph) override;
	virtual void FixupGraphParameters(UVoxelGraph::EParameterChangeType ChangeType) override;
	virtual void OnGraphEditorFocused(const TSharedRef<SGraphEditor>& GraphEditor) override;
	virtual void OnGraphEditorClosed(TSharedRef<SDockTab> TargetGraph) override;
	virtual FString GetGraphName(UEdGraph* EdGraph) const override;
	virtual const FSlateBrush* GetGraphIcon(UEdGraph* EdGraph) const override;
	virtual void OnSelectedNodesChanged(const TSet<UObject*>& NewSelection) override;

	virtual void SaveTabState(const FVoxelEditedDocumentInfo& EditedDocumentInfo) override;
	virtual void ClearSavedDocuments() override;

	virtual void PostUndo() override;
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	//~ End FVoxelToolkit Interface

	void FlushNodesToReconstruct();

	FVector2D FindLocationInGraph() const;

	void FindReferences() const;
	void ToggleSearchTab() const;
	void ToggleGlobalSearchWindow() const;
	void ReconstructAllNodes() const;
	void ToggleDebug() const;
	void TogglePreview() const;
	void UpdateDetailsView(UObject* Object);
	void SelectMember(UObject* Object, bool bRequestRename, bool bRefreshMembers) const;
	void SelectParameter(UVoxelGraph* Graph, FGuid Guid, bool bRequestRename, bool bRefreshMembers);
	void UpdateParameterGraph(const UVoxelGraph* Graph);
	void RefreshPreviewSettings() const;

public:
	static constexpr const TCHAR* ViewportTabId = TEXT("FVoxelGraphEditorToolkit_Viewport");
	static constexpr const TCHAR* DetailsTabId = TEXT("FVoxelGraphEditorToolkit_Details");
	static constexpr const TCHAR* PreviewDetailsTabId = TEXT("FVoxelGraphEditorToolkit_PreviewDetails");
	static constexpr const TCHAR* MessagesTabId = TEXT("FVoxelGraphEditorToolkit_Messages");
	static constexpr const TCHAR* GraphTabId = TEXT("FVoxelGraphEditorToolkit_Graph");
	static constexpr const TCHAR* MembersTabId = TEXT("FVoxelGraphEditorToolkit_Members");
	static constexpr const TCHAR* PreviewStatsTabId = TEXT("FVoxelGraphEditorToolkit_PreviewStats");
	static constexpr const TCHAR* SearchTabId = TEXT("FVoxelGraphEditorToolkit_Search");

private:
	TSharedPtr<SWidget> DetailsTabWidget;
	TSharedPtr<IDetailsView> DetailsView;
	TSharedPtr<SSplitter> ParametersBoxSplitter;
	TSharedPtr<SBox> ParameterGraphEditorBox;
	TSharedPtr<IDetailsView> PreviewDetailsView;
	TSharedPtr<SVoxelGraphPreview> GraphPreview;
	TSharedPtr<SVoxelGraphMessages> GraphMessages;
	TSharedPtr<SWidget> GraphPreviewStats;
	TSharedPtr<SVoxelGraphMembers> GraphMembers;
	TSharedPtr<SVoxelGraphSearch> SearchWidget;

	bool bMessageUpdateQueued = false;
	TSet<TWeakObjectPtr<UEdGraphNode>> NodesToReconstruct;
	TSet<TWeakObjectPtr<UVoxelGraph>> GraphsToCompile;
};