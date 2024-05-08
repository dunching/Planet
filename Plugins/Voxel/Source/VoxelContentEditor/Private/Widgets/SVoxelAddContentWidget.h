// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "Misc/TextFilter.h"
#include "VoxelExampleContentManager.h"

class SVoxelAddContentWidget : public SCompoundWidget
{
public:
	typedef TTextFilter<TSharedPtr<FVoxelExampleContent>> FContentSourceTextFilter;

public:
	VOXEL_SLATE_ARGS()
	{
	};

	void Construct(const FArguments& InArgs);

private:
	FReply AddButtonClicked();
	FReply CancelButtonClicked();

	void SearchTextChanged(const FText& Text);

	TSharedRef<SWidget> CreateContentTilesView();
	TSharedRef<SWidget> CreateContentDetails(const TSharedPtr<FVoxelExampleContent>& Selection) const;

	TSharedRef<ITableRow> CreateContentSourceIconTile(TSharedPtr<FVoxelExampleContent> Content, const TSharedRef<STableViewBase>& OwnerTable) const;

private:
	TSharedPtr<SBox> CategoryTabsBox;
	TSharedPtr<SBox> ContentDetailsBox;
	TSharedPtr<SSearchBox> SearchBox;
	TSharedPtr<STileView<TSharedPtr<FVoxelExampleContent>>> ContentTilesView;

	TArray<TSharedPtr<FVoxelExampleContent>> ContentsSourceList;
	TArray<TSharedPtr<FVoxelExampleContent>> FilteredList;

	TSharedPtr<FVoxelExampleContent> SelectedContent;

	TSharedPtr<FContentSourceTextFilter> Filter;

	void UpdateFilteredItems();
	void FinalizeDownload(const TArray<uint8>& InData) const;
};