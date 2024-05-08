// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "SVoxelGraphMembersVariablePaletteItem.h"

#include "VoxelNodeLibrary.h"
#include "Widgets/SVoxelMembers.h"
#include "Widgets/SVoxelGraphMembers.h"
#include "Widgets/SVoxelGraphPinTypeComboBox.h"
#include "SchemaActions/VoxelGraphMembersVariableSchemaAction.h"

void SVoxelGraphMembersVariablePaletteItem::Construct(const FArguments& InArgs, FCreateWidgetForActionData* const InCreateData)
{
	WeakMembersWidget = InArgs._MembersWidget;
	ActionPtr = InCreateData->Action;

	const FVoxelMembersColumnSizeData& ColumnSizeData = InArgs._MembersWidget->GetColumnSizeData();
	bool bIsParameters;
	FVoxelPinType Type;

	{
		const TSharedPtr<FVoxelGraphMembersVariableSchemaAction> Action = GetAction();
		if (!ensure(Action))
		{
			return;
		}

		TArray<FString> Categories;
		Action->GetCategory().ToString().ParseIntoArray(Categories, TEXT("|"));
		CategoriesCount = Categories.Num();

		Type = Action->GetPinType();
		bIsParameters = SVoxelGraphMembers::GetSection(Action->SectionID) == SVoxelGraphMembers::ESection::Parameters;
	}

	ChildSlot
	[
		SNew(SBox)
		.Padding(FMargin(0.f, -2.f))
		[
			SNew(SSplitter)
			.Style(FVoxelEditorStyle::Get(), "Members.Splitter")
			.PhysicalSplitterHandleSize(1.f)
			.HitDetectionSplitterHandleSize(5.f)
			.HighlightedHandleIndex(ColumnSizeData.HoveredSplitterIndex)
			.OnHandleHovered(ColumnSizeData.OnSplitterHandleHovered)
			+ SSplitter::Slot()
			.Value_Lambda([&]
			{
				const TSharedPtr<SVoxelMembers> MembersWidget = WeakMembersWidget.Pin();
				if (!ensure(MembersWidget))
				{
					return 0.f;
				}

				return MembersWidget->GetColumnSizeData().NameColumnWidth.Execute(CategoriesCount, GetCachedGeometry().GetAbsoluteSize().X);
			})
			.OnSlotResized(ColumnSizeData.OnNameColumnResized)
			[
				SNew(SBox)
				.Padding(FMargin(0.f, 2.f, 4.f, 2.f))
				[
					CreateTextSlotWidget(InCreateData, false)
				]
			]
			+ SSplitter::Slot()
			.Value_Lambda([&]
			{
				const TSharedPtr<SVoxelMembers> MembersWidget = WeakMembersWidget.Pin();
				if (!ensure(MembersWidget))
				{
					return 0.f;
				}

				return MembersWidget->GetColumnSizeData().ValueColumnWidth.Execute(CategoriesCount, GetCachedGeometry().GetAbsoluteSize().X);
			})
			.OnSlotResized_Lambda([&](float NewValue)
			{
				const TSharedPtr<SVoxelMembers> MembersWidget = WeakMembersWidget.Pin();
				if (!ensure(MembersWidget))
				{
					return;
				}

				MembersWidget->GetColumnSizeData().OnValueColumnResized.ExecuteIfBound(NewValue, CategoriesCount, GetCachedGeometry().GetAbsoluteSize().X);
			})
			[
				SNew(SBox)
				.Padding(FMargin(8.f, 2.f, 0.f, 2.f))
				.HAlign(HAlign_Left)
				[
					SNew(SVoxelPinTypeComboBox)
					.CurrentType(Type)
					.OnTypeChanged_Lambda([this](const FVoxelPinType NewValue)
					{
						const TSharedPtr<FVoxelGraphMembersVariableSchemaAction> Action = GetAction();
						if (!ensure(Action))
						{
							return;
						}

						Action->SetPinType(NewValue);
					})
					.AllowedTypes(bIsParameters ? FVoxelPinTypeSet::AllUniformsAndBufferArrays() : FVoxelPinTypeSet::All())
					.DetailsWindow(false)
					.ReadOnly_Lambda([this]
					{
						return !IsHovered();
					})
				]
			]
		]
	];
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedRef<SWidget> SVoxelGraphMembersVariablePaletteItem::CreateTextSlotWidget(FCreateWidgetForActionData* const InCreateData, const TAttribute<bool> bIsReadOnly)
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
			.Text(this, &SVoxelGraphMembersVariablePaletteItem::GetDisplayText)
			.HighlightText(InCreateData->HighlightText)
			.OnVerifyTextChanged(this, &SVoxelGraphMembersVariablePaletteItem::OnNameTextVerifyChanged)
			.OnTextCommitted(this, &SVoxelGraphMembersVariablePaletteItem::OnNameTextCommitted)
			.IsSelected(InCreateData->IsRowSelectedDelegate)
			.IsReadOnly(bIsReadOnly)
		];

	InCreateData->OnRenameRequest->BindSP(InlineRenameWidget.Get(), &SInlineEditableTextBlock::EnterEditingMode);

	return DisplayWidget;
}

void SVoxelGraphMembersVariablePaletteItem::OnNameTextCommitted(const FText& NewText, ETextCommit::Type InTextCommit)
{
	const TSharedPtr<FVoxelGraphMembersVariableSchemaAction> Action = GetAction();
	if (!ensure(Action))
	{
		return;
	}

	Action->SetName(NewText.ToString());
}

FText SVoxelGraphMembersVariablePaletteItem::GetDisplayText() const
{
	const TSharedPtr<FVoxelGraphMembersVariableSchemaAction> Action = GetAction();
	if (!ensure(Action))
	{
		return {};
	}

	return FText::FromString(Action->GetName());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedPtr<FVoxelGraphMembersVariableSchemaAction> SVoxelGraphMembersVariablePaletteItem::GetAction() const
{
	const TSharedPtr<FEdGraphSchemaAction> Action = ActionPtr.Pin();
	if (!Action)
	{
		return nullptr;
	}

	return StaticCastSharedPtr<FVoxelGraphMembersVariableSchemaAction>(Action);
}