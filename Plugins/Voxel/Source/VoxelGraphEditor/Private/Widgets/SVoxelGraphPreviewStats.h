// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"

class SVoxelGraphPreviewStats : public SCompoundWidget
{
public:
	VOXEL_SLATE_ARGS() {};

	struct FRow
	{
		FText Header;
		FText Tooltip;
		TAttribute<FText> Value;
	};
	TArray<TSharedPtr<FRow>> Rows;
	TSharedPtr<SListView<TSharedPtr<FRow>>> RowsView;

	void Construct(const FArguments& Args);

private:
	TSharedRef<ITableRow> CreateRow(TSharedPtr<FRow> StatsRow, const TSharedRef<STableViewBase>& OwnerTable) const;
};