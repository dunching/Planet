// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "SVoxelGraphEditorActionMenu.h"

#include "VoxelGraphSchemaAction.h"
#include "SGraphPalette.h"
#include "SGraphActionMenu.h"

class SVoxelGraphActionWidget : public SCompoundWidget
{
public:
	VOXEL_SLATE_ARGS()
	{
		SLATE_ATTRIBUTE(FText, HighlightText)
	};

	void Construct(const FArguments& InArgs, const FCreateWidgetForActionData* InCreateData)
	{
		ActionPtr = InCreateData->Action;
		MouseButtonDownDelegate = InCreateData->MouseButtonDownDelegate;

		TSharedPtr<SImage> ImageWidget;
		FSlateIcon Icon("EditorStyle", "NoBrush");
		FLinearColor Color = FLinearColor::White;

		if (InCreateData->Action->GetTypeId() == FVoxelGraphSchemaAction::StaticGetTypeId())
		{
			TSharedPtr<FVoxelGraphSchemaAction> Action = StaticCastSharedPtr<FVoxelGraphSchemaAction>(InCreateData->Action);
			Action->GetIcon(Icon, Color);
		}

		SAssignNew(ImageWidget, SImage)
		.Image(Icon.GetIcon())
		.ColorAndOpacity(Color);

		this->ChildSlot
		[
			SNew(SHorizontalBox)
			.ToolTipText(InCreateData->Action->GetTooltipDescription())
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				ImageWidget.ToSharedRef()
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(FMargin(6.0f, 0.0f, 3.0f, 0.0f))
			[
				SNew(STextBlock)
				.Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
				.Text(InCreateData->Action->GetMenuDescription())
				.HighlightText(InArgs._HighlightText)
			]
		];
	}

	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override
	{
		if (MouseButtonDownDelegate.Execute(ActionPtr))
		{
			return FReply::Handled();
		}

		return FReply::Unhandled();
	}

private:
	TWeakPtr<FEdGraphSchemaAction> ActionPtr;
	FCreateWidgetMouseButtonDown MouseButtonDownDelegate;
};

void SVoxelGraphEditorActionMenu::Construct(const FArguments& InArgs)
{
	WeakGraphObj = InArgs._GraphObj;
	DraggedFromPins = InArgs._DraggedFromPins;
	NewNodePosition = InArgs._NewNodePosition;
	OnClosedCallback = InArgs._OnClosedCallback;
	bAutoExpandActionMenu = InArgs._AutoExpandActionMenu;

	SBorder::Construct(
		SBorder::FArguments()
		.BorderImage(FAppStyle::GetBrush("Menu.Background"))
		.Padding(5.f)
		[
			SNew(SBox)
			.WidthOverride(400.f)
			.HeightOverride(400.f)
			[
				SAssignNew(GraphActionMenu, SGraphActionMenu)
				.OnActionSelected(this, &SVoxelGraphEditorActionMenu::OnActionSelected)
				.OnCollectAllActions(this, &SVoxelGraphEditorActionMenu::CollectAllActions)
				.AutoExpandActionMenu(bAutoExpandActionMenu)
				.DraggedFromPins(DraggedFromPins)
				.GraphObj(WeakGraphObj.Get())
				.OnCreateCustomRowExpander(this, &SVoxelGraphEditorActionMenu::CreateActionExpander)
				.OnCreateWidgetForAction(this, &SVoxelGraphEditorActionMenu::OnCreateWidgetForAction)
			]
		]
	);
}

SVoxelGraphEditorActionMenu::~SVoxelGraphEditorActionMenu()
{
	OnClosedCallback.ExecuteIfBound();
}

TSharedRef<SEditableTextBox> SVoxelGraphEditorActionMenu::GetFilterTextBox() const
{
	return GraphActionMenu->GetFilterTextBox();
}

void SVoxelGraphEditorActionMenu::CollectAllActions(FGraphActionListBuilderBase& OutAllActions)
{
	VOXEL_FUNCTION_COUNTER();

	if (!ensure(WeakGraphObj.IsValid()))
	{
		return;
	}

	FGraphContextMenuBuilder ContextMenuBuilder(WeakGraphObj.Get());
	if (DraggedFromPins.Num() != 0)
	{
		ContextMenuBuilder.FromPin = DraggedFromPins[0];
	}

	WeakGraphObj->GetSchema()->GetGraphContextActions(ContextMenuBuilder);
	OutAllActions.Append(ContextMenuBuilder);
}

void SVoxelGraphEditorActionMenu::OnActionSelected(const TArray<TSharedPtr<FEdGraphSchemaAction>>& SelectedActions, ESelectInfo::Type InSelectionType)
{
	if (InSelectionType != ESelectInfo::OnMouseClick &&
		InSelectionType != ESelectInfo::OnKeyPress &&
		SelectedActions.Num() != 0)
	{
		return;
	}

	if (!WeakGraphObj.IsValid())
	{
		return;
	}

	for (const TSharedPtr<FEdGraphSchemaAction>& CurrentAction : SelectedActions)
	{
		if (!CurrentAction.IsValid())
		{
			continue;
		}

		FSlateApplication::Get().DismissAllMenus();
		CurrentAction->PerformAction(WeakGraphObj.Get(), DraggedFromPins, NewNodePosition);
	}
}

TSharedRef<SExpanderArrow> SVoxelGraphEditorActionMenu::CreateActionExpander(const FCustomExpanderData& ActionMenuData) const
{
	return SNew(SExpanderArrow, ActionMenuData.TableRow);
}

TSharedRef<SWidget> SVoxelGraphEditorActionMenu::OnCreateWidgetForAction(FCreateWidgetForActionData* CreateData) const
{
	return SNew(SVoxelGraphActionWidget, CreateData);
}