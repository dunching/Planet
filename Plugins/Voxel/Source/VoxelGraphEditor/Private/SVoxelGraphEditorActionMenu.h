// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"

class SGraphActionMenu;
struct FCustomExpanderData;
struct FEdGraphSchemaAction;
struct FCreateWidgetForActionData;
struct FGraphActionListBuilderBase;

class SVoxelGraphEditorActionMenu : public SBorder
{
public:
	VOXEL_SLATE_ARGS()
	{
		SLATE_ARGUMENT(UEdGraph*, GraphObj)
		SLATE_ARGUMENT(FVector2D, NewNodePosition)
		SLATE_ARGUMENT(TArray<UEdGraphPin*>, DraggedFromPins)
		SLATE_ARGUMENT(SGraphEditor::FActionMenuClosed, OnClosedCallback)
		SLATE_ARGUMENT(bool, AutoExpandActionMenu)
	};

	void Construct(const FArguments& InArgs);

	virtual ~SVoxelGraphEditorActionMenu() override;

	TSharedRef<SEditableTextBox> GetFilterTextBox() const;

protected:
	TWeakObjectPtr<UEdGraph> WeakGraphObj = nullptr;
	TArray<UEdGraphPin*> DraggedFromPins;
	FVector2D NewNodePosition = FVector2D::ZeroVector;
	bool bAutoExpandActionMenu = false;

	SGraphEditor::FActionMenuClosed OnClosedCallback;

	TSharedPtr<SGraphActionMenu> GraphActionMenu;

	void CollectAllActions(FGraphActionListBuilderBase& OutAllActions);
	void OnActionSelected(const TArray<TSharedPtr<FEdGraphSchemaAction>>& SelectedActions, ESelectInfo::Type InSelectionType);
	TSharedRef<SExpanderArrow> CreateActionExpander(const FCustomExpanderData& ActionMenuData) const;
	TSharedRef<SWidget> OnCreateWidgetForAction(FCreateWidgetForActionData* CreateData) const;
};