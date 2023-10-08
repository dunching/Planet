// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelGraphNodeCustomization.h"
#include "VoxelGraph.h"
#include "Nodes/VoxelGraphStructNode.h"
#include "VoxelGraphNodePinArrayCustomization.h"
#include "Customizations/VoxelPinValueCustomizationHelper.h"

void FVoxelGraphNodeCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
	const TArray<TWeakObjectPtr<UObject>> SelectedObjects = DetailLayout.GetSelectedObjects();

	if (SelectedObjects.Num() != 1)
	{
		return;
	}

	UVoxelGraphStructNode* Node = Cast<UVoxelGraphStructNode>(SelectedObjects[0]);
	if (!ensureVoxelSlow(Node) ||
		!ensureVoxelSlow(Node->Struct))
	{
		return;
	}

	const TSharedRef<IPropertyHandle> StructHandle = DetailLayout.GetProperty(GET_MEMBER_NAME_STATIC(UVoxelGraphStructNode, Struct), Node->GetClass());

	NodeDefinition = Node->GetNodeDefinition();

	Node->Struct->OnExposedPinsUpdated.Add(MakeWeakPtrDelegate(this, [RefreshDelegate = FVoxelEditorUtilities::MakeRefreshDelegate(StructHandle, DetailLayout)]
	{
		RefreshDelegate.ExecuteIfBound();
	}));

	DetailLayout.GetProperty(GET_MEMBER_NAME_STATIC(UVoxelGraphNodeBase, PreviewedPin), UVoxelGraphNodeBase::StaticClass())->MarkHiddenByCustomization();
	DetailLayout.GetProperty(GET_MEMBER_NAME_STATIC(UVoxelGraphNodeBase, PreviewSettings), UVoxelGraphNodeBase::StaticClass())->MarkHiddenByCustomization();

	const TMap<FName, TSharedPtr<IPropertyHandle>> ChildrenHandles = InitializeStructChildren(StructHandle);
	if (ChildrenHandles.Num() == 0)
	{
		return;
	}

	DetailLayout.HideProperty(StructHandle);

	for (const auto& It : ChildrenHandles)
	{
		if (It.Key == GET_MEMBER_NAME_STATIC(FVoxelNode, ExposedPinValues))
		{
			continue;
		}

		DetailLayout.AddPropertyToCategory(It.Value);
	}

	const TSharedPtr<IPropertyHandle> ExposedPinValuesHandle = ChildrenHandles.FindRef(GET_MEMBER_NAME_STATIC(FVoxelNode, ExposedPinValues));
	if (!ensure(ExposedPinValuesHandle))
	{
		return;
	}

	IDetailCategoryBuilder& DefaultCategoryBuilder = DetailLayout.EditCategory("Default", INVTEXT("Default"));

	TMap<FName, TSharedPtr<IPropertyHandle>> MappedHandles;

	uint32 NumValues = 0;
	ExposedPinValuesHandle->GetNumChildren(NumValues);

	for (uint32 Index = 0; Index < NumValues; Index++)
	{
		const TSharedPtr<IPropertyHandle> ValueHandle = ExposedPinValuesHandle->GetChildHandle(Index);
		if (!ensure(ValueHandle))
		{
			continue;
		}

		FName PinName;
		ValueHandle->GetChildHandleStatic(FVoxelNodeExposedPinValue, Name)->GetValue(PinName);

		if (!ensure(!PinName.IsNone()))
		{
			continue;
		}

		MappedHandles.Add(PinName, ValueHandle);
	}

	for (const FName PinName : Node->Struct->InternalPinsOrder)
	{
		if (TSharedPtr<FVoxelNode::FPinArray> PinArray = Node->Struct->InternalPinArrays.FindRef(PinName))
		{
			if (!PinArray->PinTemplate.Metadata.bShowInDetail)
			{
				continue;
			}

			IDetailCategoryBuilder* TargetBuilder = &DefaultCategoryBuilder;
			if (!PinArray->PinTemplate.Metadata.Category.IsEmpty())
			{
				TargetBuilder = &DetailLayout.EditCategory(FName(PinArray->PinTemplate.Metadata.Category), FText::FromString(PinArray->PinTemplate.Metadata.Category));
			}

			TSharedRef<FVoxelGraphNodePinArrayCustomization> PinArrayCustomization = MakeVoxelShared<FVoxelGraphNodePinArrayCustomization>();
			PinArrayCustomization->PinName = PinName.ToString();
			PinArrayCustomization->Tooltip = PinArray->PinTemplate.Metadata.Tooltip.Get(); // TODO:
			PinArrayCustomization->WeakCustomization = SharedThis(this);
			PinArrayCustomization->WeakStructNode = Node;
			PinArrayCustomization->WeakNodeDefinition = NodeDefinition;

			for (FName ArrayElementPinName : PinArray->Pins)
			{
				if (TSharedPtr<IPropertyHandle> ValueHandle = MappedHandles.FindRef(ArrayElementPinName))
				{
					PinArrayCustomization->Properties.Add(ValueHandle);
				}
			}

			TargetBuilder->AddCustomBuilder(PinArrayCustomization);
			continue;
		}

		TSharedPtr<FVoxelPin> VoxelPin = Node->Struct->FindPin(PinName);
		if (!VoxelPin)
		{
			continue;
		}

		if (!VoxelPin->Metadata.bShowInDetail)
		{
			continue;
		}

		if (!VoxelPin->ArrayOwner.IsNone())
		{
			continue;
		}

		IDetailCategoryBuilder* TargetBuilder = &DefaultCategoryBuilder;
		if (!VoxelPin->Metadata.Category.IsEmpty())
		{
			TargetBuilder = &DetailLayout.EditCategory(FName(VoxelPin->Metadata.Category), FText::FromString(VoxelPin->Metadata.Category));
		}

		TSharedPtr<IPropertyHandle> ValueHandle = MappedHandles.FindRef(PinName);
		if (!ValueHandle)
		{
			continue;
		}

		ValueHandle->SetPropertyDisplayName(FText::FromString(VoxelPin->Metadata.DisplayName.IsEmpty() ? FName::NameToDisplayString(PinName.ToString(), false) : FName::NameToDisplayString(VoxelPin->Metadata.DisplayName, false)));

		const auto SetupRowLambda = [this, ValueHandle, WeakNode = MakeWeakObjectPtr(Node), PinName](FDetailWidgetRow& Row, const TSharedRef<SWidget>& ValueWidget)
		{
			Row
			.NameContent()
			[
				ValueHandle->CreatePropertyNameWidget()
			]
			.ValueContent()
			.HAlign(HAlign_Fill)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SBox)
					.Visibility_Lambda([WeakNode, PinName]
					{
						const UVoxelGraphStructNode* TargetNode = WeakNode.Get();
						if (!ensure(TargetNode))
						{
							return EVisibility::Visible;
						}

						return TargetNode->Struct->ExposedPins.Contains(PinName) ? EVisibility::Collapsed : EVisibility::Visible;
					})
					.MinDesiredWidth(125.f)
					[
						ValueWidget
					]
				]
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Center)
				.FillWidth(1.f)
				[
					CreateExposePinButton(WeakNode, PinName)
				]
			];
		};

		const TSharedPtr<FVoxelStructCustomizationWrapper> Wrapper = FVoxelPinValueCustomizationHelper::CreatePinValueCustomization(
			ValueHandle->GetChildHandleStatic(FVoxelNodeExposedPinValue, Value),
			*TargetBuilder,
			{},
			SetupRowLambda,
			// Used to load/save expansion state
			FAddPropertyParams().UniqueId("FVoxelGraphNodeCustomization"));

		if (Wrapper)
		{
			Wrappers.Add(Wrapper);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedRef<SWidget> FVoxelGraphNodeCustomization::CreateExposePinButton(const TWeakObjectPtr<UVoxelGraphStructNode>& WeakNode, FName PinName) const
{
	return
		PropertyCustomizationHelpers::MakeVisibilityButton(
			FOnClicked::CreateLambda([WeakNode, PinName]() -> FReply
			{
				UVoxelGraphStructNode* TargetNode = WeakNode.Get();
				if (!ensure(TargetNode))
				{
					return FReply::Handled();
				}

				const FVoxelTransaction Transaction(TargetNode, "Expose Pin");

				if (TargetNode->Struct->ExposedPins.Remove(PinName) == 0)
				{
					TargetNode->Struct->ExposedPins.Add(PinName);
				}

				TargetNode->ReconstructNode(false);
				return FReply::Handled();
			}),
			{},
			MakeAttributeLambda([WeakNode, PinName]
			{
				const UVoxelGraphStructNode* TargetNode = WeakNode.Get();
				if (!ensure(TargetNode))
				{
					return false;
				}

				return TargetNode->Struct->ExposedPins.Contains(PinName) ? true : false;
			}));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TMap<FName, TSharedPtr<IPropertyHandle>> FVoxelGraphNodeCustomization::InitializeStructChildren(const TSharedRef<IPropertyHandle>& StructHandle)
{
	const TSharedPtr<FVoxelStructCustomizationWrapper> Wrapper = FVoxelStructCustomizationWrapper::Make(StructHandle);
	if (!Wrapper)
	{
		return {};
	}

	Wrappers.Add(Wrapper);

	TMap<FName, TSharedPtr<IPropertyHandle>> MappedChildHandles;
	for (const TSharedPtr<IPropertyHandle>& ChildHandle : Wrapper->AddChildStructure())
	{
		MappedChildHandles.Add(ChildHandle->GetProperty()->GetFName(), ChildHandle);
	}
	return MappedChildHandles;
}