// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelGraphToolkit.h"
#include "VoxelNode.h"
#include "VoxelNodeStats.h"
#include "VoxelEdGraph.h"
#include "VoxelRuntimeGraph.h"
#include "VoxelGraph.h"
#include "VoxelGraphSchema.h"
#include "VoxelGraphSearchManager.h"
#include "Widgets/SVoxelGraphSearch.h"
#include "Widgets/SVoxelGraphMembers.h"
#include "Widgets/SVoxelGraphPreview.h"
#include "Widgets/SVoxelGraphMessages.h"
#include "Nodes/VoxelGraphKnotNode.h"
#include "Nodes/VoxelGraphStructNode.h"
#include "Nodes/VoxelGraphMacroParameterNode.h"
#include "Customizations/VoxelGraphNodeCustomization.h"
#include "Customizations/VoxelGraphPreviewSettingsCustomization.h"
#include "Customizations/VoxelGraphParameterSelectionCustomization.h"

#include "SGraphPanel.h"

class FVoxelGraphCommands : public TVoxelCommands<FVoxelGraphCommands>
{
public:
	TSharedPtr<FUICommandInfo> Compile;
	TSharedPtr<FUICommandInfo> CompileAll;
	TSharedPtr<FUICommandInfo> EnableStats;
	TSharedPtr<FUICommandInfo> FindInGraph;
	TSharedPtr<FUICommandInfo> FindInGraphs;
	TSharedPtr<FUICommandInfo> ReconstructAllNodes;
	TSharedPtr<FUICommandInfo> ToggleDebug;
	TSharedPtr<FUICommandInfo> TogglePreview;

	virtual void RegisterCommands() override;
};

DEFINE_VOXEL_COMMANDS(FVoxelGraphCommands);

void FVoxelGraphCommands::RegisterCommands()
{
	VOXEL_UI_COMMAND(Compile, "Compile", "Recompile the graph, done automatically whenever the graph is changed", EUserInterfaceActionType::Button, FInputChord());
	VOXEL_UI_COMMAND(CompileAll, "Compile All", "Recompile all graphs and open all the graphs with errors or warnings", EUserInterfaceActionType::Button, FInputChord());
	VOXEL_UI_COMMAND(EnableStats, "Enable Stats", "Enable stats", EUserInterfaceActionType::ToggleButton, FInputChord());
	VOXEL_UI_COMMAND(FindInGraph, "Find", "Finds references to functions, variables, and pins in the current Graph (use Ctrl+Shift+F to search in all Graphs)", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::F));
	VOXEL_UI_COMMAND(FindInGraphs, "Find in Graphs", "Find references to functions and variables in ALL Graphs", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control | EModifierKey::Shift, EKeys::F));
	VOXEL_UI_COMMAND(ReconstructAllNodes, "Reconstruct all nodes", "Reconstructs all nodes in the graph", EUserInterfaceActionType::Button, FInputChord());
	VOXEL_UI_COMMAND(ToggleDebug, "Toggle Debug", "Toggle node debug", EUserInterfaceActionType::Button, FInputChord(EKeys::D));
	VOXEL_UI_COMMAND(TogglePreview, "Toggle Preview", "Toggle node preview", EUserInterfaceActionType::Button, FInputChord(EKeys::R));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelGraphToolkit::FixupGraph(UVoxelGraph* Graph)
{
	VOXEL_FUNCTION_COUNTER();

	if (!ensure(Graph))
	{
		return;
	}

	if (!Graph->MainEdGraph)
	{
		Graph->MainEdGraph = NewObject<UVoxelEdGraph>(Graph, NAME_None, RF_Transactional);
	}

	UVoxelEdGraph* EdGraph = CastChecked<UVoxelEdGraph>(Graph->MainEdGraph);
	EdGraph->Schema = UVoxelGraphSchema::StaticClass();
	EdGraph->SetToolkit(SharedThis(this));
	EdGraph->MigrateAndReconstructAll();

	Graph->OnParametersChanged.RemoveAll(this);
	Graph->OnParametersChanged.AddSP(this, &FVoxelGraphToolkit::FixupGraphParameters);

	Graph->GetRuntimeGraph().OnMessagesChanged.RemoveAll(this);
	Graph->GetRuntimeGraph().OnMessagesChanged.Add(MakeWeakPtrDelegate(this, [=]
	{
		bMessageUpdateQueued = true;
	}));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelGraphToolkit::Initialize()
{
	Super::Initialize();

	{
		ensure(!bDisableOnGraphChanged);
		TGuardValue<bool> OnGraphChangedGuard(bDisableOnGraphChanged, true);

		for (UVoxelGraph* Graph : Asset->GetAllGraphs())
		{
			FixupGraph(Graph);
		}
	}

	GraphPreview =
		SNew(SVoxelGraphPreview)
		.IsEnabled(!bIsReadOnly)
		.Graph(Asset);

	GraphPreviewStats = GraphPreview->GetPreviewStats();

	GraphMembers =
		SNew(SVoxelGraphMembers)
		.IsEnabled(!bIsReadOnly)
		.Graph(bIsMacroLibrary ? nullptr : Asset)
		.Toolkit(SharedThis(this));

	SearchWidget =
		SNew(SVoxelGraphSearch)
		.Toolkit(SharedThis(this));

	GraphMessages =
		SNew(SVoxelGraphMessages)
		.Graph(Asset);

	{
		FDetailsViewArgs Args;
		Args.bAllowSearch = false;
		Args.bShowOptions = false;
		Args.bHideSelectionTip = true;
		Args.bShowPropertyMatrixButton = false;
		Args.NotifyHook = GetNotifyHook();
		Args.DefaultsOnlyVisibility = EEditDefaultsOnlyNodeVisibility::Hide;

		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		DetailsView = PropertyModule.CreateDetailView(Args);
		if (!bIsMacroLibrary)
		{
			DetailsView->SetObject(Asset);
		}
		DetailsView->SetEnabled(!bIsReadOnly);

		DetailsTabWidget =
			SAssignNew(ParametersBoxSplitter, SSplitter)
			.Style(FVoxelEditorStyle::Get(), "Members.Splitter")
			.PhysicalSplitterHandleSize(1.f)
			.HitDetectionSplitterHandleSize(5.f)
			.Orientation(Orient_Vertical)
			.ResizeMode(ESplitterResizeMode::Fill)
			+ SSplitter::Slot()
			.Resizable(true)
			[
				DetailsView.ToSharedRef()
			];

		ParameterGraphEditorBox = SNew(SBox);
	}

	{
		FDetailsViewArgs Args;
		Args.bAllowSearch = false;
		Args.bShowOptions = false;
		Args.bHideSelectionTip = true;
		Args.bShowPropertyMatrixButton = false;
		Args.NotifyHook = GetNotifyHook();
		Args.DefaultsOnlyVisibility = EEditDefaultsOnlyNodeVisibility::Hide;

		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PreviewDetailsView = PropertyModule.CreateDetailView(Args);
		PreviewDetailsView->SetEnabled(!bIsReadOnly);
		PreviewDetailsView->SetGenericLayoutDetailsDelegate(MakeLambdaDelegate([]() -> TSharedRef<IDetailCustomization>
		{
			return MakeVoxelShared<FVoxelGraphPreviewSettingsCustomization>();
		}));
	}

	GetCommands()->MapAction(
		FVoxelGraphCommands::Get().Compile,
		MakeLambdaDelegate([=]
		{
			for (const UVoxelGraph* Graph : Asset->GetAllGraphs())
			{
				Graph->GetRuntimeGraph().ForceRecompile();
			}
		}));

	GetCommands()->MapAction(
		FVoxelGraphCommands::Get().CompileAll,
		MakeLambdaDelegate([=]
		{
			GVoxelGraphEditorCompiler->CompileAll();
		}));

	GetCommands()->MapAction(
		FVoxelGraphCommands::Get().EnableStats,
		MakeLambdaDelegate([=]
		{
			GVoxelEnableNodeStats = !GVoxelEnableNodeStats;

			if (!GVoxelEnableNodeStats)
			{
				for (IVoxelNodeStatProvider* Provider : GVoxelNodeStatProviders)
				{
					Provider->ClearStats();
				}
			}
		}),
		MakeLambdaDelegate([]
		{
			return true;
		}),
		MakeLambdaDelegate([]
		{
			return GVoxelEnableNodeStats;
		}));

	GetCommands()->MapAction(FGraphEditorCommands::Get().FindReferences,
		FExecuteAction::CreateSP(this, &FVoxelGraphToolkit::FindReferences)
	);

	GetCommands()->MapAction(FVoxelGraphCommands::Get().FindInGraph,
		FExecuteAction::CreateSP(this, &FVoxelGraphToolkit::ToggleSearchTab)
	);

	GetCommands()->MapAction(FVoxelGraphCommands::Get().FindInGraphs,
		FExecuteAction::CreateSP(this, &FVoxelGraphToolkit::ToggleGlobalSearchWindow)
	);

	GetCommands()->MapAction(FVoxelGraphCommands::Get().ReconstructAllNodes,
		FExecuteAction::CreateSP(this, &FVoxelGraphToolkit::ReconstructAllNodes)
	);

	GetCommands()->MapAction(FVoxelGraphCommands::Get().ToggleDebug,
		FExecuteAction::CreateSP(this, &FVoxelGraphToolkit::ToggleDebug)
	);

	GetCommands()->MapAction(FVoxelGraphCommands::Get().TogglePreview,
		FExecuteAction::CreateSP(this, &FVoxelGraphToolkit::TogglePreview)
	);

	// Once everything is ready, update errors
	for (const UVoxelGraph* Graph : Asset->GetAllGraphs())
	{
		UVoxelRuntimeGraph& RuntimeGraph = Graph->GetRuntimeGraph();
		const TArray<TSharedRef<FTokenizedMessage>> RuntimeMessages = RuntimeGraph.RuntimeMessages;

		RuntimeGraph.ForceRecompile();

		// Add back runtime messages, we don't want to clear them when simply opening the asset
		RuntimeGraph.RuntimeMessages.Append(RuntimeMessages);
	}
}

void FVoxelGraphToolkit::Tick()
{
	VOXEL_FUNCTION_COUNTER();

	Super::Tick();

	FlushNodesToReconstruct();

	if (bMessageUpdateQueued)
	{
		bMessageUpdateQueued = false;

		// Focus Messages tab
		const TSharedPtr<FTabManager> TabManager = GetTabManager();
		if (ensure(TabManager))
		{
			const TSharedPtr<SDockTab> MessagesTab = TabManager->FindExistingLiveTab(FTabId(MessagesTabId));
			if (ensure(MessagesTab) &&
				// Only focus if asset is focused
				TabManager->GetOwnerTab()->HasAnyUserFocus())
			{
				MessagesTab->DrawAttention();
			}
		}

		GraphMessages->UpdateNodes();
	}

	if (GraphsToCompile.Num() > 0)
	{
		const TSet<TWeakObjectPtr<UVoxelGraph>> GraphsToCompileCopy = MoveTemp(GraphsToCompile);
		ensure(GraphsToCompile.Num() == 0);

		for (const TWeakObjectPtr<UVoxelGraph> Graph : GraphsToCompileCopy)
		{
			if (!ensure(Graph.IsValid()))
			{
				continue;
			}

			Graph->ForceRecompile();
		}

		GraphPreview->QueueUpdate();
	}
}

void FVoxelGraphToolkit::BuildMenu(FMenuBarBuilder& MenuBarBuilder)
{
	Super::BuildMenu(MenuBarBuilder);

	MenuBarBuilder.AddPullDownMenu(
		INVTEXT("Graph"),
		INVTEXT(""),
		FNewMenuDelegate::CreateLambda([this](FMenuBuilder& MenuBuilder)
		{
			MenuBuilder.BeginSection("Search", INVTEXT("Search"));
			{
				MenuBuilder.AddMenuEntry(FVoxelGraphCommands::Get().FindInGraph);
				MenuBuilder.AddMenuEntry(FVoxelGraphCommands::Get().FindInGraphs);
			}
			MenuBuilder.EndSection();

			MenuBuilder.BeginSection("Tools", INVTEXT("Tools"));
			{
				MenuBuilder.AddMenuEntry(FVoxelGraphCommands::Get().ReconstructAllNodes);
			}
			MenuBuilder.EndSection();
		}));
}

void FVoxelGraphToolkit::BuildToolbar(FToolBarBuilder& ToolbarBuilder)
{
	Super::BuildToolbar(ToolbarBuilder);

	ToolbarBuilder.BeginSection("Voxel");
	ToolbarBuilder.AddToolBarButton(FVoxelGraphCommands::Get().Compile);
	ToolbarBuilder.AddToolBarButton(FVoxelGraphCommands::Get().CompileAll);
	ToolbarBuilder.AddToolBarButton(FVoxelGraphCommands::Get().EnableStats);
	ToolbarBuilder.EndSection();
}

TSharedPtr<FTabManager::FLayout> FVoxelGraphToolkit::GetLayout() const
{
	return FTabManager::NewLayout("FVoxelGraphToolkit_Layout_v5")
		->AddArea
		(
			FTabManager::NewPrimaryArea()
			->SetOrientation(Orient_Horizontal)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.15f)
				->SetHideTabWell(true)
				->AddTab(MembersTabId, ETabState::OpenedTab)
			)
			->Split
			(
				FTabManager::NewSplitter()
				->SetOrientation(Orient_Vertical)
				->SetSizeCoefficient(0.7f)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.8f)
					->SetHideTabWell(true)
					->AddTab("Document", ETabState::ClosedTab)
				)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.2f)
					->AddTab(MessagesTabId, ETabState::OpenedTab)
					->AddTab(SearchTabId, ETabState::OpenedTab)
					->SetForegroundTab(FTabId(MessagesTabId))
				)
			)
			->Split
			(
				FTabManager::NewSplitter()
				->SetOrientation(Orient_Vertical)
				->SetSizeCoefficient(0.15f)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.4f)
					->SetHideTabWell(true)
					->AddTab(ViewportTabId, ETabState::OpenedTab)
				)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.1f)
					->SetHideTabWell(true)
					->AddTab(PreviewStatsTabId, ETabState::OpenedTab)
				)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.5f)
					->AddTab(DetailsTabId, ETabState::OpenedTab)
					->AddTab(PreviewDetailsTabId, ETabState::OpenedTab)
					->SetForegroundTab(FTabId(DetailsTabId))
				)
			)
		);
}

void FVoxelGraphToolkit::RegisterTabs(const FRegisterTab RegisterTab)
{
	RegisterTab(ViewportTabId, INVTEXT("Preview"), "LevelEditor.Tabs.Viewports", GraphPreview);
	RegisterTab(PreviewStatsTabId, INVTEXT("Stats"), "MaterialEditor.ToggleMaterialStats.Tab", GraphPreviewStats);
	RegisterTab(DetailsTabId, INVTEXT("Details"), "LevelEditor.Tabs.Details", DetailsTabWidget);
	RegisterTab(PreviewDetailsTabId, INVTEXT("Preview Settings"), "LevelEditor.Tabs.Details", PreviewDetailsView);
	RegisterTab(MembersTabId, INVTEXT("Members"), "ClassIcon.BlueprintCore", GraphMembers);
	RegisterTab(MessagesTabId, INVTEXT("Messages"), "MessageLog.TabIcon", GraphMessages);
	RegisterTab(SearchTabId, INVTEXT("Find Results"), "Kismet.Tabs.FindResults", SearchWidget);
}

void FVoxelGraphToolkit::LoadDocuments()
{
	Super::LoadDocuments();

	Asset->LastEditedDocuments.RemoveAll([](const FVoxelEditedDocumentInfo& Document)
	{
		return !Cast<UEdGraph>(Document.EditedObjectPath.ResolveObject());
	});

	if (Asset->LastEditedDocuments.Num() == 0)
	{
		if (bIsMacroLibrary)
		{
			if (Asset->InlineMacros.Num() > 0)
			{
				Asset->LastEditedDocuments.Add(FVoxelEditedDocumentInfo(Asset->InlineMacros[0]->MainEdGraph));
			}
		}
		else
		{
			Asset->LastEditedDocuments.Add(FVoxelEditedDocumentInfo(Asset->MainEdGraph));
		}
	}

	for (const FVoxelEditedDocumentInfo& Document : Asset->LastEditedDocuments)
	{
		const UEdGraph* Graph = Cast<UEdGraph>(Document.EditedObjectPath.ResolveObject());
		if (!ensure(Graph))
		{
			continue;
		}

		const TSharedPtr<SDockTab> TabWithGraph = OpenDocument(Graph, FDocumentTracker::RestorePreviousDocument);
		if (!ensure(TabWithGraph))
		{
			continue;
		}

		const TSharedRef<SGraphEditor> GraphEditor = StaticCastSharedRef<SGraphEditor>(TabWithGraph->GetContent());
		GraphEditor->SetViewLocation(Document.SavedViewOffset, Document.SavedZoomAmount);
	}
}

void FVoxelGraphToolkit::OnGraphChangedImpl(const UEdGraph* EdGraph)
{
	VOXEL_FUNCTION_COUNTER();

	UVoxelGraph* Graph = Asset->FindGraph(EdGraph);
	if (!ensure(Graph))
	{
		return;
	}

	// Fixup reroute nodes
	for (UEdGraphNode* GraphNode : EdGraph->Nodes)
	{
		if (UVoxelGraphKnotNode* Node = Cast<UVoxelGraphKnotNode>(GraphNode))
		{
			Node->PropagatePinType();
		}
	}

	GraphsToCompile.Add(Graph);
}

void FVoxelGraphToolkit::FixupGraphParameters(const UVoxelGraph::EParameterChangeType ChangeType)
{
	VOXEL_FUNCTION_COUNTER();

	FVoxelGraphDelayOnGraphChangedScope DelayScope;

	if (ChangeType == UVoxelGraph::EParameterChangeType::DefaultValue)
	{
		return;
	}

	for (UVoxelGraph* Graph : Asset->GetAllGraphs())
	{
		FixupGraph(Graph);

		for (UEdGraphNode* Node : Graph->MainEdGraph->Nodes)
		{
			if (!ensure(IsValid(Node)))
			{
				continue;
			}

			if (Node->IsA<UVoxelGraphParameterNodeBase>())
			{
				// Reconstruct to be safe
				Node->ReconstructNode();
			}
		}
	}
}

void FVoxelGraphToolkit::OnGraphEditorFocused(const TSharedRef<SGraphEditor>& GraphEditor)
{
	const UEdGraph* EdGraph = GraphEditor->GetCurrentGraph();
	if (!ensure(EdGraph))
	{
		return;
	}

	UVoxelGraph* Graph = Asset->FindGraph(EdGraph);
	// Fails to find graph, after graph removal with undo
	if (!Graph)
	{
		return;
	}

	GraphPreview->WeakGraph = Graph;
	GraphPreview->QueueUpdate();

	GraphMembers->UpdateActiveGraph(Graph);

	if (UEdGraphNode* SelectedNode = GraphEditor->GetSingleSelectedNode())
	{
		UpdateDetailsView(SelectedNode);
	}
	else
	{
		UpdateDetailsView(Graph);
	}

	GraphMembers->RequestRefresh();
}

void FVoxelGraphToolkit::OnGraphEditorClosed(TSharedRef<SDockTab> TargetGraph)
{
	Super::OnGraphEditorClosed(TargetGraph);

	GraphMembers->UpdateActiveGraph(nullptr);
	GraphMembers->RequestRefresh();
}

FString FVoxelGraphToolkit::GetGraphName(UEdGraph* EdGraph) const
{
	if (!ensure(EdGraph))
	{
		return {};
	}

	const UVoxelGraph* Graph = Asset->FindGraph(EdGraph);
	// Fails to find graph, after graph removal with undo
	if (!Graph)
	{
		return {};
	}

	if (Graph == Asset)
	{
		FString Name = Asset->GetClass()->GetDisplayNameText().ToString();
		ensure(Name.RemoveFromStart("Voxel "));
		return Name;
	}

	const UVoxelGraph* MacroGraph = Cast<UVoxelGraph>(Graph);
	if (!ensure(MacroGraph))
	{
		return {};
	}

	return MacroGraph->GetGraphName();
}

const FSlateBrush* FVoxelGraphToolkit::GetGraphIcon(UEdGraph* EdGraph) const
{
	if (!ensure(EdGraph))
	{
		return nullptr;
	}

	if (Asset->FindGraph(EdGraph) == Asset)
	{
		return FAppStyle::GetBrush(TEXT("GraphEditor.EventGraph_16x"));
	}

	return FAppStyle::GetBrush(TEXT("GraphEditor.Macro_16x"));
}

void FVoxelGraphToolkit::OnSelectedNodesChanged(const TSet<UObject*>& NewSelection)
{
	if (NewSelection.Num() == 0)
	{
		UpdateDetailsView(GetActiveEdGraph());
		SelectMember(nullptr, false, false);
		return;
	}

	if (NewSelection.Num() != 1)
	{
		UpdateDetailsView(nullptr);
		SelectMember(nullptr, false, false);
		return;
	}

	for (UObject* Node : NewSelection)
	{
		UpdateDetailsView(Node);
		SelectMember(Node, false, false);
		return;
	}
}

void FVoxelGraphToolkit::SaveTabState(const FVoxelEditedDocumentInfo& EditedDocumentInfo)
{
	Asset->LastEditedDocuments.Add(EditedDocumentInfo);
}

void FVoxelGraphToolkit::ClearSavedDocuments()
{
	Asset->LastEditedDocuments.Empty();
}

void FVoxelGraphToolkit::PostUndo()
{
	Super::PostUndo();

	CloseInvalidGraphs();

	// Clear selection, to avoid holding refs to nodes that go away

	for (const UVoxelGraph* Graph : Asset->GetAllGraphs())
	{
		if (!ensure(Graph->MainEdGraph))
		{
			continue;
		}

		if (const TSharedPtr<SGraphEditor> GraphEditor = FindGraphEditor(Graph->MainEdGraph))
		{
			GraphEditor->ClearSelectionSet();
			GraphEditor->NotifyGraphChanged();
		}

		OnGraphChanged(Graph->MainEdGraph);
	}
}

void FVoxelGraphToolkit::AddReferencedObjects(FReferenceCollector& Collector)
{
	Super::AddReferencedObjects(Collector);

	GraphPreview->AddReferencedObjects(Collector);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelGraphToolkit::FlushNodesToReconstruct()
{
	VOXEL_FUNCTION_COUNTER();

	const TSet<TWeakObjectPtr<UEdGraphNode>> NodesToReconstructCopy = MoveTemp(NodesToReconstruct);
	ensure(NodesToReconstruct.Num() == 0);

	for (const TWeakObjectPtr<UEdGraphNode>& WeakNode : NodesToReconstructCopy)
	{
		UEdGraphNode* Node = WeakNode.Get();
		if (!ensure(Node))
		{
			continue;
		}

		Node->ReconstructNode();
	}
}

FVector2D FVoxelGraphToolkit::FindLocationInGraph() const
{
	const TSharedPtr<SGraphEditor> GraphEditor = GetActiveGraphEditor();
	UEdGraph* EdGraph = GetActiveEdGraph();

	if (!GraphEditor)
	{
		if (!EdGraph)
		{
			return FVector2D::ZeroVector;
		}

		return EdGraph->GetGoodPlaceForNewNode();
	}

	const FGeometry& CachedGeometry = GraphEditor->GetCachedGeometry();
	const FVector2D CenterLocation = GraphEditor->GetGraphPanel()->PanelCoordToGraphCoord(CachedGeometry.GetLocalSize() / 2.f);

	FVector2D Location = CenterLocation;

	const FVector2D StaticSize = FVector2D(100.f, 50.f);
	FBox2D CurrentBox(Location, Location + StaticSize);

	const FBox2D ViewportBox(
		GraphEditor->GetGraphPanel()->PanelCoordToGraphCoord(FVector2D::ZeroVector),
		GraphEditor->GetGraphPanel()->PanelCoordToGraphCoord(CachedGeometry.GetLocalSize()));

	if (!EdGraph)
	{
		return Location;
	}

	int32 Iterations = 0;
	bool bFoundLocation = false;
	while (!bFoundLocation)
	{
		bFoundLocation = true;
		for (int32 Index = 0; Index < EdGraph->Nodes.Num(); Index++)
		{
			const UEdGraphNode* CurrentNode = EdGraph->Nodes[Index];
			const FVector2D NodePosition(CurrentNode->NodePosX, CurrentNode->NodePosY);
			FBox2D NodeBox(NodePosition, NodePosition);

			if (const TSharedPtr<SGraphNode> Widget = GraphEditor->GetGraphPanel()->GetNodeWidgetFromGuid(CurrentNode->NodeGuid))
			{
				if (Widget->GetCachedGeometry().GetLocalSize() == FVector2D::ZeroVector)
				{
					continue;
				}

				NodeBox.Max += Widget->GetCachedGeometry().GetAbsoluteSize() / GraphEditor->GetGraphPanel()->GetZoomAmount();
			}
			else
			{
				NodeBox.Max += StaticSize;
			}

			if (CurrentBox.Intersect(NodeBox))
			{
				Location.Y = NodeBox.Max.Y + 30.f;

				CurrentBox.Min = Location;
				CurrentBox.Max = Location + StaticSize;

				bFoundLocation = false;
				break;
			}
		}

		if (!CurrentBox.Intersect(ViewportBox))
		{
			Iterations++;
			if (Iterations == 1)
			{
				Location = CenterLocation + FVector2D(200.f, 0.f);
			}
			else if (Iterations == 2)
			{
				Location = CenterLocation - FVector2D(200.f, 0.f);
			}
			else
			{
				Location = CenterLocation;
				break;
			}

			CurrentBox.Min = Location;
			CurrentBox.Max = Location + StaticSize;

			bFoundLocation = false;
		}
	}

	return Location;
}

void FVoxelGraphToolkit::FindReferences() const
{
	const TSet<UEdGraphNode*> SelectedNodes = GetSelectedNodes();
	if (SelectedNodes.Num() != 1)
	{
		return;
	}
	const UEdGraphNode* Node = *SelectedNodes.CreateConstIterator();

	if (const UVoxelGraphParameterNodeBase* ParameterNode = Cast<UVoxelGraphParameterNodeBase>(Node))
	{
		SearchWidget->FocusForUse(ParameterNode->Guid.ToString());
	}
	else
	{
		SearchWidget->FocusForUse(Node->GetNodeTitle(ENodeTitleType::FullTitle).ToString().Replace(TEXT("\n"), TEXT(" ")));
	}
}

void FVoxelGraphToolkit::ToggleSearchTab() const
{
	SearchWidget->FocusForUse({});
}

void FVoxelGraphToolkit::ToggleGlobalSearchWindow() const
{
	if (const TSharedPtr<SVoxelGraphSearch> GlobalSearch = FVoxelGraphSearchManager::Get().OpenGlobalSearch())
	{
		GlobalSearch->FocusForUse({});
	}
}

void FVoxelGraphToolkit::ReconstructAllNodes() const
{
	for (const UVoxelGraph* Graph : Asset->GetAllGraphs())
	{
		if (!ensure(Graph->MainEdGraph))
		{
			continue;
		}

		for (UEdGraphNode* Node : Graph->MainEdGraph->Nodes)
		{
			if (!ensure(Node))
			{
				continue;
			}

			Node->ReconstructNode();
		}
	}
}

void FVoxelGraphToolkit::ToggleDebug() const
{
	TVoxelArray<UVoxelGraphNodeBase*> Nodes;
	for (UEdGraphNode* Node : GetSelectedNodes())
	{
		if (UVoxelGraphNodeBase* VoxelNode = Cast<UVoxelGraphNodeBase>(Node))
		{
			Nodes.Add(VoxelNode);
		}
	}

	if (Nodes.Num() != 1)
	{
		return;
	}

	UVoxelGraphNodeBase* Node = Nodes[0];

	if (Node->bEnableDebug)
	{
		Node->bEnableDebug = false;
		UVoxelGraphSchema::OnGraphChanged(Node);
		return;
	}

	for (const TObjectPtr<UEdGraphNode>& OtherNode : Node->GetGraph()->Nodes)
	{
		if (UVoxelGraphNodeBase* OtherVoxelNode = Cast<UVoxelGraphNodeBase>(OtherNode.Get()))
		{
			if (OtherVoxelNode->bEnableDebug)
			{
				OtherVoxelNode->bEnableDebug = false;
			}
		}
	}

	Node->bEnableDebug = true;
	UVoxelGraphSchema::OnGraphChanged(Node);
}

void FVoxelGraphToolkit::TogglePreview() const
{
	TVoxelArray<UVoxelGraphNodeBase*> Nodes;
	for (UEdGraphNode* Node : GetSelectedNodes())
	{
		if (UVoxelGraphNodeBase* VoxelNode = Cast<UVoxelGraphNodeBase>(Node))
		{
			Nodes.Add(VoxelNode);
		}
	}

	if (Nodes.Num() != 1)
	{
		return;
	}

	UVoxelGraphNodeBase* Node = Nodes[0];

	if (Node->bEnablePreview)
	{
		const FVoxelTransaction Transaction(Node, "Stop previewing");
		Node->bEnablePreview = false;
		return;
	}

	const FVoxelTransaction Transaction(Node, "Start previewing");

	for (const TObjectPtr<UEdGraphNode>& OtherNode : Node->GetGraph()->Nodes)
	{
		if (UVoxelGraphNodeBase* OtherVoxelNode = Cast<UVoxelGraphNodeBase>(OtherNode.Get()))
		{
			if (OtherVoxelNode->bEnablePreview)
			{
				const FVoxelTransaction OtherTransaction(OtherVoxelNode, "Stop previewing");
				OtherVoxelNode->bEnablePreview = false;
			}
		}
	}

	Node->bEnablePreview = true;
}

void FVoxelGraphToolkit::UpdateDetailsView(UObject* Object)
{
	if (const UVoxelGraphParameterNodeBase* ParameterNode = Cast<UVoxelGraphParameterNodeBase>(Object))
	{
		DetailsView->SetGenericLayoutDetailsDelegate(MakeLambdaDelegate([this, EdGraph = MakeWeakObjectPtr(ParameterNode->GetGraph()), Guid = ParameterNode->Guid, Object = MakeWeakObjectPtr(Object)]() -> TSharedRef<IDetailCustomization>
		{
			return MakeVoxelShared<FVoxelGraphParameterSelectionCustomization>(EdGraph, Guid, Object);
		}));
	}
	else if (UVoxelGraphStructNode* StructNode = Cast<UVoxelGraphStructNode>(Object))
	{
		DetailsView->SetGenericLayoutDetailsDelegate(MakeLambdaDelegate([this]() -> TSharedRef<IDetailCustomization>
		{
			return MakeVoxelShared<FVoxelGraphNodeCustomization>();
		}));
	}
	else
	{
		DetailsView->SetGenericLayoutDetailsDelegate(nullptr);
	}

	UVoxelGraph* ParameterGraph = nullptr;
	if (const UEdGraph* EdGraph = Cast<UEdGraph>(Object))
	{
		if (UVoxelGraph* Graph = Asset->FindGraph(EdGraph))
		{
			DetailsView->SetObject(Graph);
		}
		else
		{
			if (!Graph)
			{
				if (TSharedPtr<SGraphEditor> GraphEditor = FindGraphEditor(EdGraph))
				{
					CloseInvalidGraphs();
				}
			}
			DetailsView->SetObject(nullptr);
		}
	}
	else if (const UVoxelGraphParameterNodeBase* ParameterNode = Cast<UVoxelGraphParameterNodeBase>(Object))
	{
		UVoxelGraph* Graph = Asset->FindGraph(ParameterNode->GetGraph());
		if (ensure(Graph))
		{
			ParameterGraph = Graph->ParameterGraphs.FindRef(ParameterNode->Guid);
			DetailsView->SetObject(Graph);
		}
		else
		{
			DetailsView->SetObject(nullptr);
		}
	}
	else
	{
		DetailsView->SetObject(Object);
	}

	PreviewDetailsView->SetObject(Cast<UVoxelGraphNode>(Object));

	DetailsView->ForceRefresh();
	PreviewDetailsView->ForceRefresh();

	UpdateParameterGraph(ParameterGraph);
}

void FVoxelGraphToolkit::SelectMember(UObject* Object, const bool bRequestRename, const bool bRefreshMembers) const
{
	using ESection = SVoxelGraphMembers::ESection;

	if (UEdGraph* EdGraph = Cast<UEdGraph>(Object))
	{
		const UVoxelGraph* Graph = Asset->FindGraph(EdGraph);

		ESection Section = ESection::None;
		if (bIsMacroLibrary)
		{
			Section = ESection::MacroLibraries;
		}
		else if (Graph != Asset)
		{
			Section = ESection::InlineMacros;
		}

		GraphMembers->SelectMember(FName(GetGraphName(EdGraph)), SVoxelGraphMembers::GetSectionId(Section), bRequestRename, bRefreshMembers);
		return;
	}

	if (const UVoxelGraph* Graph = Cast<UVoxelGraph>(Object))
	{
		ESection Section = ESection::None;
		if (bIsMacroLibrary)
		{
			Section = ESection::MacroLibraries;
		}
		else if (Graph != Asset)
		{
			Section = ESection::InlineMacros;
		}

		GraphMembers->SelectMember(
			FName(GetGraphName(Graph->MainEdGraph)),
			SVoxelGraphMembers::GetSectionId(Section),
			bRequestRename,
			bRefreshMembers);
		return;
	}

	if (const UVoxelGraphParameterNodeBase* ParameterNode = Cast<UVoxelGraphParameterNodeBase>(Object))
	{
		GraphMembers->SelectMember(
			ParameterNode->CachedParameter.Name,
			SVoxelGraphMembers::GetSectionId(ParameterNode->CachedParameter.ParameterType),
			bRequestRename,
			bRefreshMembers);
		return;
	}

	GraphMembers->SelectMember(
		"",
		SVoxelGraphMembers::GetSectionId(ESection::None),
		false,
		bRefreshMembers);
}

void FVoxelGraphToolkit::SelectParameter(UVoxelGraph* Graph, const FGuid Guid, const bool bRequestRename, const bool bRefreshMembers)
{
	const FVoxelGraphParameter* Parameter = Graph->FindParameterByGuid(Guid);
	if (!ensure(Parameter))
	{
		UpdateDetailsView(GetActiveEdGraph());
		SelectMember(nullptr, false, true);
		return;
	}

	GraphMembers->SelectMember(
		Parameter->Name,
		SVoxelGraphMembers::GetSectionId(Parameter->ParameterType),
		bRequestRename,
		bRefreshMembers);

	// Find the selected node if any
	// This is needed as SelectParameter will be called when changing bExposeDefaultPin,
	// essentially unselecting the node from the detail panel
	TWeakObjectPtr<UEdGraphNode> SelectedNode;
	for (UEdGraphNode* Node : GetSelectedNodes())
	{
		if (SelectedNode.IsValid())
		{
			// More than one selected
			SelectedNode = {};
			break;
		}

		SelectedNode = Node;
	}

	DetailsView->SetGenericLayoutDetailsDelegate(MakeLambdaDelegate([=, EdGraph = MakeWeakObjectPtr(Graph->MainEdGraph)]() -> TSharedRef<IDetailCustomization>
	{
		return MakeVoxelShared<FVoxelGraphParameterSelectionCustomization>(EdGraph, Guid, SelectedNode);
	}));
	DetailsView->SetObject(Graph);
	DetailsView->ForceRefresh();

	UpdateParameterGraph(Graph->ParameterGraphs.FindRef(Guid));
}

void FVoxelGraphToolkit::UpdateParameterGraph(const UVoxelGraph* Graph)
{
	if (!Graph ||
		!ensure(Graph->MainEdGraph) ||
		!ensure(Graph->bIsParameterGraph))
	{
		if (ParametersBoxSplitter->GetChildren()->Num() > 1)
		{
			ParametersBoxSplitter->RemoveAt(1);
		}

		ParameterGraphEditorBox->SetContent(SNullWidget::NullWidget);
		return;
	}

	if (ParametersBoxSplitter->GetChildren()->Num() == 1)
	{
		ParametersBoxSplitter->AddSlot()
		.Resizable(true)
		[
			ParameterGraphEditorBox.ToSharedRef()
		];
	}

	ParameterGraphEditorBox->SetContent(CreateGraphEditor(Graph->MainEdGraph, false));
}

void FVoxelGraphToolkit::RefreshPreviewSettings() const
{
	if (PreviewDetailsView)
	{
		PreviewDetailsView->ForceRefresh();
	}
}