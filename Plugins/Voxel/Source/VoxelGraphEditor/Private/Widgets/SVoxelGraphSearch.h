// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "VoxelGraphToolkit.h"

class SSearchBox;
class UVoxelGraph;
struct FVoxelGraphSearchResult;
struct FVoxelGraphToolkit;

struct FVoxelGraphSearchSettings
{
	bool bNodesLookup = true;
	bool bPinsLookup = true;
	bool bParametersLookup = false;
	bool bParameterGettersAndSettersLookup = true;
	bool bTypesLookup = false;
	bool bDefaultValueLookup = false;
	bool bDescriptionLookup = true;
	bool bRemoveSpacesInLookup = false;
};

class SVoxelGraphSearch : public SCompoundWidget
{
public:
	VOXEL_SLATE_ARGS()
	{
		FArguments() : _IsGlobalSearch(false)
		{
		}

		SLATE_ARGUMENT(TSharedPtr<FVoxelGraphToolkit>, Toolkit);
		SLATE_ARGUMENT(bool, IsGlobalSearch);
	};

	void Construct(const FArguments& InArgs);
	void FocusForUse(const FString& NewSearchTerms);

private:
	void OnSearchTextCommitted(const FText& Text, ETextCommit::Type CommitType);

	TSharedRef<SWidget> CreateSearchSettingsWidget();
	FReply OnOpenGlobalFindResults() const;

	TSharedRef<ITableRow> OnGenerateRow(TSharedPtr<FVoxelGraphSearchResult> Item, const TSharedRef<STableViewBase>& OwnerTable) const;
	void OnGetChildren(TSharedPtr<FVoxelGraphSearchResult> Item, TArray<TSharedPtr<FVoxelGraphSearchResult>>& OutChildren) const;
	void OnTreeSelectionDoubleClicked(TSharedPtr<FVoxelGraphSearchResult> Item) const;

	void MakeSearchQuery(const FString& SearchQuery);
	TArray<TSharedPtr<FVoxelGraphSearchResult>> MatchTokens(const TArray<FString>& Tokens);
	void MatchGraphAsset(UVoxelGraph* Graph, const TArray<FString>& Tokens, const TSharedPtr<FVoxelGraphSearchResult>& AssetResult, TArray<TSharedPtr<FVoxelGraphSearchResult>>& AllItems);

private:
	using STreeViewType = STreeView<TSharedPtr<FVoxelGraphSearchResult>>;

	TSharedPtr<SSearchBox> SearchTextField;
	TSharedPtr<STreeViewType> TreeView;

	FString	SearchValue;
	FText HighlightText;

	TArray<TSharedPtr<FVoxelGraphSearchResult>> ItemsFound;

	TWeakPtr<FVoxelGraphToolkit> WeakToolkit;
	bool bIsGlobalSearch = false;

	FVoxelGraphSearchSettings SearchSettings;
};