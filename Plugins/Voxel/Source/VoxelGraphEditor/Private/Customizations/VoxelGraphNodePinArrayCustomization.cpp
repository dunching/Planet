// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelGraphNodePinArrayCustomization.h"
#include "Nodes/VoxelGraphStructNode.h"
#include "VoxelGraphNodeCustomization.h"
#include "Customizations/VoxelPinValueCustomizationHelper.h"

void FVoxelGraphNodePinArrayCustomization::GenerateHeaderRowContent(FDetailWidgetRow& NodeRow)
{
	const TSharedRef<SWidget> AddButton = PropertyCustomizationHelpers::MakeAddButton(
		FSimpleDelegate::CreateSP(this, &FVoxelGraphNodePinArrayCustomization::AddNewPin),
		INVTEXT("Add Element"),
		MakeAttributeSP(this, &FVoxelGraphNodePinArrayCustomization::CanAddNewPin));

	const TSharedRef<SWidget> ClearButton = PropertyCustomizationHelpers::MakeEmptyButton(
		FSimpleDelegate::CreateSP(this, &FVoxelGraphNodePinArrayCustomization::ClearAllPins),
		INVTEXT("Removes All Elements"),
		MakeAttributeSP(this, &FVoxelGraphNodePinArrayCustomization::CanRemovePin));

	NodeRow
	.NameContent()
	[
		SNew(SVoxelDetailText)
		.Text(FText::FromString(FName::NameToDisplayString(PinName, false)))
	]
	.ValueContent()
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.AutoWidth()
		[
			SNew(SVoxelDetailText)
			.Text(FText::FromString(LexToString(Properties.Num()) + " Array elements"))
		]
		+ SHorizontalBox::Slot()
		.Padding(2.0f)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.AutoWidth()
		[
			AddButton
		]
		+ SHorizontalBox::Slot()
		.Padding(2.0f)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.AutoWidth()
		[
			ClearButton
		]
	];
}

void FVoxelGraphNodePinArrayCustomization::GenerateChildContent(IDetailChildrenBuilder& ChildrenBuilder)
{
	const TSharedPtr<FVoxelGraphNodeCustomization> Customization = WeakCustomization.Pin();
	if (!ensure(Customization))
	{
		return;
	}

	int32 Index = 0;
	for (const TSharedPtr<IPropertyHandle>& Handle : Properties)
	{
		FName ElementPinName;
		Handle->GetChildHandleStatic(FVoxelNodeExposedPinValue, Name)->GetValue(ElementPinName);
		Handle->SetPropertyDisplayName(FText::FromString(LexToString(Index++)));

		const auto SetupRowLambda = [this, Handle, Customization, WeakNode = WeakStructNode, ElementPinName](FDetailWidgetRow& Row, const TSharedRef<SWidget>& ValueWidget)
		{
			Row
			.NameContent()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				.Padding(0, 0, 3, 0)
				.AutoWidth()
				[
					SNew(SVoxelDetailText)
					.Text(INVTEXT("Index"))
					.ColorAndOpacity(FSlateColor::UseSubduedForeground())
				]
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				.Padding(0, 0, 3, 0)
				.AutoWidth()
				[
					SNew(SVoxelDetailText)
					.Text(INVTEXT("["))
					.ColorAndOpacity(FSlateColor::UseSubduedForeground())
				]
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				.Padding(0, 0, 3, 0)
				.AutoWidth()
				[
					Handle->CreatePropertyNameWidget()
				]
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				.AutoWidth()
				[
					SNew(SVoxelDetailText)
					.Text(INVTEXT("]"))
					.ColorAndOpacity(FSlateColor::UseSubduedForeground())
				]
			]
			.ValueContent()
			.HAlign(HAlign_Fill)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.VAlign(VAlign_Center)
					[
						SNew(SBox)
						.Visibility_Lambda([WeakNode, ElementPinName]
						{
							const UVoxelGraphStructNode* TargetNode = WeakNode.Get();
							if (!ensure(TargetNode))
							{
								return EVisibility::Visible;
							}

							return TargetNode->Struct->ExposedPins.Contains(ElementPinName) ? EVisibility::Collapsed : EVisibility::Visible;
						})
						.MinDesiredWidth(125.f)
						[
							ValueWidget
						]
					]
					+ SHorizontalBox::Slot()
					.VAlign(VAlign_Center)
					[
						SNew(SBox)
						.Visibility_Lambda([WeakNode, ElementPinName]
						{
							const UVoxelGraphStructNode* TargetNode = WeakNode.Get();
							if (!ensure(TargetNode))
							{
								return EVisibility::Collapsed;
							}

							return TargetNode->Struct->ExposedPins.Contains(ElementPinName) ? EVisibility::Visible : EVisibility::Collapsed;
						})
						.MinDesiredWidth(125.f)
						[
							SNew(SVoxelDetailText)
							.Text(INVTEXT("Pin is exposed"))
							.ColorAndOpacity(FSlateColor::UseSubduedForeground())
						]
					]
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				.Padding(4.0f, 1.0f, 0.0f, 1.0f)
				[
					CreateElementEditButton(ElementPinName)
				]
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Center)
				.FillWidth(1.f)
				[
					Customization->CreateExposePinButton(WeakNode, ElementPinName)
				]
			];
		};

		const TSharedPtr<FVoxelStructCustomizationWrapper> Wrapper = FVoxelPinValueCustomizationHelper::CreatePinValueCustomization(
			Handle->GetChildHandleStatic(FVoxelNodeExposedPinValue, Value),
			ChildrenBuilder,
			{},
			SetupRowLambda,
			{});

		if (Wrapper)
		{
			Customization->Wrappers.Add(Wrapper);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelGraphNodePinArrayCustomization::AddNewPin()
{
	UVoxelGraphStructNode* Node = WeakStructNode.Get();
	if (!Node)
	{
		return;
	}

	if (!Node->GetNodeDefinition()->CanAddToCategory(FName(PinName)))
	{
		return;
	}

	const FVoxelTransaction Transaction(Node, "Add Pin to Array");
	Node->GetNodeDefinition()->AddToCategory(FName(PinName));
	Node->ReconstructNode(false);
}

bool FVoxelGraphNodePinArrayCustomization::CanAddNewPin() const
{
	UVoxelGraphStructNode* Node = WeakStructNode.Get();
	if (!Node)
	{
		return false;
	}

	return Node->GetNodeDefinition()->CanAddToCategory(FName(PinName));
}

void FVoxelGraphNodePinArrayCustomization::ClearAllPins()
{
	UVoxelGraphStructNode* Node = WeakStructNode.Get();
	if (!Node)
	{
		return;
	}

	const FVoxelTransaction Transaction(Node, "Clear Pins Array");
	for (int32 Index = 0; Index < Properties.Num(); Index++)
	{
		if (!Node->GetNodeDefinition()->CanRemoveFromCategory(FName(PinName)))
		{
			break;
		}

		Node->GetNodeDefinition()->RemoveFromCategory(FName(PinName));
	}
	Node->ReconstructNode(false);
}

bool FVoxelGraphNodePinArrayCustomization::CanRemovePin() const
{
	UVoxelGraphStructNode* Node = WeakStructNode.Get();
	if (!Node)
	{
		return false;
	}

	return Node->GetNodeDefinition()->CanRemoveFromCategory(FName(PinName));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedRef<SWidget> FVoxelGraphNodePinArrayCustomization::CreateElementEditButton(const FName EntryPinName)
{
	FMenuBuilder MenuContentBuilder( true, nullptr, nullptr, true );
	{
		FUIAction InsertAction(
			FExecuteAction::CreateSP(this, &FVoxelGraphNodePinArrayCustomization::InsertPinBefore, EntryPinName));
		MenuContentBuilder.AddMenuEntry(INVTEXT("Insert"), {}, FSlateIcon(), InsertAction);

		FUIAction DeleteAction(
			FExecuteAction::CreateSP(this, &FVoxelGraphNodePinArrayCustomization::DeletePin, EntryPinName),
			FCanExecuteAction::CreateSP(this, &FVoxelGraphNodePinArrayCustomization::CanDeletePin, EntryPinName));
		MenuContentBuilder.AddMenuEntry(INVTEXT("Delete"), {}, FSlateIcon(), DeleteAction);

		FUIAction DuplicateAction(FExecuteAction::CreateSP(this, &FVoxelGraphNodePinArrayCustomization::DuplicatePin, EntryPinName));
		MenuContentBuilder.AddMenuEntry( INVTEXT("Duplicate"), {}, FSlateIcon(), DuplicateAction );
	}

	return
		SNew(SComboButton)
		.ComboButtonStyle( FAppStyle::Get(), "SimpleComboButton" )
		.ContentPadding(2)
		.ForegroundColor( FSlateColor::UseForeground() )
		.HasDownArrow(true)
		.MenuContent()
		[
			MenuContentBuilder.MakeWidget()
		];
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelGraphNodePinArrayCustomization::InsertPinBefore(const FName EntryPinName)
{
	const TSharedPtr<IVoxelNodeDefinition> NodeDefinition = WeakNodeDefinition.Pin();
	if (!NodeDefinition)
	{
		return;
	}

	NodeDefinition->InsertPinBefore(EntryPinName);
}

void FVoxelGraphNodePinArrayCustomization::DeletePin(const FName EntryPinName)
{
	const TSharedPtr<IVoxelNodeDefinition> NodeDefinition = WeakNodeDefinition.Pin();
	if (!NodeDefinition)
	{
		return;
	}

	if (!NodeDefinition->CanRemoveSelectedPin(EntryPinName))
	{
		return;
	}

	NodeDefinition->RemoveSelectedPin(EntryPinName);
}

bool FVoxelGraphNodePinArrayCustomization::CanDeletePin(const FName EntryPinName)
{
	const TSharedPtr<IVoxelNodeDefinition> NodeDefinition = WeakNodeDefinition.Pin();
	if (!NodeDefinition)
	{
		return false;
	}

	return NodeDefinition->CanRemoveSelectedPin(EntryPinName);
}

void FVoxelGraphNodePinArrayCustomization::DuplicatePin(const FName EntryPinName)
{
	const TSharedPtr<IVoxelNodeDefinition> NodeDefinition = WeakNodeDefinition.Pin();
	if (!NodeDefinition)
	{
		return;
	}

	NodeDefinition->DuplicatePin(EntryPinName);
}