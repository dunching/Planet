// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "SVoxelGraphPinParameter.h"
#include "SVoxelGraphParameterComboBox.h"
#include "K2Node_VoxelGraphParameterBase.h"

void SVoxelGraphPinParameter::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
{
	if (const UEdGraphNode* Node = InGraphPinObj->GetOwningNode())
	{
		if (UEdGraphPin* AssetPin = Node->FindPin(UK2Node_VoxelGraphParameterBase::AssetPinName))
		{
			ensure(!AssetPin->DefaultObject || AssetPin->DefaultObject->Implements<UVoxelParameterProvider>());

			WeakParameterProvider = MakeWeakInterfacePtr<IVoxelParameterProvider>(AssetPin->DefaultObject);
			AssetPinReference = AssetPin;
		}
	}

	SGraphPin::Construct(SGraphPin::FArguments(), InGraphPinObj);
}

void SVoxelGraphPinParameter::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SGraphPin::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	if (bGraphDataInvalid)
	{
		AssetPinReference = {};
		WeakParameterProvider = nullptr;
		return;
	}

	if (const UEdGraphPin* Pin = AssetPinReference.Get())
	{
		if (WeakParameterProvider.GetObject() == Pin->DefaultObject)
		{
			return;
		}

		if (Pin->DefaultObject &&
			!Pin->DefaultObject->Implements<UVoxelParameterProvider>())
		{
			return;
		}

		UpdateParameterProvider(Pin->DefaultObject);
	}
}

TSharedRef<SWidget>	SVoxelGraphPinParameter::GetDefaultValueWidget()
{
	return
		SNew(SBox)
		.MinDesiredWidth(18)
		.MaxDesiredWidth(400)
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			[
				SAssignNew(TextContainer, SBox)
				.Visibility_Lambda([this]
				{
					if (!WeakParameterProvider.IsValid())
					{
						return GetDefaultValueVisibility();
					}
					return EVisibility::Collapsed;
				})
				[
					SNew(SEditableTextBox)
					.Style(FAppStyle::Get(), "Graph.EditableTextBox")
					.Text_Lambda([this]
					{
						return FText::FromString(GraphPinObj->DefaultValue);
					})
					.SelectAllTextWhenFocused(true)
					.IsReadOnly_Lambda([this]() -> bool
					{
						return GraphPinObj->bDefaultValueIsReadOnly;
					})
					.OnTextCommitted_Lambda([this](const FText& NewValue, ETextCommit::Type)
					{
						if (!ensure(!GraphPinObj->IsPendingKill()))
						{
							return;
						}

						FString NewName = NewValue.ToString();
						if (NewName.IsEmpty())
						{
							NewName = "None";
						}

						const FVoxelTransaction Transaction(GraphPinObj, "Change Parameter Pin Value");
						GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, *NewName);
					})
					.ForegroundColor(FSlateColor::UseForeground())
				]
			]
			+ SOverlay::Slot()
			[
				SAssignNew(ParameterSelectorContainer, SBox)
				.Visibility_Lambda([this]
				{
					if (WeakParameterProvider.IsValid())
					{
						return GetDefaultValueVisibility();
					}
					return EVisibility::Collapsed;
				})
				[
					SAssignNew(ParameterComboBox, SVoxelGraphParameterComboBox)
					.ParameterProvider(WeakParameterProvider)
					.CurrentParameter(GetCachedParameter())
					.OnTypeChanged_Lambda([this](const FVoxelParameter NewParameter)
					{
						if (!ensure(!GraphPinObj->IsPendingKill()) ||
							!GraphPinObj->GetOwningNode())
						{
							return;
						}

						const FVoxelTransaction Transaction(GraphPinObj, "Change Parameter Pin Value");
						GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, NewParameter.Guid.ToString());
					})
				]
			]
		];
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void SVoxelGraphPinParameter::UpdateParameterProvider(const TWeakInterfacePtr<IVoxelParameterProvider>& NewParameterProvider)
{
	WeakParameterProvider = NewParameterProvider;

	if (ParameterComboBox)
	{
		ParameterComboBox->UpdateParameterProvider(NewParameterProvider);
		ParameterComboBox->UpdateParameter(GetCachedParameter());
	}
}

FVoxelGraphBlueprintParameter SVoxelGraphPinParameter::GetCachedParameter() const
{
	return Cast<UK2Node_VoxelGraphParameterBase>(GraphPinObj->GetOwningNode())->CachedParameter;
}