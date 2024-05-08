// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "SVoxelGraphSearch.h"

#include "VoxelGraph.h"
#include "VoxelGraphSearchResult.h"
#include "VoxelGraphToolkit.h"
#include "VoxelGraphSearchManager.h"

void SVoxelGraphSearch::Construct(const FArguments& InArgs)
{
	WeakToolkit = InArgs._Toolkit;
	bIsGlobalSearch = InArgs._IsGlobalSearch;

	ChildSlot
	[
		SNew(SBorder)
		.BorderImage(FAppStyle::GetBrush("Brushes.Panel"))
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(8.f, 5.f)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.FillWidth(1.f)
				[
					SAssignNew(SearchTextField, SSearchBox)
					.HintText(INVTEXT("Enter function or event name to find references..."))
					.OnTextCommitted(this, &SVoxelGraphSearch::OnSearchTextCommitted)
					.DelayChangeNotificationsWhileTyping(false)
				]
				+ SHorizontalBox::Slot()
				.Padding(4.f, 0.f, 2.f, 0.f)
				.AutoWidth()
				[
					SNew(SButton)
					.Visibility(bIsGlobalSearch ? EVisibility::Collapsed : EVisibility::Visible)
					.VAlign(VAlign_Center)
					.OnClicked(this, &SVoxelGraphSearch::OnOpenGlobalFindResults)
					.ToolTipText(INVTEXT("Find in all Voxel Graphs"))
					[
						SNew(STextBlock)
						.ColorAndOpacity(FSlateColor::UseForeground())
						.TextStyle(FAppStyle::Get(), "FindResults.FindInBlueprints")
						.Text(FText::FromString(TEXT("\xf1e5")))
					]
				]
			]
			+ SVerticalBox::Slot()
			.FillHeight(1.f)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.FillWidth(1.f)
				[
					SNew(SBorder)
					.BorderImage(FAppStyle::GetBrush("Brushes.Recessed"))
					.Padding(8.f, 8.f, 4.f, 0.f)
					[
						SAssignNew(TreeView, STreeViewType)
						.ItemHeight(24.f)
						.TreeItemsSource(&ItemsFound)
						.OnGenerateRow(this, &SVoxelGraphSearch::OnGenerateRow)
						.OnGetChildren(this, &SVoxelGraphSearch::OnGetChildren)
						.OnMouseButtonDoubleClick(this, &SVoxelGraphSearch::OnTreeSelectionDoubleClicked)
						.SelectionMode(ESelectionMode::Multi)
					]
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					CreateSearchSettingsWidget()
				]
			]
		]
	];
}

void SVoxelGraphSearch::FocusForUse(const FString& NewSearchTerms)
{
	if (!bIsGlobalSearch)
	{
		const TSharedPtr<FVoxelGraphToolkit> Toolkit = WeakToolkit.Pin();
		if (!ensure(Toolkit))
		{
			return;
		}

		const TSharedPtr<FTabManager> TabManager = Toolkit->GetTabManager();
		if (!ensure(TabManager))
		{
			return;
		}

		if (const TSharedPtr<SDockTab> Tab = TabManager->FindExistingLiveTab(FTabId(FVoxelGraphToolkit::SearchTabId)))
		{
			TabManager->DrawAttention(Tab.ToSharedRef());
		}
		else
		{
			TabManager->TryInvokeTab(FTabId(FVoxelGraphToolkit::SearchTabId));
		}
	}

	FWidgetPath SearchTextBoxPath;
	if (FSlateApplication::Get().GeneratePathToWidgetUnchecked(SearchTextField.ToSharedRef(), SearchTextBoxPath))
	{
		FSlateApplication::Get().SetKeyboardFocus(SearchTextBoxPath, EFocusCause::SetDirectly);
	}

	if (NewSearchTerms.IsEmpty())
	{
		return;
	}

	SearchTextField->SetText(FText::FromString(NewSearchTerms));
	SearchValue = NewSearchTerms;
	MakeSearchQuery(SearchValue);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void SVoxelGraphSearch::OnSearchTextCommitted(const FText& Text, const ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		SearchValue = Text.ToString();
		MakeSearchQuery(SearchValue);
	}
}

TSharedRef<SWidget> SVoxelGraphSearch::CreateSearchSettingsWidget()
{
	FMenuBuilder MenuBuilder(true, nullptr);

#define ADD_ENTRY(Label, ToolTip, Variable) \
	MenuBuilder.AddMenuEntry( \
		INVTEXT(Label), \
		INVTEXT(ToolTip), \
		FSlateIcon(), \
		FUIAction(FExecuteAction::CreateLambda([this] \
		{ \
			SearchSettings.Variable = !SearchSettings.Variable; \
			MakeSearchQuery(SearchValue); \
		}), \
		FCanExecuteAction(), \
		FGetActionCheckState::CreateLambda([this] \
		{ \
			return SearchSettings.Variable ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; \
		})), \
		{}, \
		EUserInterfaceActionType::ToggleButton);

	MenuBuilder.BeginSection("", INVTEXT("Show in Search Results"));
	{
		ADD_ENTRY("Nodes", "Search through nodes", bNodesLookup);
		ADD_ENTRY("Pins", "Search through node pins", bPinsLookup);
		ADD_ENTRY("Parameters", "Search through the graph parameter list", bParametersLookup);
		ADD_ENTRY("Parameters Getters and Setters", "Search for parameter getters and setters used within the graph", bParameterGettersAndSettersLookup);
		ADD_ENTRY("Type", "Compare the search term to parameter/pin types in lookup", bTypesLookup);
		ADD_ENTRY("Default Value", "Compare the search term parameter/pin default values in lookup", bDefaultValueLookup);
		ADD_ENTRY("Description", "Search node/parameter/pin descriptions", bDescriptionLookup);
	}
	MenuBuilder.EndSection();

	MenuBuilder.BeginSection("", INVTEXT("Misc"));
	{
		ADD_ENTRY("Remove spaces in Search Results", "Will remove spaces from search lookup string, which will make search results more lenient for partial matches", bRemoveSpacesInLookup);
	}
	MenuBuilder.EndSection();

#undef ADD_ENTRY

	return MenuBuilder.MakeWidget();
}

FReply SVoxelGraphSearch::OnOpenGlobalFindResults() const
{
	if (const TSharedPtr<SVoxelGraphSearch> GlobalSearch = FVoxelGraphSearchManager::Get().OpenGlobalSearch())
	{
		GlobalSearch->SearchSettings = SearchSettings;
		GlobalSearch->FocusForUse(SearchValue);
	}

	return FReply::Handled();
}

TSharedRef<ITableRow> SVoxelGraphSearch::OnGenerateRow(TSharedPtr<FVoxelGraphSearchResult> Item, const TSharedRef<STableViewBase>& OwnerTable) const
{
	return
		SNew(STableRow<TSharedPtr<FVoxelGraphSearchResult>>, OwnerTable)
		.Style(&FAppStyle::GetWidgetStyle<FTableRowStyle>("ShowParentsTableView.Row"))
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.AutoWidth()
			[
				Item->GetIcon()
			]
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.AutoWidth()
			.Padding(2.f)
			[
				SNew(SVoxelDetailText)
				.Text(Item->GetName())
				.HighlightText(HighlightText)
			]
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.AutoWidth()
			.Padding(2.f)
			[
				SNew(SVoxelDetailText)
				.Text(Item->GetSubName())
				.HighlightText(HighlightText)
				.ColorAndOpacity(FSlateColor::UseSubduedForeground())
			]
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Center)
			.FillWidth(1.f)
			.Padding(2.f)
			[
				SNew(SVoxelDetailText)
				.Text(Item->GetComment())
				.HighlightText(HighlightText)
			]
		];
}

void SVoxelGraphSearch::OnGetChildren(TSharedPtr<FVoxelGraphSearchResult> Item, TArray<TSharedPtr<FVoxelGraphSearchResult>>& OutChildren) const
{
	if (!ensure(Item))
	{
		return;
	}

	OutChildren.Append(Item->Children);
}

void SVoxelGraphSearch::OnTreeSelectionDoubleClicked(TSharedPtr<FVoxelGraphSearchResult> Item) const
{
	if (!ensure(Item))
	{
		return;
	}

	Item->OnClick(WeakToolkit);
}

void SVoxelGraphSearch::MakeSearchQuery(const FString& SearchQuery)
{
	TArray<FString> Tokens;
	if (SearchQuery.Contains("\"") &&
		SearchQuery.ParseIntoArray(Tokens, TEXT("\""), true) > 0)
	{
		for (FString& Token : Tokens)
		{
			Token = Token.TrimQuotes().Replace(TEXT(" "), TEXT(""));
		}

		Tokens.RemoveAll([](const FString& Token)
		{
			return Token.IsEmpty();
		});
	}
	else
	{
		SearchQuery.ParseIntoArray(Tokens, TEXT(" "), true);
	}

	ItemsFound = {};

	TArray<TSharedPtr<FVoxelGraphSearchResult>> ItemsToExpand;
	if (Tokens.Num() > 0)
	{
		HighlightText = FText::FromString(SearchValue);
		ItemsToExpand = MatchTokens(Tokens);
	}

	if (ItemsFound.Num() == 0)
	{
		ItemsFound.Add(MakeVoxelShared<FVoxelGraphSearchTextResult>("No Results found"));
	}

	TreeView->RequestTreeRefresh();

	for (const TSharedPtr<FVoxelGraphSearchResult>& Item : ItemsToExpand)
	{
		TreeView->SetItemExpansion(Item, true);
	}
}

TArray<TSharedPtr<FVoxelGraphSearchResult>> SVoxelGraphSearch::MatchTokens(const TArray<FString>& Tokens)
{
	TArray<TSharedPtr<FVoxelGraphSearchResult>> AllItems;
	if (bIsGlobalSearch)
	{
		TMap<UObject*, UVoxelGraph*> Assets = FVoxelGraphSearchManager::Get().GetAllLookupGraphs();

		for (const auto& It : Assets)
		{
			if (!ensure(It.Key) ||
				!ensure(It.Value))
			{
				continue;
			}

			const TSharedRef<FVoxelGraphSearchAssetResult> AssetResult = MakeVoxelShared<FVoxelGraphSearchAssetResult>(It.Key->GetPackage()->GetPathName(), It.Value);
			MatchGraphAsset(It.Value, Tokens, AssetResult, AllItems);

			if (AssetResult->Children.Num() > 0)
			{
				AllItems.Add(AssetResult);
				ItemsFound.Add(AssetResult);
			}
		}
		return AllItems;
	}

	const TSharedPtr<FVoxelGraphToolkit> Toolkit = WeakToolkit.Pin();
	if (!ensure(Toolkit))
	{
		return AllItems;
	}

	MatchGraphAsset(Toolkit->Asset, Tokens, nullptr, AllItems);
	return AllItems;
}

void SVoxelGraphSearch::MatchGraphAsset(UVoxelGraph* Graph, const TArray<FString>& Tokens, const TSharedPtr<FVoxelGraphSearchResult>& AssetResult, TArray<TSharedPtr<FVoxelGraphSearchResult>>& AllItems)
{
	if (!ensure(Graph))
	{
		return;
	}

	{
		const TSharedPtr<FVoxelGraphSearchResult> GraphResult = MakeVoxelShared<FVoxelGraphSearchGraphResult>(Graph);
		if (GraphResult->MatchesTokens(Tokens, SearchSettings, AllItems))
		{
			AllItems.Add(GraphResult);
			if (AssetResult)
			{
				AssetResult->AddChild(GraphResult);
			}
			else
			{
				ItemsFound.Add(GraphResult);
			}
		}
	}

	for (UVoxelGraph* InlineMacro : Graph->InlineMacros)
	{
		if (!ensure(InlineMacro))
		{
			continue;
		}

		const TSharedPtr<FVoxelGraphSearchResult> GraphResult = MakeVoxelShared<FVoxelGraphSearchGraphResult>(InlineMacro);
		if (GraphResult->MatchesTokens(Tokens, SearchSettings, AllItems))
		{
			AllItems.Add(GraphResult);
			if (AssetResult)
			{
				AssetResult->AddChild(GraphResult);
			}
			else
			{
				ItemsFound.Add(GraphResult);
			}
		}
	}
}