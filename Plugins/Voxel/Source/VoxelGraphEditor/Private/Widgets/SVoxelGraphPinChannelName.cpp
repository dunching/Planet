// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "SVoxelGraphPinChannelName.h"
#include "VoxelChannel.h"
#include "VoxelGraphVisuals.h"
#include "Widgets/SVoxelChannelEditor.h"

void SVoxelGraphPinChannelName::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
{
	SGraphPin::Construct(SGraphPin::FArguments(), InGraphPinObj);
}

TSharedRef<SWidget>	SVoxelGraphPinChannelName::GetDefaultValueWidget()
{
	const FVoxelPinValue DefaultValue = FVoxelPinValue::MakeFromPinDefaultValue(*GraphPinObj);
	const FName CurrentName = ensure(DefaultValue.Is<FVoxelChannelName>())
		? DefaultValue.Get<FVoxelChannelName>().Name
		: "INVALID";

	TOptional<FVoxelChannelDefinition> ChannelDef = GVoxelChannelManager->FindChannelDefinition(CurrentName);

	TSharedPtr<SWidget> ImageWidget = SNullWidget::NullWidget;
	if (ChannelDef.IsSet() &&
		ChannelDef->Type.IsValid())
	{
		ImageWidget =
			SNew(SImage)
			.Visibility(ChannelDef->Type.IsValid() ? EVisibility::Visible : EVisibility::Collapsed)
			.Image(FVoxelGraphVisuals::GetPinIcon(ChannelDef->Type).GetIcon())
			.ColorAndOpacity(FVoxelGraphVisuals::GetPinColor(ChannelDef->Type));
	}

	return
		SNew(SComboButton)
		.Visibility(this, &SGraphPin::GetDefaultValueVisibility)
		.ComboButtonStyle(FAppStyle::Get(), "ComboButton")
		.OnGetMenuContent_Lambda([=]
		{
			return
				SNew(SVoxelChannelEditor)
				.SelectedChannel(CurrentName)
				.OnChannelSelected_Lambda(MakeWeakPtrLambda(this, [this](const FName NewChannel)
				{
					if (!ensure(!GraphPinObj->IsPendingKill()))
					{
						return;
					}

					const FVoxelTransaction Transaction(GraphPinObj, "Set Channel Name");

					FVoxelPinValue Value = FVoxelPinValue::Make<FVoxelChannelName>();
					Value.Get<FVoxelChannelName>().Name = NewChannel;
					GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, Value.ExportToString());
				}));
		})
		.ContentPadding(0)
		.ForegroundColor(FSlateColor::UseForeground())
		.ButtonContent()
		[
			SNew(SHorizontalBox)
			.Clipping(EWidgetClipping::ClipToBoundsAlways)
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Left)
			.Padding(0.f, 0.f, 2.f, 0.f)
			.AutoWidth()
			[
				ImageWidget.ToSharedRef()
			]
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Left)
			.Padding(2.f, 0.f, 0.f, 0.f)
			.AutoWidth()
			[
				SNew(STextBlock)
				.Font(FVoxelEditorUtilities::Font())
				.Text(FText::FromName(CurrentName))
			]
		];
}