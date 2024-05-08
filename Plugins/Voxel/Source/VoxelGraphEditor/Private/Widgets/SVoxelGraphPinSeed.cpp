// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "SVoxelGraphPinSeed.h"
#include "VoxelPinValue.h"
#include "VoxelExposedSeed.h"

void SVoxelGraphPinSeed::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
{
	SGraphPin::Construct(SGraphPin::FArguments(), InGraphPinObj);
}

TSharedRef<SWidget>	SVoxelGraphPinSeed::GetDefaultValueWidget()
{
	return
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		[
			SNew(SBox)
			.MinDesiredWidth(18)
			.MaxDesiredWidth(400)
			[
				SNew(SEditableTextBox)
				.Style(FAppStyle::Get(), "Graph.EditableTextBox")
				.Text_Lambda([this]
				{
					const FVoxelPinValue DefaultValue = FVoxelPinValue::MakeFromPinDefaultValue(*GraphPinObj);
					if (!ensure(DefaultValue.Is<FVoxelExposedSeed>()))
					{
						return FText();
					}
					return FText::FromString(DefaultValue.Get<FVoxelExposedSeed>().Seed);
				})
				.SelectAllTextWhenFocused(true)
				.Visibility(this, &SGraphPin::GetDefaultValueVisibility)
				.IsReadOnly_Lambda([this]() -> bool
				{
					return GraphPinObj->bDefaultValueIsReadOnly;
				})
				.OnTextCommitted_Lambda([this](const FText& NewValue, ETextCommit::Type)
				{
					if (!ensure(!GraphPinObj->IsPendingKill()) ||
						GraphPinObj->DefaultValue.Equals(NewValue.ToString()))
					{
						return;
					}

					const FVoxelTransaction Transaction(GraphPinObj, "Change Seed Pin Value");

					FVoxelPinValue Value = FVoxelPinValue::Make<FVoxelExposedSeed>();
					Value.Get<FVoxelExposedSeed>().Seed = NewValue.ToString();
					GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, Value.ExportToString());
				})
				.ForegroundColor(FSlateColor::UseForeground())
			]
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(4.f, 1.f)
		[
			SNew(SBox)
			.WidthOverride(16.f)
			.HeightOverride(16.f)
			.Visibility(this, &SGraphPin::GetDefaultValueVisibility)
			[
				SNew(SButton)
				.ButtonStyle(FVoxelEditorStyle::Get(), "Graph.Seed.Dice")
				.OnClicked_Lambda([&]
				{
					if (!ensure(!GraphPinObj->IsPendingKill()))
					{
						return FReply::Handled();
					}

					const FVoxelTransaction Transaction(GraphPinObj, "Randomize Seed Pin Value");

					FVoxelPinValue Value = FVoxelPinValue::Make<FVoxelExposedSeed>();
					Value.Get<FVoxelExposedSeed>().Randomize();
					GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, Value.ExportToString());

					return FReply::Handled();
				})
			]
		];
}