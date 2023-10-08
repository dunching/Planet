// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "SVoxelMaterialDefinitionParameterPaletteItem.h"
#include "VoxelMaterialDefinitionParameterSchemaAction.h"
#include "Material/VoxelMaterialDefinition.h"
#include "Widgets/SVoxelMembers.h"
#include "Widgets/SVoxelGraphPinTypeComboBox.h"

void SVoxelMaterialDefinitionParameterPaletteItem::Construct(const FArguments& InArgs, FCreateWidgetForActionData* const InCreateData)
{
	WeakParametersWidget = InArgs._ParametersWidget;
	ActionPtr = InCreateData->Action;

	const FVoxelMembersColumnSizeData& ColumnSizeData = InArgs._ParametersWidget->GetColumnSizeData();
	FVoxelPinType Type;

	{
		const TSharedPtr<FVoxelMaterialDefinitionParameterSchemaAction> Action = GetAction();
		if (!ensure(Action))
		{
			return;
		}

		TArray<FString> Categories;
		Action->GetCategory().ToString().ParseIntoArray(Categories, TEXT("|"));
		CategoriesCount = Categories.Num();

		Type = Action->GetPinType();
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
				const TSharedPtr<SVoxelMembers> ParametersWidget = WeakParametersWidget.Pin();
				if (!ensure(ParametersWidget))
				{
					return 0.f;
				}

				return ParametersWidget->GetColumnSizeData().NameColumnWidth.Execute(CategoriesCount, GetCachedGeometry().GetAbsoluteSize().X);
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
				const TSharedPtr<SVoxelMembers> ParametersWidget = WeakParametersWidget.Pin();
				if (!ensure(ParametersWidget))
				{
					return 0.f;
				}

				return ParametersWidget->GetColumnSizeData().ValueColumnWidth.Execute(CategoriesCount, GetCachedGeometry().GetAbsoluteSize().X);
			})
			.OnSlotResized_Lambda([&](float NewValue)
			{
				const TSharedPtr<SVoxelMembers> ParametersWidget = WeakParametersWidget.Pin();
				if (!ensure(ParametersWidget))
				{
					return;
				}

				ParametersWidget->GetColumnSizeData().OnValueColumnResized.ExecuteIfBound(NewValue, CategoriesCount, GetCachedGeometry().GetAbsoluteSize().X);
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
						const TSharedPtr<FVoxelMaterialDefinitionParameterSchemaAction> Action = GetAction();
						if (!ensure(Action))
						{
							return;
						}

						Action->SetPinType(NewValue);
					})
					.AllowedTypes(FVoxelPinTypeSet::AllMaterials())
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

TSharedRef<SWidget> SVoxelMaterialDefinitionParameterPaletteItem::CreateTextSlotWidget(FCreateWidgetForActionData* const InCreateData, const TAttribute<bool> bIsReadOnly)
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
			.Text(this, &SVoxelMaterialDefinitionParameterPaletteItem::GetDisplayText)
			.HighlightText(InCreateData->HighlightText)
			.OnVerifyTextChanged(this, &SVoxelMaterialDefinitionParameterPaletteItem::OnNameTextVerifyChanged)
			.OnTextCommitted(this, &SVoxelMaterialDefinitionParameterPaletteItem::OnNameTextCommitted)
			.IsSelected(InCreateData->IsRowSelectedDelegate)
			.IsReadOnly(bIsReadOnly)
		];

	InCreateData->OnRenameRequest->BindSP(InlineRenameWidget.Get(), &SInlineEditableTextBlock::EnterEditingMode);

	return DisplayWidget;
}

void SVoxelMaterialDefinitionParameterPaletteItem::OnNameTextCommitted(const FText& NewText, ETextCommit::Type InTextCommit)
{
	const TSharedPtr<FVoxelMaterialDefinitionParameterSchemaAction> Action = GetAction();
	if (!ensure(Action))
	{
		return;
	}

	Action->SetName(NewText.ToString());
}

FText SVoxelMaterialDefinitionParameterPaletteItem::GetDisplayText() const
{
	const TSharedPtr<FVoxelMaterialDefinitionParameterSchemaAction> Action = GetAction();
	if (!ensure(Action))
	{
		return {};
	}

	return FText::FromString(Action->GetName());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedPtr<FVoxelMaterialDefinitionParameterSchemaAction> SVoxelMaterialDefinitionParameterPaletteItem::GetAction() const
{
	const TSharedPtr<FEdGraphSchemaAction> Action = ActionPtr.Pin();
	if (!Action)
	{
		return nullptr;
	}

	return StaticCastSharedPtr<FVoxelMaterialDefinitionParameterSchemaAction>(Action);
}