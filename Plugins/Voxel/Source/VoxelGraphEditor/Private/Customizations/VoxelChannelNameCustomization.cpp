// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelEditorMinimal.h"
#include "VoxelChannel.h"
#include "VoxelGraphVisuals.h"
#include "Widgets/SVoxelChannelEditor.h"

class FVoxelChannelNameCustomization
	: public IPropertyTypeCustomization
	, public FVoxelTicker
{
public:
	virtual void CustomizeHeader(
		const TSharedRef<IPropertyHandle> PropertyHandle,
		FDetailWidgetRow& HeaderRow,
		IPropertyTypeCustomizationUtils& CustomizationUtils) override
	{
		NameHandle = PropertyHandle->GetChildHandleStatic(FVoxelChannelName, Name);

		FName Channel;
		switch (NameHandle->GetValue(Channel))
		{
		default:
		{
			ensure(false);
			return;
		}
		case FPropertyAccess::MultipleValues:
		{
			HeaderRow
			.NameContent()
			[
				PropertyHandle->CreatePropertyNameWidget()
			]
			.ValueContent()
			[
				NameHandle->CreatePropertyValueWidget()
			];
			return;
		}
		case FPropertyAccess::Fail:
		{
			ensure(false);
			return;
		}
		case FPropertyAccess::Success: break;
		}

		CachedChannel = Channel;
		TOptional<FVoxelChannelDefinition> ChannelDef = GVoxelChannelManager->FindChannelDefinition(Channel);

		PreviewImageWidget =
			SNew(SImage)
			.Visibility(ChannelDef.IsSet() && ChannelDef->Type.IsValid() ? EVisibility::Visible : EVisibility::Collapsed);

		if (ChannelDef.IsSet() &&
			ChannelDef->Type.IsValid())
		{
			PreviewImageWidget->SetImage(FVoxelGraphVisuals::GetPinIcon(ChannelDef->Type).GetIcon());
			PreviewImageWidget->SetColorAndOpacity(FVoxelGraphVisuals::GetPinColor(ChannelDef->Type));
		}

		HeaderRow
		.NameContent()
		[
			PropertyHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		[
			SNew(SBox)
			.MinDesiredWidth(125.f)
			[
				SAssignNew(SelectedComboButton, SComboButton)
				.ComboButtonStyle(FAppStyle::Get(), "ComboButton")
				.OnGetMenuContent(this, &FVoxelChannelNameCustomization::GetMenuContent)
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
						PreviewImageWidget.ToSharedRef()
					]
					+ SHorizontalBox::Slot()
					.VAlign(VAlign_Center)
					.HAlign(HAlign_Left)
					.Padding(2.f, 0.f, 0.f, 0.f)
					.AutoWidth()
					[
						SAssignNew(PreviewTextWidget, STextBlock)
						.Font(FVoxelEditorUtilities::Font())
						.Text(FText::FromName(Channel))
					]
				]
			]
		];
	}

	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override
	{
	}

	virtual void Tick() override
	{
		if (!NameHandle ||
			!SelectedComboButton)
		{
			return;
		}

		FName Channel;
		ensure(NameHandle->GetValue(Channel) == FPropertyAccess::Success);
		if (CachedChannel == Channel)
		{
			return;
		}

		CachedChannel = Channel;
		UpdateButtonContents();
	}

private:
	TSharedRef<SWidget> GetMenuContent()
	{
		if (!ensure(NameHandle))
		{
			return SNullWidget::NullWidget;
		}

		FName Channel;
		NameHandle->GetValue(Channel);

		return
			SAssignNew(MenuContent, SMenuOwner)
			[
				SNew(SVoxelChannelEditor)
				.SelectedChannel(Channel)
				.OnChannelSelected_Lambda(MakeWeakPtrLambda(this, [=](const FName NewChannel)
				{
					CachedChannel = NewChannel;
					if (!ensure(NameHandle))
					{
						return;
					}

					NameHandle->SetValue(NewChannel);

					UpdateButtonContents();

					if (MenuContent &&
						SelectedComboButton)
					{
						MenuContent->CloseSummonedMenus();
						SelectedComboButton->SetIsOpen(false);
					}
				}))
			];
	}

	void UpdateButtonContents() const
	{
		if (!ensure(PreviewTextWidget) ||
			!ensure(PreviewImageWidget))
		{
			return;
		}

		PreviewTextWidget->SetText(FText::FromName(CachedChannel));

		TOptional<FVoxelChannelDefinition> ChannelDef = GVoxelChannelManager->FindChannelDefinition(CachedChannel);
		if (ChannelDef.IsSet() &&
			ChannelDef->Type.IsValid())
		{
			PreviewImageWidget->SetImage(FVoxelGraphVisuals::GetPinIcon(ChannelDef->Type).GetIcon());
			PreviewImageWidget->SetColorAndOpacity(FVoxelGraphVisuals::GetPinColor(ChannelDef->Type));
			PreviewImageWidget->SetVisibility(EVisibility::Visible);
		}
		else
		{
			PreviewImageWidget->SetVisibility(EVisibility::Collapsed);
		}
	}

private:
	TSharedPtr<IPropertyHandle> NameHandle;
	TSharedPtr<STextBlock> PreviewTextWidget;
	TSharedPtr<SImage> PreviewImageWidget;
	TSharedPtr<SMenuOwner> MenuContent;
	TSharedPtr<SComboButton> SelectedComboButton;
	FName CachedChannel;
};

DEFINE_VOXEL_STRUCT_LAYOUT(FVoxelChannelName, FVoxelChannelNameCustomization);