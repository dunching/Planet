// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "SVoxelGraphMembersMacroLibraryPaletteItem.h"
#include "SchemaActions/VoxelGraphMembersMacroLibrarySchemaAction.h"

void SVoxelGraphMembersMacroLibraryPaletteItem::Construct(const FArguments& InArgs, FCreateWidgetForActionData* const InCreateData)
{
	ActionPtr = InCreateData->Action;

	ChildSlot
	[
		SNew(SBox)
		.Padding(0.f, -2.f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.Padding(0.f, 2.f, 4.f, 2.f)
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Left)
			.AutoWidth()
			[
				SNew(SImage)
				.Image(FAppStyle::GetBrush(TEXT("GraphEditor.Macro_16x")))
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.f)
			.Padding(0.f)
			[
				CreateTextSlotWidget(InCreateData, false)
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(4.f, 0.f)
			[
				CreateVisibilityWidget()
			]
		]
	];
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedRef<SWidget> SVoxelGraphMembersMacroLibraryPaletteItem::CreateTextSlotWidget(FCreateWidgetForActionData* const InCreateData, TAttribute<bool> bIsReadOnly)
{
	if (InCreateData->bHandleMouseButtonDown)
	{
		MouseButtonDownDelegate = InCreateData->MouseButtonDownDelegate;
	}

	TSharedRef<SOverlay> DisplayWidget =
		SNew(SOverlay)
		+ SOverlay::Slot()
		[
			SAssignNew(InlineRenameWidget, SInlineEditableTextBlock)
			.Text(this, &SVoxelGraphMembersMacroLibraryPaletteItem::GetDisplayText)
			.HighlightText(InCreateData->HighlightText)
			.OnVerifyTextChanged(this, &SVoxelGraphMembersMacroLibraryPaletteItem::OnNameTextVerifyChanged)
			.OnTextCommitted(this, &SVoxelGraphMembersMacroLibraryPaletteItem::OnNameTextCommitted)
			.IsSelected(InCreateData->IsRowSelectedDelegate)
			.IsReadOnly(bIsReadOnly)
		];

	InCreateData->OnRenameRequest->BindSP(InlineRenameWidget.Get(), &SInlineEditableTextBlock::EnterEditingMode);

	return DisplayWidget;
}

void SVoxelGraphMembersMacroLibraryPaletteItem::OnNameTextCommitted(const FText& NewText, ETextCommit::Type InTextCommit)
{
	const TSharedPtr<FVoxelGraphMembersMacroLibrarySchemaAction> Action = GetAction();
	if (!ensure(Action))
	{
		return;
	}

	Action->SetName(NewText.ToString());
}

FText SVoxelGraphMembersMacroLibraryPaletteItem::GetDisplayText() const
{
	const TSharedPtr<FVoxelGraphMembersMacroLibrarySchemaAction> Action = GetAction();
	if (!ensure(Action))
	{
		return {};
	}

	return FText::FromString(Action->GetName());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedRef<SWidget> SVoxelGraphMembersMacroLibraryPaletteItem::CreateVisibilityWidget() const
{
	TSharedRef<SWidget> Button = PropertyCustomizationHelpers::MakeVisibilityButton(
		FOnClicked::CreateLambda([this]() -> FReply
		{
			const TSharedPtr<FVoxelGraphMembersMacroLibrarySchemaAction> Action = GetAction();
			if (!ensure(Action))
			{
				return FReply::Handled();
			}

			Action->ToggleMacroVisibility();
			return FReply::Handled();
		}),
		{},
		MakeAttributeLambda([this]
		{
			const TSharedPtr<FVoxelGraphMembersMacroLibrarySchemaAction> Action = GetAction();
			if (!ensure(Action))
			{
				return false;
			}

			return Action->IsMacroVisible();
		}));

	Button->SetToolTipText(INVTEXT("Expose Macro"));

	return Button;
}

TSharedPtr<FVoxelGraphMembersMacroLibrarySchemaAction> SVoxelGraphMembersMacroLibraryPaletteItem::GetAction() const
{
	const TSharedPtr<FEdGraphSchemaAction> Action = ActionPtr.Pin();
	if (!Action)
	{
		return nullptr;
	}

	return StaticCastSharedPtr<FVoxelGraphMembersMacroLibrarySchemaAction>(Action);
}