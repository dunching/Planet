// Fill out your copyright notice in the Description page of Project Settings.

#include "SVoxelGraphSelector.h"

#include "VoxelGraph.h"
#include "VoxelGraphExecutor.h"

void SVoxelGraphSelector::Construct(const FArguments& InArgs)
{
	OnGraphSelected = InArgs._OnGraphSelected;
	IsGraphValid = InArgs._IsGraphValid;

	IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(FName("AssetRegistry")).Get();
	AssetRegistry.OnAssetAdded().Add(MakeTSWeakPtrDelegate(this, [this](const FAssetData&)
	{
		RebuildList();
	}));
	AssetRegistry.OnAssetRemoved().Add(MakeTSWeakPtrDelegate(this, [this](const FAssetData&)
	{
		RebuildList();
	}));
	AssetRegistry.OnAssetRenamed().Add(MakeTSWeakPtrDelegate(this, [this](const FAssetData&, const FString&)
	{
		RebuildList();
	}));
	AssetRegistry.OnAssetUpdated().Add(MakeTSWeakPtrDelegate(this, [this](const FAssetData&)
	{
		RebuildList();
	}));

	GVoxelGraphExecutorManager->OnGraphChanged.Add(MakeWeakPtrDelegate(this, [this](const UVoxelGraphInterface&)
	{
		RebuildList();
	}));

	FindAllGraphs();

	ChildSlot
	[
		SNew(SBorder)
		.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.Padding(0.f, 2.f)
			.AutoHeight()
			[
				SNew(SSearchBox)
				.HintText(INVTEXT("Search..."))
				.OnTextChanged(this, &SVoxelGraphSelector::OnSearchChanged)
			]
			+ SVerticalBox::Slot()
			.FillHeight(1.f)
			[
				SAssignNew(GraphsListView, SListView<TSharedPtr<FVoxelGraphListNode>>)
				.SelectionMode(ESelectionMode::Single)
				.ListItemsSource(&FilteredGraphsList)
				.OnGenerateRow(this, &SVoxelGraphSelector::OnGenerateRow)
				.OnSelectionChanged(this, &SVoxelGraphSelector::OnSelectionChanged)
				.ItemHeight(24.0f)
			]
		]
	];
}

void SVoxelGraphSelector::OnSearchChanged(const FText& Text)
{
	LookupString = Text.ToString().ToLower();
	LookupText = Text;

	FilterGraphs();

	if (ActiveGraph.IsValid())
	{
		bool bContainsActiveGraph = false;
		for (const TSharedPtr<FVoxelGraphListNode>& Node : FilteredGraphsList)
		{
			if (Node->WeakGraph == ActiveGraph)
			{
				bContainsActiveGraph = true;
				break;
			}
		}

		if (!bContainsActiveGraph)
		{
			OnGraphSelected.ExecuteIfBound(nullptr);
		}
	}

	GraphsListView->RebuildList();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedRef<ITableRow> SVoxelGraphSelector::OnGenerateRow(TSharedPtr<FVoxelGraphListNode> Node, const TSharedRef<STableViewBase>& OwningTable)
{
	FAssetThumbnailConfig ThumbnailConfig;
	ThumbnailConfig.bAllowFadeIn = false;

	return
		SNew(STableRow<TSharedPtr<FVoxelGraphListNode>>, OwningTable)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(7.5f, 1.5f, 5.f, 1.5f)
			[
				SNew(SBox)
				.WidthOverride(36.f)
				.HeightOverride(36.f)
				[
					Node->AssetThumbnail->MakeThumbnailWidget()
				]
			]
			+ SHorizontalBox::Slot()
			.Padding(2.5f, 0.f)
			.VAlign(VAlign_Center)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(STextBlock)
					.Text(Node->GetName())
					.TextStyle(FVoxelEditorStyle::Get(), "Graph.NewAssetDialog.AssetPickerBoldAssetNameText")
					.HighlightText_Lambda(MakeWeakPtrLambda(this, [this]
					{
						return LookupText;
					}))
				]
				+ SVerticalBox::Slot()
				[
					SNew(STextBlock)
					.Text(Node->GetDescription())
					.TextStyle(FVoxelEditorStyle::Get(), "Graph.NewAssetDialog.AssetPickerAssetNameText")
					.AutoWrapText(true)
				]
			]
		];
}

void SVoxelGraphSelector::OnSelectionChanged(TSharedPtr<FVoxelGraphListNode> Node, ESelectInfo::Type SelectType)
{
	ActiveGraph = Node ? Cast<UVoxelGraph>(Node->Asset.GetAsset()) : nullptr;

	if (SelectType != ESelectInfo::Direct)
	{
		OnGraphSelected.ExecuteIfBound(ActiveGraph.Get());
	}
}

void SVoxelGraphSelector::FindAllGraphs()
{
	VOXEL_FUNCTION_COUNTER();

	GraphsList = {};

	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

	TArray<FAssetData> Assets;
	AssetRegistryModule.Get().GetAssetsByClass(UVoxelGraph::StaticClass()->GetClassPathName(), Assets);

	for (const FAssetData& Asset : Assets)
	{
		UVoxelGraph* Graph = Cast<UVoxelGraph>(Asset.GetAsset());
		if (!Graph)
		{
			continue;
		}

		if (!IsGraphValid.Execute(Graph))
		{
			continue;
		}

		GraphsList.Add(MakeShared<FVoxelGraphListNode>(Asset, Graph));
	}

	FilterGraphs();
}

void SVoxelGraphSelector::FilterGraphs()
{
	if (LookupString.IsEmpty())
	{
		FilteredGraphsList = GraphsList;
		return;
	}

	FilteredGraphsList = {};
	FilteredGraphsList.Reserve(GraphsList.Num());

	for (const TSharedPtr<FVoxelGraphListNode>& Graph : GraphsList)
	{
		if (Graph->LookupName.Contains(LookupString))
		{
			FilteredGraphsList.Add(Graph);
		}
	}
}

void SVoxelGraphSelector::RebuildList()
{
	VOXEL_FUNCTION_COUNTER();

	FindAllGraphs();
	GraphsListView->RebuildList();

	for (const TSharedPtr<FVoxelGraphListNode>& Node : FilteredGraphsList)
	{
		if (Node->WeakGraph == ActiveGraph)
		{
			GraphsListView->SetSelection(Node, ESelectInfo::Direct);
		}
	}
}