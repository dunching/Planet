// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "SVoxelGraphMembersMacroPaletteItem.h"
#include "SchemaActions/VoxelGraphMembersMacroSchemaAction.h"

void SVoxelGraphMembersMacroPaletteItem::Construct(const FArguments& InArgs, FCreateWidgetForActionData* const InCreateData)
{
	ActionPtr = InCreateData->Action;

	ChildSlot
	[
		SNew(SBox)
		.Padding(FMargin(0.f, -2.f))
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.Padding(FMargin(0.f, 2.f, 4.f, 2.f))
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
		]
	];
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedRef<SWidget> SVoxelGraphMembersMacroPaletteItem::CreateTextSlotWidget(FCreateWidgetForActionData* const InCreateData, TAttribute<bool> bIsReadOnly)
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
			.Text(this, &SVoxelGraphMembersMacroPaletteItem::GetDisplayText)
			.HighlightText(InCreateData->HighlightText)
			.OnVerifyTextChanged(this, &SVoxelGraphMembersMacroPaletteItem::OnNameTextVerifyChanged)
			.OnTextCommitted(this, &SVoxelGraphMembersMacroPaletteItem::OnNameTextCommitted)
			.IsSelected(InCreateData->IsRowSelectedDelegate)
			.IsReadOnly(bIsReadOnly)
		];

	InCreateData->OnRenameRequest->BindSP(InlineRenameWidget.Get(), &SInlineEditableTextBlock::EnterEditingMode);

	return DisplayWidget;
}

void SVoxelGraphMembersMacroPaletteItem::OnNameTextCommitted(const FText& NewText, ETextCommit::Type InTextCommit)
{
	const TSharedPtr<FVoxelGraphMembersMacroSchemaAction> Action = GetAction();
	if (!ensure(Action))
	{
		return;
	}

	Action->SetName(NewText.ToString());
}

FText SVoxelGraphMembersMacroPaletteItem::GetDisplayText() const
{
	const TSharedPtr<FVoxelGraphMembersMacroSchemaAction> Action = GetAction();
	if (!ensure(Action))
	{
		return {};
	}

	return FText::FromString(Action->GetName());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedPtr<FVoxelGraphMembersMacroSchemaAction> SVoxelGraphMembersMacroPaletteItem::GetAction() const
{
	const TSharedPtr<FEdGraphSchemaAction> Action = ActionPtr.Pin();
	if (!Action)
	{
		return nullptr;
	}

	return StaticCastSharedPtr<FVoxelGraphMembersMacroSchemaAction>(Action);
}