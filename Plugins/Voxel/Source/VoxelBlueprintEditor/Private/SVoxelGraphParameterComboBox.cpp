// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "SVoxelGraphParameterComboBox.h"
#include "SVoxelGraphParameterSelector.h"
#include "VoxelGraphVisuals.h"

void SVoxelGraphParameterComboBox::Construct(const FArguments& InArgs)
{
	CachedParameterProvider = InArgs._ParameterProvider;
	OnParameterChanged = InArgs._OnTypeChanged;
	ensure(OnParameterChanged.IsBound());

	SAssignNew(MainIcon, SImage);

	SAssignNew(MainTextBlock, STextBlock)
	.Font(FVoxelEditorUtilities::Font())
	.ColorAndOpacity(FSlateColor::UseForeground());

	UpdateParameter(InArgs._CurrentParameter);

	TSharedPtr<SHorizontalBox> SelectorBox;
	ChildSlot
	[
		SNew(SBox)
		.Padding(-6.f, 0.f,0.f,0.f)
		.Clipping(EWidgetClipping::ClipToBoundsAlways)
		.HAlign(HAlign_Left)
		[
			SAssignNew(TypeComboButton, SComboButton)
			.ComboButtonStyle(FAppStyle::Get(), "ComboButton")
			.OnGetMenuContent(this, &SVoxelGraphParameterComboBox::GetMenuContent)
			.ContentPadding(0)
			.ForegroundColor_Lambda([this]
			{
				return bIsValidParameter ? FSlateColor::UseForeground() : FStyleColors::Error;
			})
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
					MainIcon.ToSharedRef()
				]
				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Left)
				.Padding(2.f, 0.f, 0.f, 0.f)
				.AutoWidth()
				[
					MainTextBlock.ToSharedRef()
				]
			]
		]
	];
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void SVoxelGraphParameterComboBox::UpdateParameter(const FVoxelGraphBlueprintParameter& NewParameter)
{
	bIsValidParameter = NewParameter.bIsValid;
	MainIcon->SetImage(GetIcon(NewParameter.Type));
	MainIcon->SetColorAndOpacity(GetColor(NewParameter.Type));

	MainTextBlock->SetText(FText::FromName(NewParameter.Name));
}

void SVoxelGraphParameterComboBox::UpdateParameterProvider(const TWeakInterfacePtr<IVoxelParameterProvider>& NewParameterProvider)
{
	CachedParameterProvider = NewParameterProvider;
	if (!ParameterSelector)
	{
		return;
	}

	ParameterSelector->UpdateParameterProvider(NewParameterProvider);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedRef<SWidget> SVoxelGraphParameterComboBox::GetMenuContent()
{
	if (MenuContent)
	{
		ParameterSelector->ClearSelection();

		return MenuContent.ToSharedRef();
	}

	SAssignNew(MenuContent, SMenuOwner)
	[
		SAssignNew(ParameterSelector, SVoxelGraphParameterSelector)
		.ParameterProvider(CachedParameterProvider)
		.OnParameterChanged(MakeWeakPtrDelegate(this, [this](const FVoxelParameter& NewParameter)
		{
			OnParameterChanged.ExecuteIfBound(NewParameter);
			UpdateParameter(FVoxelGraphBlueprintParameter(NewParameter));
		}))
		.OnCloseMenu(MakeWeakPtrDelegate(this, [this]
		{
			MenuContent->CloseSummonedMenus();
			TypeComboButton->SetIsOpen(false);
		}))
	];

	TypeComboButton->SetMenuContentWidgetToFocus(ParameterSelector->GetWidgetToFocus());

	return MenuContent.ToSharedRef();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

const FSlateBrush* SVoxelGraphParameterComboBox::GetIcon(const FVoxelPinType& PinType) const
{
	return FVoxelGraphVisuals::GetPinIcon(PinType).GetIcon();
}

FLinearColor SVoxelGraphParameterComboBox::GetColor(const FVoxelPinType& PinType) const
{
	return FVoxelGraphVisuals::GetPinColor(PinType);
}