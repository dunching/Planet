// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelGraphToolkitBase.h"
#include "VoxelEdGraph.h"
#include "VoxelGraphNodeBase.h"
#include "VoxelGraphSchemaAction.h"
#include "SVoxelGraphEditorActionMenu.h"

#include "SNodePanel.h"

int32 GVoxelGraphDelayOnGraphChangedScopeCount = 0;

struct FVoxelPendingOnGraphChanged
{
	TWeakPtr<FVoxelGraphToolkitBase> WeakToolkit;
	TWeakObjectPtr<const UEdGraph> Graph;

	bool operator==(const FVoxelPendingOnGraphChanged& Other) const
	{
		return
			WeakToolkit == Other.WeakToolkit &&
			Graph == Other.Graph;
	}
	friend uint32 GetTypeHash(const FVoxelPendingOnGraphChanged& OnGraphChanged)
	{
		return FVoxelUtilities::MurmurHashMulti(
			GetTypeHash(OnGraphChanged.WeakToolkit),
			GetTypeHash(OnGraphChanged.Graph));
	}
};
TSet<FVoxelPendingOnGraphChanged> GVoxelPendingOnGraphChangedSet;

FVoxelGraphDelayOnGraphChangedScope::FVoxelGraphDelayOnGraphChangedScope()
{
	ensure(GVoxelGraphDelayOnGraphChangedScopeCount >= 0);
	GVoxelGraphDelayOnGraphChangedScopeCount++;
}

FVoxelGraphDelayOnGraphChangedScope::~FVoxelGraphDelayOnGraphChangedScope()
{
	GVoxelGraphDelayOnGraphChangedScopeCount--;
	ensure(GVoxelGraphDelayOnGraphChangedScopeCount >= 0);

	if (GVoxelGraphDelayOnGraphChangedScopeCount != 0)
	{
		return;
	}

	const TSet<FVoxelPendingOnGraphChanged> PendingOnGraphChangedSet = MoveTemp(GVoxelPendingOnGraphChangedSet);
	for (const FVoxelPendingOnGraphChanged& PendingOnGraphChanged : PendingOnGraphChangedSet)
	{
		const TSharedPtr<FVoxelGraphToolkitBase> Toolkit = PendingOnGraphChanged.WeakToolkit.Pin();
		const UEdGraph* Graph = PendingOnGraphChanged.Graph.Get();
		if (!ensure(Toolkit) ||
			!ensure(Graph))
		{
			continue;
		}

		Toolkit->OnGraphChanged(Graph);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelGraphToolkitBase::OnGraphEditorClosed(TSharedRef<SDockTab> TargetGraph)
{
	WeakFocusedGraph = nullptr;
}

void FVoxelGraphToolkitBase::OnGraphChanged(const UEdGraph* EdGraph)
{
	if (bDisableOnGraphChanged ||
		!ensure(!bOnGraphChangedCalled) ||
		// Happens when assets are teared down
		IsGarbageCollecting())
	{
		return;
	}

	TGuardValue<bool> Guard(bOnGraphChangedCalled, true);

	if (GVoxelGraphDelayOnGraphChangedScopeCount > 0)
	{
		GVoxelPendingOnGraphChangedSet.Add({ SharedThis(this), EdGraph });
		return;
	}

	OnGraphChangedImpl(EdGraph);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedRef<SGraphEditor> FVoxelGraphToolkitBase::CreateGraphEditor(UEdGraph* Graph, const bool bBindOnSelect)
{
	FGraphAppearanceInfo AppearanceInfo;
	AppearanceInfo.CornerText = INVTEXT("VOXEL");

	SGraphEditor::FGraphEditorEvents Events;
	if (bBindOnSelect)
	{
		Events.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &FVoxelGraphToolkitBase::OnSelectedNodesChanged);
	}
	Events.OnTextCommitted = FOnNodeTextCommitted::CreateSP(this, &FVoxelGraphToolkitBase::OnNodeTitleCommitted);
	Events.OnNodeDoubleClicked = FSingleNodeEvent::CreateSP(this, &FVoxelGraphToolkitBase::OnNodeDoubleClicked);
	Events.OnSpawnNodeByShortcut = MakeWeakPtrDelegate(this, [this](FInputChord Chord, const FVector2D& Position)
	{
		OnSpawnGraphNodeByShortcut(Chord, Position);
		return FReply::Handled();
	});

	Events.OnCreateActionMenu = SGraphEditor::FOnCreateActionMenu::CreateSP(this, &FVoxelGraphToolkitBase::OnCreateGraphActionMenu);

	TSharedRef<SGraphEditor> Editor = SNew(SGraphEditor)
		.AdditionalCommands(GetCommands())
		.IsEditable(!bIsReadOnly)
		.Appearance(AppearanceInfo)
		.GraphToEdit(Graph)
		.GraphEvents(Events)
		.AutoExpandActionMenu(false)
		.ShowGraphStateOverlay(false);

	FVector2D ViewOffset = FVector2D::ZeroVector;
	float ZoomAmount = -1.f;

	const TSharedPtr<SDockTab> ActiveTab = DocumentManager->GetActiveTab();
	if (ActiveTab.IsValid())
	{
		if (const TSharedPtr<SGraphEditor> GraphEditor = StaticCastSharedRef<SGraphEditor>(ActiveTab->GetContent()))
		{
			if (GraphEditor->GetCurrentGraph() == Graph)
			{
				GraphEditor->GetViewLocation(ViewOffset, ZoomAmount);
			}
		}
	}

	Editor->SetViewLocation(ViewOffset, ZoomAmount);

	return Editor;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelGraphToolkitBase::Initialize()
{
	Super::Initialize();

	DocumentManager = MakeVoxelShared<FDocumentTracker>();
	// HostingApp is not used
	DocumentManager->Initialize(nullptr);
	DocumentManager->RegisterDocumentFactory(MakeVoxelShared<FVoxelGraphEditorSummoner>(SharedThis(this)));

	FGraphEditorCommands::Register();

	GetCommands()->MapAction(
		FGenericCommands::Get().Undo,
		MakeLambdaDelegate([]
		{
			GEditor->UndoTransaction();
		}));

	GetCommands()->MapAction(
		FGenericCommands::Get().Redo,
		MakeLambdaDelegate([]
		{
			GEditor->RedoTransaction();
		}));

	GetCommands()->MapAction(FGraphEditorCommands::Get().CreateComment,
		FExecuteAction::CreateSP(this, &FVoxelGraphToolkitBase::CreateComment)
	);

	GetCommands()->MapAction(FGraphEditorCommands::Get().SplitStructPin,
		FExecuteAction::CreateSP(this, &FVoxelGraphToolkitBase::OnSplitPin)
	);

	GetCommands()->MapAction(FGraphEditorCommands::Get().RecombineStructPin,
		FExecuteAction::CreateSP(this, &FVoxelGraphToolkitBase::OnRecombinePin)
	);

	GetCommands()->MapAction(FGraphEditorCommands::Get().ResetPinToDefaultValue,
		FExecuteAction::CreateSP(this, &FVoxelGraphToolkitBase::OnResetPinToDefaultValue),
		FCanExecuteAction::CreateSP(this, &FVoxelGraphToolkitBase::CanResetPinToDefaultValue)
	);

	GetCommands()->MapAction(FGenericCommands::Get().SelectAll,
		MakeWeakPtrDelegate(this, [this]
		{
			if (const TSharedPtr<SGraphEditor> ActiveGraphEditor = GetActiveGraphEditor())
			{
				ActiveGraphEditor->SelectAllNodes();
			}
		}));

	GetCommands()->MapAction(FGenericCommands::Get().Delete,
		FExecuteAction::CreateSP(this, &FVoxelGraphToolkitBase::DeleteSelectedNodes),
		FCanExecuteAction::CreateSP(this, &FVoxelGraphToolkitBase::CanDeleteNodes)
	);

	GetCommands()->MapAction(FGenericCommands::Get().Copy,
		FExecuteAction::CreateSP(this, &FVoxelGraphToolkitBase::CopySelectedNodes),
		FCanExecuteAction::CreateSP(this, &FVoxelGraphToolkitBase::CanCopyNodes)
	);

	GetCommands()->MapAction(FGenericCommands::Get().Cut,
		FExecuteAction::CreateSP(this, &FVoxelGraphToolkitBase::CutSelectedNodes),
		FCanExecuteAction::CreateSP(this, &FVoxelGraphToolkitBase::CanCutNodes)
	);

	GetCommands()->MapAction(FGenericCommands::Get().Paste,
		FExecuteAction::CreateSP(this, &FVoxelGraphToolkitBase::PasteNodes),
		FCanExecuteAction::CreateSP(this, &FVoxelGraphToolkitBase::CanPasteNodes)
	);

	GetCommands()->MapAction(FGenericCommands::Get().Duplicate,
		FExecuteAction::CreateSP(this, &FVoxelGraphToolkitBase::DuplicateNodes),
		FCanExecuteAction::CreateSP(this, &FVoxelGraphToolkitBase::CanDuplicateNodes)
	);

	GetCommands()->MapAction(FGraphEditorCommands::Get().AlignNodesTop,
		MakeWeakPtrDelegate(this, [this]
		{
			if (const TSharedPtr<SGraphEditor> ActiveGraphEditor = GetActiveGraphEditor())
			{
				ActiveGraphEditor->OnAlignTop();
			}
		}));

	GetCommands()->MapAction(FGraphEditorCommands::Get().AlignNodesMiddle,
		MakeWeakPtrDelegate(this, [this]
		{
			if (const TSharedPtr<SGraphEditor> ActiveGraphEditor = GetActiveGraphEditor())
			{
				ActiveGraphEditor->OnAlignMiddle();
			}
		}));

	GetCommands()->MapAction(FGraphEditorCommands::Get().AlignNodesBottom,
		MakeWeakPtrDelegate(this, [this]
		{
			if (const TSharedPtr<SGraphEditor> ActiveGraphEditor = GetActiveGraphEditor())
			{
				ActiveGraphEditor->OnAlignBottom();
			}
		}));

	GetCommands()->MapAction(FGraphEditorCommands::Get().AlignNodesLeft,
		MakeWeakPtrDelegate(this, [this]
		{
			if (const TSharedPtr<SGraphEditor> ActiveGraphEditor = GetActiveGraphEditor())
			{
				ActiveGraphEditor->OnAlignLeft();
			}
		}));

	GetCommands()->MapAction(FGraphEditorCommands::Get().AlignNodesCenter,
		MakeWeakPtrDelegate(this, [this]
		{
			if (const TSharedPtr<SGraphEditor> ActiveGraphEditor = GetActiveGraphEditor())
			{
				ActiveGraphEditor->OnAlignCenter();
			}
		}));

	GetCommands()->MapAction(FGraphEditorCommands::Get().AlignNodesRight,
		MakeWeakPtrDelegate(this, [this]
		{
			if (const TSharedPtr<SGraphEditor> ActiveGraphEditor = GetActiveGraphEditor())
			{
				ActiveGraphEditor->OnAlignRight();
			}
		}));

	GetCommands()->MapAction(FGraphEditorCommands::Get().StraightenConnections,
		MakeWeakPtrDelegate(this, [this]
		{
			if (const TSharedPtr<SGraphEditor> ActiveGraphEditor = GetActiveGraphEditor())
			{
				ActiveGraphEditor->OnStraightenConnections();
			}
		}));

	GetCommands()->MapAction(FGraphEditorCommands::Get().DistributeNodesHorizontally,
		MakeWeakPtrDelegate(this, [this]
		{
			if (const TSharedPtr<SGraphEditor> ActiveGraphEditor = GetActiveGraphEditor())
			{
				ActiveGraphEditor->OnDistributeNodesH();
			}
		}));

	GetCommands()->MapAction(FGraphEditorCommands::Get().DistributeNodesVertically,
		MakeWeakPtrDelegate(this, [this]
		{
			if (const TSharedPtr<SGraphEditor> ActiveGraphEditor = GetActiveGraphEditor())
			{
				ActiveGraphEditor->OnDistributeNodesV();
			}
		}));
}

void FVoxelGraphToolkitBase::SaveDocuments()
{
	Super::SaveDocuments();

	ClearSavedDocuments();
	DocumentManager->SaveAllState();
}

void FVoxelGraphToolkitBase::SetTabManager(const TSharedRef<FTabManager>& TabManager)
{
	Super::SetTabManager(TabManager);

	DocumentManager->SetTabManager(TabManager);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedPtr<SGraphEditor> FVoxelGraphToolkitBase::FindGraphEditor(const UEdGraph* EdGraph) const
{
	for (const TSharedPtr<SDockTab>& Tab : DocumentManager->GetAllDocumentTabs())
	{
		if (!ensure(Tab))
		{
			continue;
		}

		TSharedRef<SGraphEditor> GraphEditor = StaticCastSharedRef<SGraphEditor>(Tab->GetContent());
		if (GraphEditor->GetCurrentGraph() == EdGraph)
		{
			return GraphEditor;
		}
	}

	return nullptr;
}

TSharedPtr<SGraphEditor> FVoxelGraphToolkitBase::OpenGraphAndBringToFront(const UEdGraph* EdGraph, const bool bSetFocus) const
{
	if (!ensure(EdGraph))
	{
		return nullptr;
	}

	const TSharedPtr<SDockTab> TabWithGraph = OpenDocument(EdGraph, FDocumentTracker::OpenNewDocument);
	if (!ensure(TabWithGraph))
	{
		return nullptr;
	}

	const TSharedRef<SGraphEditor> GraphEditor = StaticCastSharedRef<SGraphEditor>(TabWithGraph->GetContent());

	if (bSetFocus)
	{
		GraphEditor->CaptureKeyboard();
	}

	return GraphEditor;
}

TSharedPtr<SDockTab> FVoxelGraphToolkitBase::OpenDocument(const UObject* DocumentID, const FDocumentTracker::EOpenDocumentCause Cause) const
{
	return DocumentManager->OpenDocument(FTabPayload_UObject::Make(DocumentID), Cause);
}

void FVoxelGraphToolkitBase::CloseGraph(UEdGraph* EdGraph)
{
	TGuardValue<TWeakObjectPtr<UEdGraph>> Guard(GraphToBeClosed, EdGraph);
	DocumentManager->CloseTab(FTabPayload_UObject::Make(EdGraph));
}

void FVoxelGraphToolkitBase::CloseInvalidGraphs() const
{
	DocumentManager->RefreshAllTabs();
	DocumentManager->CleanInvalidTabs();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelGraphToolkitBase::OnNodeTitleCommitted(const FText& NewText, ETextCommit::Type CommitInfo, UEdGraphNode* NodeBeingChanged) const
{
	if (!ensure(NodeBeingChanged))
	{
		return;
	}

	const FVoxelTransaction Transaction(NodeBeingChanged, "Rename Node");
	NodeBeingChanged->OnRenameNode(NewText.ToString());
}

void FVoxelGraphToolkitBase::OnNodeDoubleClicked(UEdGraphNode* Node)
{
	if (Node->CanJumpToDefinition())
	{
		Node->JumpToDefinition();
	}
}

FActionMenuContent FVoxelGraphToolkitBase::OnCreateGraphActionMenu(UEdGraph* InGraph, const FVector2D& InNodePosition, const TArray<UEdGraphPin*>& InDraggedPins, bool bAutoExpand, SGraphEditor::FActionMenuClosed InOnMenuClosed) const
{
	const TSharedRef<SVoxelGraphEditorActionMenu> Menu =
		SNew(SVoxelGraphEditorActionMenu)
		.GraphObj(InGraph)
		.NewNodePosition(InNodePosition)
		.DraggedFromPins(InDraggedPins)
		.AutoExpandActionMenu(bAutoExpand)
		.OnClosedCallback(InOnMenuClosed);

	return FActionMenuContent(Menu, Menu->GetFilterTextBox());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSet<UEdGraphNode*> FVoxelGraphToolkitBase::GetSelectedNodes() const
{
	const TSharedPtr<SGraphEditor> GraphEditor = GetActiveGraphEditor();
	if (!ensure(GraphEditor))
	{
		return {};
	}

	TSet<UEdGraphNode*> SelectedNodes;
	for (UObject* Object : GraphEditor->GetSelectedNodes())
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(Object);
		if (!ensure(Node))
		{
			continue;
		}

		SelectedNodes.Add(Node);
	}
	return SelectedNodes;
}

void FVoxelGraphToolkitBase::CreateComment() const
{
	const TSharedPtr<SGraphEditor> GraphEditor = GetActiveGraphEditor();
	if (!ensure(GraphEditor))
	{
		return;
	}

	UEdGraph* Graph = GraphEditor->GetCurrentGraph();
	if (!ensure(Graph))
	{
		return;
	}

	FVoxelGraphSchemaAction_NewComment CommentAction;
	CommentAction.PerformAction(Graph, nullptr, GraphEditor->GetPasteLocation());
}

void FVoxelGraphToolkitBase::DeleteNodes(const TArray<UEdGraphNode*>& NodesRef)
{
	if (NodesRef.Num() == 0)
	{
		return;
	}

	// Ensure we're not going to edit the array while iterating it, eg if it's directly the graph node array
	const TArray<UEdGraphNode*> Nodes = NodesRef;

	const TSharedPtr<SGraphEditor> GraphEditor = GetActiveGraphEditor();
	if (!ensure(GraphEditor))
	{
		return;
	}

	UEdGraph* Graph = GraphEditor->GetCurrentGraph();
	if (!ensure(Graph))
	{
		return;
	}

	const FVoxelTransaction Transaction(Graph, "Delete nodes");

	for (UEdGraphNode* Node : Nodes)
	{
		if (!ensure(Node) || !Node->CanUserDeleteNode())
		{
			continue;
		}

		GraphEditor->SetNodeSelection(Node, false);

		Node->Modify();
		Graph->GetSchema()->BreakNodeLinks(*Node);
		Node->DestroyNode();
	}
}

void FVoxelGraphToolkitBase::OnSplitPin()
{
	const TSharedPtr<SGraphEditor> GraphEditor = GetActiveGraphEditor();
	if (!ensure(GraphEditor))
	{
		return;
	}

	UEdGraphPin* Pin = GraphEditor->GetGraphPinForMenu();
	if (!ensure(Pin))
	{
		return;
	}

	UVoxelGraphNodeBase* Node = Cast<UVoxelGraphNodeBase>(Pin->GetOwningNode());
	if (!ensure(Node) ||
		!ensure(Node->CanSplitPin(*Pin)))
	{
		return;
	}

	const FVoxelTransaction Transaction(Node, "Split pin");
	Node->SplitPin(*Pin);
}

void FVoxelGraphToolkitBase::OnRecombinePin()
{
	const TSharedPtr<SGraphEditor> GraphEditor = GetActiveGraphEditor();
	if (!ensure(GraphEditor))
	{
		return;
	}

	UEdGraphPin* Pin = GraphEditor->GetGraphPinForMenu();
	if (!ensure(Pin))
	{
		return;
	}

	UVoxelGraphNodeBase* Node = Cast<UVoxelGraphNodeBase>(Pin->GetOwningNode());
	if (!ensure(Node) ||
		!ensure(Node->CanRecombinePin(*Pin)))
	{
		return;
	}

	const FVoxelTransaction Transaction(Node, "Recombine pin");
	Node->RecombinePin(*Pin);
}

void FVoxelGraphToolkitBase::OnResetPinToDefaultValue()
{
	const TSharedPtr<SGraphEditor> GraphEditor = GetActiveGraphEditor();
	if (!ensure(GraphEditor))
	{
		return;
	}

	const UEdGraph* Graph = GraphEditor->GetCurrentGraph();
	if (!ensure(Graph))
	{
		return;
	}

	UEdGraphPin* Pin = GraphEditor->GetGraphPinForMenu();
	if (!ensure(Pin))
	{
		return;
	}

	Graph->GetSchema()->ResetPinToAutogeneratedDefaultValue(Pin);
}

bool FVoxelGraphToolkitBase::CanResetPinToDefaultValue() const
{
	const TSharedPtr<SGraphEditor> GraphEditor = GetActiveGraphEditor();
	if (!ensure(GraphEditor))
	{
		return false;
	}

	const UEdGraphPin* Pin = GraphEditor->GetGraphPinForMenu();
	return Pin && !Pin->DoesDefaultValueMatchAutogenerated();
}

void FVoxelGraphToolkitBase::DeleteSelectedNodes()
{
	DeleteNodes(GetSelectedNodes().Array());
}

bool FVoxelGraphToolkitBase::CanDeleteNodes() const
{
	const TSharedPtr<SGraphEditor> GraphEditor = GetActiveGraphEditor();
	if (!GraphEditor)
	{
		return false;
	}

	if (GraphEditor->GetNumberOfSelectedNodes() == 0)
	{
		return false;
	}

	for (const UEdGraphNode* Node : GetSelectedNodes())
	{
		if (!Node->CanUserDeleteNode())
		{
			return false;
		}
	}

	return true;
}

void FVoxelGraphToolkitBase::CutSelectedNodes()
{
	CopySelectedNodes();

	TArray<UEdGraphNode*> NodesToDelete;
	for (UEdGraphNode* Node : GetSelectedNodes())
	{
		if (Node->CanDuplicateNode())
		{
			NodesToDelete.Add(Node);
		}
	}

	DeleteNodes(NodesToDelete);
}

bool FVoxelGraphToolkitBase::CanCutNodes() const
{
	return CanCopyNodes() && CanDeleteNodes();
}

void FVoxelGraphToolkitBase::CopySelectedNodes()
{
	TSet<UEdGraphNode*> NodesToCopy;
	{
		for (UEdGraphNode* Node : GetSelectedNodes())
		{
			if (!Node->CanDuplicateNode())
			{
				continue;
			}

			Node->PrepareForCopying();
			NodesToCopy.Add(Node);
		}
	}

	FString ExportedText;
	FEdGraphUtilities::ExportNodesToText(reinterpret_cast<TSet<UObject*>&>(NodesToCopy), ExportedText);
	FPlatformApplicationMisc::ClipboardCopy(*ExportedText);
}

bool FVoxelGraphToolkitBase::CanCopyNodes() const
{
	for (const UEdGraphNode* Node : GetSelectedNodes())
	{
		if (Node->CanDuplicateNode())
		{
			return true;
		}
	}
	return false;
}

void FVoxelGraphToolkitBase::PasteNodes()
{
	const TSharedPtr<SGraphEditor> GraphEditor = GetActiveGraphEditor();
	if (!ensure(GraphEditor))
	{
		return;
	}

	PasteNodesHere(GraphEditor->GetPasteLocation());
}

void FVoxelGraphToolkitBase::PasteNodesHere(const FVector2D& Location)
{
	const TSharedPtr<SGraphEditor> GraphEditor = GetActiveGraphEditor();
	if (!ensure(GraphEditor))
	{
		return;
	}

	UEdGraph* Graph = GraphEditor->GetCurrentGraph();
	if (!ensure(Graph))
	{
		return;
	}

	const FVoxelTransaction Transaction(Graph, "Paste nodes");
	FVoxelGraphDelayOnGraphChangedScope OnGraphChangedScope;

	// Clear the selection set (newly pasted stuff will be selected)
	GraphEditor->ClearSelectionSet();

	// Grab the text to paste from the clipboard.
	FString TextToImport;
	FPlatformApplicationMisc::ClipboardPaste(TextToImport);

	// Import the nodes
	TSet<UEdGraphNode*> PastedNodes;
	FEdGraphUtilities::ImportNodesFromText(Graph, TextToImport, PastedNodes);

	TSet<UEdGraphNode*> CopyPastedNodes = PastedNodes;
	for (UEdGraphNode* Node : CopyPastedNodes)
	{
		if (UVoxelGraphNodeBase* VoxelNode = Cast<UVoxelGraphNodeBase>(Node))
		{
			if (!VoxelNode->CanPasteVoxelNode(PastedNodes))
			{
				Node->DestroyNode();
				PastedNodes.Remove(Node);
			}
		}
	}

	// Average position of nodes so we can move them while still maintaining relative distances to each other
	FVector2D AvgNodePosition(0.0f, 0.0f);

	for (const UEdGraphNode* Node : PastedNodes)
	{
		AvgNodePosition.X += Node->NodePosX;
		AvgNodePosition.Y += Node->NodePosY;
	}

	if (PastedNodes.Num() > 0)
	{
		AvgNodePosition.X /= PastedNodes.Num();
		AvgNodePosition.Y /= PastedNodes.Num();
	}

	for (UEdGraphNode* Node : PastedNodes)
	{
		// Select the newly pasted stuff
		GraphEditor->SetNodeSelection(Node, true);

		Node->NodePosX = (Node->NodePosX - AvgNodePosition.X) + Location.X;
		Node->NodePosY = (Node->NodePosY - AvgNodePosition.Y) + Location.Y;

		Node->SnapToGrid(SNodePanel::GetSnapGridSize());

		// Give new node a different Guid from the old one
		Node->CreateNewGuid();
	}

	// Post paste for local variables
	for (UEdGraphNode* Node : PastedNodes)
	{
		if (UVoxelGraphNodeBase* VoxelNode = Cast<UVoxelGraphNodeBase>(Node))
		{
			VoxelNode->bEnableDebug = false;
			VoxelNode->bEnablePreview = false;
			VoxelNode->PostPasteVoxelNode(PastedNodes);
		}
	}

	// Update UI
	GraphEditor->NotifyGraphChanged();
}

bool FVoxelGraphToolkitBase::CanPasteNodes() const
{
	const TSharedPtr<SGraphEditor> GraphEditor = GetActiveGraphEditor();
	if (!ensure(GraphEditor))
	{
		return false;
	}

	const UEdGraph* Graph = GraphEditor->GetCurrentGraph();
	if (!ensure(Graph))
	{
		return false;
	}

	FString ClipboardContent;
	FPlatformApplicationMisc::ClipboardPaste(ClipboardContent);

	return FEdGraphUtilities::CanImportNodesFromText(Graph, ClipboardContent);
}

void FVoxelGraphToolkitBase::DuplicateNodes()
{
	// Copy and paste current selection
	CopySelectedNodes();
	PasteNodes();
}

bool FVoxelGraphToolkitBase::CanDuplicateNodes() const
{
	return CanCopyNodes();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelGraphEditorSummoner::FVoxelGraphEditorSummoner(const TSharedRef<FVoxelGraphToolkitBase>& Toolkit)
	: FDocumentTabFactoryForObjects<UEdGraph>(STATIC_FNAME("FVoxelGraphEditorSummoner"), nullptr)
	, WeakToolkit(Toolkit)
{
}

void FVoxelGraphEditorSummoner::OnTabActivated(TSharedPtr<SDockTab> Tab) const
{
	const TSharedPtr<FVoxelGraphToolkitBase> Toolkit = WeakToolkit.Pin();
	if (!ensure(Toolkit))
	{
		return;
	}

	Tab->SetOnTabClosed(SDockTab::FOnTabClosedCallback::CreateSP(Toolkit.Get(), &FVoxelGraphToolkitBase::OnGraphEditorClosed));
	if (Toolkit->GraphToBeClosed == GetGraphEditor(Tab)->GetCurrentGraph() ||
		Toolkit->WeakFocusedGraph == GetGraphEditor(Tab))
	{
		return;
	}

	Toolkit->WeakFocusedGraph = GetGraphEditor(Tab);
	Toolkit->OnGraphEditorFocused(GetGraphEditor(Tab));
}

void FVoxelGraphEditorSummoner::SaveState(TSharedPtr<SDockTab> Tab, TSharedPtr<FTabPayload> Payload) const
{
	if (!ensure(Payload->IsValid()))
	{
		return;
	}

	UEdGraph* Graph = FTabPayload_UObject::CastChecked<UEdGraph>(Payload);
	const TSharedPtr<FVoxelGraphToolkitBase> Toolkit = WeakToolkit.Pin();

	if (!ensure(Graph) ||
		!ensure(Toolkit))
	{
		return;
	}

	FVector2D ViewLocation = FVector2D::ZeroVector;
	float ZoomAmount = 0.f;
	GetGraphEditor(Tab)->GetViewLocation(ViewLocation, ZoomAmount);

	Toolkit->SaveTabState(FVoxelEditedDocumentInfo(Graph, ViewLocation, ZoomAmount));
}

TAttribute<FText> FVoxelGraphEditorSummoner::ConstructTabNameForObject(UEdGraph* DocumentID) const
{
	const TSharedPtr<FVoxelGraphToolkitBase> Toolkit = WeakToolkit.Pin();
	if (!ensure(Toolkit))
	{
		return {};
	}

	return TAttribute<FText>::Create(MakeWeakPtrDelegate(Toolkit, [&Toolkit = *Toolkit, DocumentID]()
	{
		return FText::FromString(Toolkit.GetGraphName(DocumentID));
	}));
}

TSharedRef<SWidget> FVoxelGraphEditorSummoner::CreateTabBodyForObject(const FWorkflowTabSpawnInfo& Info, UEdGraph* DocumentID) const
{
	const TSharedPtr<FVoxelGraphToolkitBase> Toolkit = WeakToolkit.Pin();
	check(Toolkit);
	return Toolkit->CreateGraphEditor(DocumentID, true);
}

const FSlateBrush* FVoxelGraphEditorSummoner::GetTabIconForObject(const FWorkflowTabSpawnInfo& Info, UEdGraph* DocumentID) const
{
	const TSharedPtr<FVoxelGraphToolkitBase> Toolkit = WeakToolkit.Pin();
	if (!ensure(Toolkit))
	{
		return FAppStyle::GetNoBrush();
	}

	return Toolkit->GetGraphIcon(DocumentID);
}

TSharedRef<FGenericTabHistory> FVoxelGraphEditorSummoner::CreateTabHistoryNode(TSharedPtr<FTabPayload> Payload)
{
	return MakeVoxelShared<FVoxelGraphTabHistory>(SharedThis(this), Payload);
}

TSharedRef<SGraphEditor> FVoxelGraphEditorSummoner::GetGraphEditor(const TSharedPtr<SDockTab>& Tab) const
{
	return StaticCastSharedRef<SGraphEditor>(Tab->GetContent());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelGraphTabHistory::EvokeHistory(const TSharedPtr<FTabInfo> InTabInfo, const bool bPrevTabMatches)
{
	const TSharedPtr<SDockTab> DockTab = InTabInfo->GetTab().Pin();
	if (!ensure(DockTab))
	{
		return;
	}

	if (bPrevTabMatches)
	{
		WeakGraphEditor = StaticCastSharedRef<SGraphEditor>(DockTab->GetContent());
		return;
	}

	const TSharedPtr<FDocumentTabFactory> Factory = FactoryPtr.Pin();
	if (!ensure(Factory))
	{
		return;
	}

	FWorkflowTabSpawnInfo SpawnInfo;
	SpawnInfo.Payload = Payload;
	SpawnInfo.TabInfo = InTabInfo;

	const TSharedRef<SGraphEditor> GraphEditor = StaticCastSharedRef<SGraphEditor>(Factory->CreateTabBody(SpawnInfo));
	WeakGraphEditor = GraphEditor;
	Factory->UpdateTab(DockTab, SpawnInfo, GraphEditor);
}

void FVoxelGraphTabHistory::SaveHistory()
{
	if (!IsHistoryValid())
	{
		return;
	}

	const TSharedPtr<SGraphEditor> GraphEditor = WeakGraphEditor.Pin();
	if (!ensure(GraphEditor))
	{
		return;
	}

	GraphEditor->GetViewLocation(SavedLocation, SavedZoomAmount);
	GraphEditor->GetViewBookmark(SavedBookmarkId);
}

void FVoxelGraphTabHistory::RestoreHistory()
{
	if (!IsHistoryValid())
	{
		return;
	}

	const TSharedPtr<SGraphEditor> GraphEditor = WeakGraphEditor.Pin();
	if (!ensure(GraphEditor))
	{
		return;
	}

	GraphEditor->SetViewLocation(SavedLocation, SavedZoomAmount, SavedBookmarkId);
}