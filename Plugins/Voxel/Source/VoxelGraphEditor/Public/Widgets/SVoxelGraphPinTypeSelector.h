// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "VoxelPinType.h"

struct FSelectorPinTypeRow;
typedef STreeView<TSharedPtr<FSelectorPinTypeRow>> SPinTypeTreeView;

class VOXELGRAPHEDITOR_API SVoxelPinTypeSelector : public SCompoundWidget
{
public:
	DECLARE_DELEGATE_OneParam(FOnTypeChanged, FVoxelPinType)

public:
	VOXEL_SLATE_ARGS()
	{
		SLATE_ATTRIBUTE(FVoxelPinTypeSet, AllowedTypes)
		SLATE_EVENT(FOnTypeChanged, OnTypeChanged)
		SLATE_EVENT(FSimpleDelegate, OnCloseMenu)
	};

	void Construct(const FArguments& InArgs);

public:
	void ClearSelection() const;
	TSharedPtr<SWidget> GetWidgetToFocus() const;

private:
	TSharedRef<ITableRow> GenerateTypeTreeRow(TSharedPtr<FSelectorPinTypeRow> RowItem, const TSharedRef<STableViewBase>& OwnerTable) const;
	void OnTypeSelectionChanged(TSharedPtr<FSelectorPinTypeRow> Selection, ESelectInfo::Type SelectInfo) const;
	void GetTypeChildren(TSharedPtr<FSelectorPinTypeRow> PinTypeRow, TArray<TSharedPtr<FSelectorPinTypeRow>>& PinTypeRows) const;

	void OnFilterTextChanged(const FText& NewText);
	void OnFilterTextCommitted(const FText& NewText, ETextCommit::Type CommitInfo) const;

private:
	void GetChildrenMatchingSearch(const FText& InSearchText);
	void FillTypesList();
	const FSlateBrush* GetIcon(const FVoxelPinType& PinType) const;
	FLinearColor GetColor(const FVoxelPinType& PinType) const;

private:
	TSharedPtr<SPinTypeTreeView> TypeTreeView;
	TSharedPtr<SSearchBox> FilterTextBox;

private:
	TAttribute<FVoxelPinTypeSet> AllowedTypes;
	FOnTypeChanged OnTypeChanged;
	FSimpleDelegate OnCloseMenu;

private:
	FText SearchText;

	TArray<TSharedPtr<FSelectorPinTypeRow>> TypesList;
	TArray<TSharedPtr<FSelectorPinTypeRow>> FilteredTypesList;

	int32 FilteredTypesCount = 0;
	int32 TotalTypesCount = 0;
};