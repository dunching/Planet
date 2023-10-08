// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VoxelEditorMinimal.h"
#include "VoxelGraph.h"

struct FVoxelGraphListNode
{
	const FAssetData Asset;
	const TSharedRef<FAssetThumbnail> AssetThumbnail;
	const FString LookupName;
	const TWeakObjectPtr<const UVoxelGraph> WeakGraph;

	explicit FVoxelGraphListNode(const FAssetData& Asset, const UVoxelGraph* Graph)
		: Asset(Asset)
		, AssetThumbnail(MakeVoxelShared<FAssetThumbnail>(Asset, 36, 36, FVoxelEditorUtilities::GetThumbnailPool()))
		, LookupName(Graph->GetGraphName().ToLower())
		, WeakGraph(Graph)
	{
	}

	FText GetName() const
	{
		if (const UVoxelGraph* Graph = WeakGraph.Get())
		{
			return FText::FromString(Graph->GetGraphName());
		}

		return INVTEXT("None");
	}
	FText GetDescription() const
	{
		if (const UVoxelGraph* Graph = WeakGraph.Get())
		{
			return FText::FromString(Graph->Description);
		}

		return INVTEXT("");
	}
};

class VOXELGRAPHEDITOR_API SVoxelGraphSelector : public SCompoundWidget
{
public:
	DECLARE_DELEGATE_RetVal_OneParam(bool, FIsGraphValid, const UVoxelGraph*);
	DECLARE_DELEGATE_OneParam(FOnGraphSelected, UVoxelGraph*);

	VOXEL_SLATE_ARGS()
	{
		SLATE_EVENT(FIsGraphValid, IsGraphValid);
		SLATE_EVENT(FOnGraphSelected, OnGraphSelected);
	};

	void Construct(const FArguments& InArgs);

private:
	void OnSearchChanged(const FText& Text);

	TSharedRef<ITableRow> OnGenerateRow(TSharedPtr<FVoxelGraphListNode> Node, const TSharedRef<STableViewBase>& OwningTable);
	void OnSelectionChanged(TSharedPtr<FVoxelGraphListNode> Node, ESelectInfo::Type SelectType);

	void FindAllGraphs();
	void FilterGraphs();
	void RebuildList();

private:
	FOnGraphSelected OnGraphSelected;
	FIsGraphValid IsGraphValid;

private:
	TSharedPtr<SListView<TSharedPtr<FVoxelGraphListNode>>> GraphsListView;
	TArray<TSharedPtr<FVoxelGraphListNode>> GraphsList;
	TArray<TSharedPtr<FVoxelGraphListNode>> FilteredGraphsList;
	TWeakObjectPtr<UVoxelGraph> ActiveGraph;

	FString LookupString;
	FText LookupText;
};