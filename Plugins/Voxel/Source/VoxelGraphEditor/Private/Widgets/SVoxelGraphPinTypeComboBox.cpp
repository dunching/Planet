// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Widgets/SVoxelGraphPinTypeComboBox.h"
#include "VoxelGraphVisuals.h"
#include "Widgets/SVoxelGraphPinTypeSelector.h"

void SVoxelPinTypeComboBox::Construct(const FArguments& InArgs)
{
	AllowedTypes = InArgs._AllowedTypes;
	CurrentType = InArgs._CurrentType;
	ReadOnly = InArgs._ReadOnly;

	OnTypeChanged = InArgs._OnTypeChanged;
	ensure(OnTypeChanged.IsBound());

	SAssignNew(MainIcon, SImage);

	SAssignNew(MainTextBlock, STextBlock)
	.Font(FVoxelEditorUtilities::Font())
	.ColorAndOpacity(FSlateColor::UseForeground());

	UpdateType(CurrentType.Get());

	bAllowUniforms = false;
	bAllowBuffers = false;
	bAllowBufferArrays = false;
	for (const FVoxelPinType& Type : AllowedTypes.Get().GetTypes())
	{
		if (Type.IsBuffer())
		{
			if (Type.IsBufferArray())
			{
				bAllowBufferArrays = true;
			}
			else
			{
				bAllowBuffers = true;
			}
		}
		else
		{
			bAllowUniforms = true;
		}

		if (bAllowUniforms &&
			bAllowBuffers &&
			bAllowBufferArrays)
		{
			break;
		}
	}

	const bool bShowContainerSelection = (bAllowUniforms + bAllowBuffers + bAllowBufferArrays) > 1;

	TSharedPtr<SHorizontalBox> SelectorBox;
	ChildSlot
	[
		SNew(SWidgetSwitcher)
		.WidgetIndex_Lambda([this]
		{
			return ReadOnly.Get() ? 1 : 0;
		})
		+ SWidgetSwitcher::Slot()
		.Padding(InArgs._DetailsWindow ? FMargin(0.f) : FMargin(-6.f, 0.f,0.f,0.f))
		[
			SAssignNew(SelectorBox, SHorizontalBox)
			.Clipping(EWidgetClipping::ClipToBoundsAlways)
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			[
				SNew(SBox)
				.WidthOverride(InArgs._DetailsWindow ? 125.f : FOptionalSize())
				[
					SAssignNew(TypeComboButton, SComboButton)
					.ComboButtonStyle(FAppStyle::Get(), "ComboButton")
					.OnGetMenuContent(this, &SVoxelPinTypeComboBox::GetMenuContent)
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
			]
		]
		+ SWidgetSwitcher::Slot()
		[
			SNew(SHorizontalBox)
			.Clipping(EWidgetClipping::OnDemand)
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Left)
			.Padding(2.f, bShowContainerSelection ? 4.f : 3.f)
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
	];

	if (!bShowContainerSelection)
	{
		return;
	}

	SelectorBox->AddSlot()
	.AutoWidth()
	.VAlign(VAlign_Center)
	.HAlign(HAlign_Center)
	.Padding(2.f)
	[
		SAssignNew(ContainerTypeComboButton, SComboButton)
		.ComboButtonStyle(FAppStyle::Get(),"BlueprintEditor.CompactVariableTypeSelector")
		.MenuPlacement(MenuPlacement_ComboBoxRight)
		.OnGetMenuContent(this, &SVoxelPinTypeComboBox::GetPinContainerTypeMenuContent)
		.ContentPadding(0.f)
		.ButtonContent()
		[
			MainIcon.ToSharedRef()
		]
	];
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void SVoxelPinTypeComboBox::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	if (!CurrentType.IsBound())
	{
		return;
	}

	const FVoxelPinType NewType = CurrentType.Get();
	if (NewType == CachedType)
	{
		return;
	}

	UpdateType(NewType);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedRef<SWidget> SVoxelPinTypeComboBox::GetMenuContent()
{
	if (MenuContent)
	{
		PinTypeSelector->ClearSelection();

		return MenuContent.ToSharedRef();
	}

	SAssignNew(MenuContent, SMenuOwner)
	[
		SAssignNew(PinTypeSelector, SVoxelPinTypeSelector)
		.AllowedTypes(AllowedTypes)
		.OnTypeChanged_Lambda([this](FVoxelPinType NewType)
		{
			if (CachedType.IsBuffer())
			{
				NewType = NewType.GetBufferType();
			}
			OnTypeChanged.ExecuteIfBound(NewType);
			UpdateType(NewType);
		})
		.OnCloseMenu_Lambda([this]
		{
			MenuContent->CloseSummonedMenus();
			TypeComboButton->SetIsOpen(false);
		})
	];

	TypeComboButton->SetMenuContentWidgetToFocus(PinTypeSelector->GetWidgetToFocus());

	return MenuContent.ToSharedRef();
}

TSharedRef<SWidget> SVoxelPinTypeComboBox::GetPinContainerTypeMenuContent()
{
	if (ContainerTypeMenuContent)
	{
		return ContainerTypeMenuContent.ToSharedRef();
	}

	struct FData
	{
		FText Label;
		const FSlateBrush* Brush;
	};

	static const TMap<EVoxelPinContainerType, FData> Containers
	{
		{
			EVoxelPinContainerType::None,
			{
				INVTEXT("Uniform"),
				FAppStyle::Get().GetBrush("Kismet.VariableList.TypeIcon")
			}
		},
		{
			EVoxelPinContainerType::Buffer,
			{
				INVTEXT("Buffer"),
				FVoxelEditorStyle::GetBrush("Pill.Buffer")
			}
		},
	};

	FMenuBuilder MenuBuilder(false, nullptr);

	const auto AddMenu = [&](const EVoxelPinContainerType Type, const FText& Label, const FSlateBrush* Brush)
	{
		FUIAction Action;
		Action.ExecuteAction.BindSP(this, &SVoxelPinTypeComboBox::OnContainerTypeSelectionChanged, Type);

		MenuBuilder.AddMenuEntry(Action,
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(SImage)
				.Image(Brush)
				.ColorAndOpacity(GetColor(CachedInnerType))
			]
			+ SHorizontalBox::Slot()
			.Padding(4.0f,2.0f)
			[
				SNew(STextBlock)
				.Text(Label)
			]);
	};

	if (bAllowUniforms)
	{
		AddMenu(EVoxelPinContainerType::None, INVTEXT("Uniform"), FAppStyle::Get().GetBrush("Kismet.VariableList.TypeIcon"));
	}
	if (bAllowBuffers)
	{
		AddMenu(EVoxelPinContainerType::Buffer, INVTEXT("Buffer"), FVoxelEditorStyle::Get().GetBrush("Pill.Buffer"));
	}
	if (bAllowBufferArrays)
	{
		AddMenu(EVoxelPinContainerType::BufferArray, INVTEXT("Array"), FAppStyle::Get().GetBrush("Kismet.VariableList.ArrayTypeIcon"));
	}

	SAssignNew(ContainerTypeMenuContent, SMenuOwner)
	[
		MenuBuilder.MakeWidget()
	];

	return ContainerTypeMenuContent.ToSharedRef();
}

void SVoxelPinTypeComboBox::OnContainerTypeSelectionChanged(const EVoxelPinContainerType ContainerType) const
{
	switch (ContainerType)
	{
	default: check(false);
	case EVoxelPinContainerType::None:
	{
		OnTypeChanged.ExecuteIfBound(CachedType.GetInnerType());
	}
	break;
	case EVoxelPinContainerType::Buffer:
	{
		OnTypeChanged.ExecuteIfBound(CachedType.GetBufferType().WithBufferArray(false));
	}
	break;
	case EVoxelPinContainerType::BufferArray:
	{
		OnTypeChanged.ExecuteIfBound(CachedType.GetBufferType().WithBufferArray(true));
	}
	break;
	}

	ContainerTypeMenuContent->CloseSummonedMenus();
	ContainerTypeComboButton->SetIsOpen(false);
}

void SVoxelPinTypeComboBox::UpdateType(const FVoxelPinType& NewType)
{
	CachedType = NewType;
	CachedInnerType = NewType.GetInnerType();

	MainIcon->SetImage(GetIcon(CachedType));
	MainIcon->SetColorAndOpacity(GetColor(CachedInnerType));

	MainTextBlock->SetText(FText::FromString(CachedInnerType.ToString()));

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

const FSlateBrush* SVoxelPinTypeComboBox::GetIcon(const FVoxelPinType& PinType) const
{
	return FVoxelGraphVisuals::GetPinIcon(PinType).GetIcon();
}

FLinearColor SVoxelPinTypeComboBox::GetColor(const FVoxelPinType& PinType) const
{
	return FVoxelGraphVisuals::GetPinColor(PinType);
}