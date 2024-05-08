// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "VoxelGraph.h"
#include "VoxelPinType.h"
#include "VoxelParameter.h"

struct FSelectorParameterRow;
typedef STreeView<TSharedPtr<FSelectorParameterRow>> SParameterTreeView;

class SVoxelGraphParameterSelector : public SCompoundWidget
{
public:
	DECLARE_DELEGATE_OneParam(FOnParameterChanged, const FVoxelParameter&)

public:
	VOXEL_SLATE_ARGS()
	{
		SLATE_ARGUMENT(TWeakInterfacePtr<IVoxelParameterProvider>, ParameterProvider)
		SLATE_EVENT(FOnParameterChanged, OnParameterChanged)
		SLATE_EVENT(FSimpleDelegate, OnCloseMenu)
	};

	void Construct(const FArguments& InArgs);

public:
	void ClearSelection() const;
	TSharedPtr<SWidget> GetWidgetToFocus() const;
	void UpdateParameterProvider(const TWeakInterfacePtr<IVoxelParameterProvider>& ParameterProvider);
	FVoxelParameter GetUpdatedParameter(const FVoxelParameter& TargetParameter) const;

private:
	TSharedRef<ITableRow> GenerateParameterTreeRow(TSharedPtr<FSelectorParameterRow> RowItem, const TSharedRef<STableViewBase>& OwnerTable) const;
	void OnParameterSelectionChanged(TSharedPtr<FSelectorParameterRow> Selection, ESelectInfo::Type SelectInfo) const;
	void GetParameterChildren(TSharedPtr<FSelectorParameterRow> ParameterRow, TArray<TSharedPtr<FSelectorParameterRow>>& ChildrenRows) const;

	void OnFilterTextChanged(const FText& NewText);
	void OnFilterTextCommitted(const FText& NewText, ETextCommit::Type CommitInfo) const;

private:
	void GetChildrenMatchingSearch(const FText& InSearchText);
	void FillParametersList();
	const FSlateBrush* GetIcon(const FVoxelPinType& PinType) const;
	FLinearColor GetColor(const FVoxelPinType& PinType) const;
	TArray<FString> GetCategoryChain(const FString& Category) const;

private:
	TSharedPtr<SParameterTreeView> ParametersTreeView;
	TSharedPtr<SSearchBox> FilterTextBox;

private:
	TWeakInterfacePtr<IVoxelParameterProvider> CachedParameterProvider;
	FOnParameterChanged OnParameterChanged;
	FSimpleDelegate OnCloseMenu;

private:
	FText SearchText;

	TArray<TSharedPtr<FSelectorParameterRow>> ParametersList;
	TArray<TSharedPtr<FSelectorParameterRow>> FilteredParametersList;
	TWeakPtr<FSelectorParameterRow> AutoExpandedRow;

	int32 FilteredParametersCount = 0;
	int32 TotalParametersCount = 0;
};