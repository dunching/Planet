// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelGraphStructNode.h"
#include "VoxelGraphVisuals.h"
#include "VoxelGraphToolkit.h"
#include "VoxelExposedSeed.h"
#include "VoxelFunctionNode.h"
#include "VoxelGraphMigration.h"

void UVoxelGraphStructNode::AllocateDefaultPins()
{
	if (!Struct.IsValid())
	{
		return;
	}

	CachedName = Struct->GetDisplayName();

	for (const FVoxelPin& Pin : Struct->GetPins())
	{
		UEdGraphPin* GraphPin = CreatePin(
			Pin.bIsInput ? EGPD_Input : EGPD_Output,
			Pin.GetType().GetEdGraphPinType(),
			Pin.Name);

		GraphPin->PinFriendlyName = FText::FromString(Pin.Metadata.DisplayName);
		if (GraphPin->PinFriendlyName.IsEmpty())
		{
			GraphPin->PinFriendlyName = INVTEXT(" ");
		}

		GraphPin->PinToolTip = Pin.Metadata.Tooltip.Get();

		GraphPin->bHidden = Struct->IsPinHidden(Pin);

		InitializeDefaultValue(Pin, *GraphPin);
	}

	// If we only have a single pin hide its name
	if (Pins.Num() == 1 &&
		Pins[0]->Direction == EGPD_Output)
	{
		Pins[0]->PinFriendlyName = INVTEXT(" ");
	}

	Super::AllocateDefaultPins();
}

void UVoxelGraphStructNode::PrepareForCopying()
{
	Super::PrepareForCopying();

	if (!Struct.IsValid())
	{
		return;
	}

	Struct->PreSerialize();
}

void UVoxelGraphStructNode::PostPasteNode()
{
	Super::PostPasteNode();

	if (!Struct.IsValid())
	{
		return;
	}

	Struct->PostSerialize();
}

FText UVoxelGraphStructNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (!Struct.IsValid())
	{
		return FText::FromString(CachedName);
	}

	if (Struct->GetMetadataContainer().HasMetaData(STATIC_FNAME("Autocast")))
	{
		return INVTEXT("->");
	}

	const FText CompactTitle = Struct->GetMetadataContainer().GetMetaDataText(FBlueprintMetadata::MD_CompactNodeTitle);
	if (!CompactTitle.IsEmpty())
	{
		return CompactTitle;
	}

	return FText::FromString(Struct->GetDisplayName());
}

FLinearColor UVoxelGraphStructNode::GetNodeTitleColor() const
{
	if (Struct.IsValid() &&
		Struct->GetMetadataContainer().HasMetaDataHierarchical(STATIC_FNAME("NodeColor")))
	{
		return FVoxelGraphVisuals::GetNodeColor(GetStringMetaDataHierarchical(&Struct->GetMetadataContainer(), STATIC_FNAME("NodeColor")));
	}

	if (Struct.IsValid() &&
		Struct->IsPureNode())
	{
		return GetDefault<UGraphEditorSettings>()->PureFunctionCallNodeTitleColor;
	}

	return GetDefault<UGraphEditorSettings>()->FunctionCallNodeTitleColor;
}

FText UVoxelGraphStructNode::GetTooltipText() const
{
	if (!Struct.IsValid())
	{
		return {};
	}

	return FText::FromString(Struct->GetTooltip());
}

FSlateIcon UVoxelGraphStructNode::GetIconAndTint(FLinearColor& OutColor) const
{
	FSlateIcon Icon("EditorStyle", "Kismet.AllClasses.FunctionIcon");
	if (Struct.IsValid() &&
		Struct->GetMetadataContainer().HasMetaDataHierarchical("NodeIcon"))
	{
		Icon = FVoxelGraphVisuals::GetNodeIcon(GetStringMetaDataHierarchical(&Struct->GetMetadataContainer(), "NodeIcon"));
	}

	if (Struct.IsValid() &&
		Struct->GetMetadataContainer().HasMetaDataHierarchical("NodeIconColor"))
	{
		OutColor = FVoxelGraphVisuals::GetNodeColor(GetStringMetaDataHierarchical(&Struct->GetMetadataContainer(), "NodeIconColor"));
		return Icon;
	}

	if (Struct.IsValid() &&
		Struct->IsPureNode())
	{
		OutColor = GetDefault<UGraphEditorSettings>()->PureFunctionCallNodeTitleColor;
		return Icon;
	}

	OutColor = GetDefault<UGraphEditorSettings>()->FunctionCallNodeTitleColor;
	return Icon;
}

bool UVoxelGraphStructNode::IsCompact() const
{
	if (!Struct.IsValid())
	{
		return {};
	}

	return
		Struct->GetMetadataContainer().HasMetaData(STATIC_FNAME("Autocast")) ||
		Struct->GetMetadataContainer().HasMetaData(FBlueprintMetadata::MD_CompactNodeTitle);
}

bool UVoxelGraphStructNode::GetOverlayInfo(FString& Type, FString& Tooltip, FString& Color)
{
	if (!Struct.IsValid())
	{
		return false;
	}

	const UStruct& MetadataContainer = Struct->GetMetadataContainer();
	if (!MetadataContainer.HasMetaDataHierarchical("OverlayTooltip") &&
		!MetadataContainer.HasMetaDataHierarchical("OverlayType") &&
		!MetadataContainer.HasMetaDataHierarchical("OverlayColor"))
	{
		return false;
	}

	if (MetadataContainer.HasMetaDataHierarchical("OverlayTooltip"))
	{
		Tooltip = GetStringMetaDataHierarchical(&MetadataContainer, "OverlayTooltip");
	}

	if (MetadataContainer.HasMetaDataHierarchical("OverlayType"))
	{
		Type = GetStringMetaDataHierarchical(&MetadataContainer, "OverlayType");
	}
	else
	{
		Type = "Warning";
	}

	if (MetadataContainer.HasMetaDataHierarchical("OverlayColor"))
	{
		Color = GetStringMetaDataHierarchical(&MetadataContainer, "OverlayColor");
	}

	return true;
}

bool UVoxelGraphStructNode::ShowAsPromotableWildcard(const UEdGraphPin& Pin) const
{
	if (!Struct.IsValid() ||
		Pin.ParentPin ||
		Pin.bOrphanedPin)
	{
		return false;
	}

	const TSharedPtr<const FVoxelPin> StructPin = Struct->FindPin(Pin.PinName);
	if (!ensure(StructPin))
	{
		return false;
	}

	if (!StructPin->IsPromotable() ||
		!Struct->ShowPromotablePinsAsWildcards())
	{
		return false;
	}

	return true;
}

bool UVoxelGraphStructNode::IsPinOptional(const UEdGraphPin& Pin) const
{
	if (!Struct.IsValid() ||
		Pin.ParentPin ||
		Pin.bOrphanedPin)
	{
		return false;
	}

	const TSharedPtr<const FVoxelPin> StructPin = Struct->FindPin(Pin.PinName);
	if (!ensure(StructPin))
	{
		return {};
	}

	return StructPin->Metadata.bOptionalPin;
}

bool UVoxelGraphStructNode::ShouldHidePinDefaultValue(const UEdGraphPin& Pin) const
{
	if (!Struct.IsValid() ||
		Pin.ParentPin ||
		Pin.bOrphanedPin)
	{
		return false;
	}

	const TSharedPtr<const FVoxelPin> StructPin = Struct->FindPin(Pin.PinName);
	if (!ensure(StructPin))
	{
		return {};
	}

	return StructPin->Metadata.bNoDefault;
}

FName UVoxelGraphStructNode::GetPinCategory(const UEdGraphPin& Pin) const
{
	if (!Struct.IsValid())
	{
		return {};
	}

	const TSharedPtr<const FVoxelPin> StructPin = Struct->FindPin(Pin.PinName);
	if (!ensure(StructPin))
	{
		return {};
	}

	return *StructPin->Metadata.Category;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedRef<IVoxelNodeDefinition> UVoxelGraphStructNode::GetNodeDefinition()
{
	if (!ensureVoxelSlow(Struct))
	{
		return MakeVoxelShared<IVoxelNodeDefinition>();
	}

	const TSharedRef<FVoxelNodeDefinition> NodeDefinition = Struct->GetNodeDefinition();
	NodeDefinition->Initialize(*this);
	return MakeVoxelShared<FVoxelGraphStructNodeDefinition>(*this, NodeDefinition);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool UVoxelGraphStructNode::CanRemovePin_ContextMenu(const UEdGraphPin& Pin) const
{
	if (!Struct.IsValid() ||
		Pin.ParentPin ||
		Pin.bOrphanedPin)
	{
		return false;
	}

	return ConstCast(this)->GetNodeDefinition()->CanRemoveSelectedPin(Pin.PinName);
}

void UVoxelGraphStructNode::RemovePin_ContextMenu(UEdGraphPin& Pin)
{
	GetNodeDefinition()->RemoveSelectedPin(Pin.PinName);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool UVoxelGraphStructNode::CanPromotePin(const UEdGraphPin& Pin, FVoxelPinTypeSet& OutTypes) const
{
	if (Pin.ParentPin ||
		Pin.bOrphanedPin)
	{
		return false;
	}

	const TSharedPtr<const FVoxelPin> VoxelPin = Struct->FindPin(Pin.PinName);
	if (!ensure(VoxelPin) ||
		!VoxelPin->IsPromotable())
	{
		return false;
	}

	OutTypes = Struct->GetPromotionTypes(*VoxelPin);
	return true;
}

FString UVoxelGraphStructNode::GetPinPromotionWarning(const UEdGraphPin& Pin, const FVoxelPinType& NewType) const
{
	const TSharedPtr<const FVoxelPin> VoxelPin = Struct->FindPin(Pin.PinName);
	if (!ensure(VoxelPin) ||
		!VoxelPin->IsPromotable())
	{
		return {};
	}

	return Struct->GetPinPromotionWarning(*VoxelPin, NewType);
}

void UVoxelGraphStructNode::PromotePin(UEdGraphPin& Pin, const FVoxelPinType& NewType)
{
	Modify();

	const TSharedPtr<FVoxelPin> VoxelPin = Struct->FindPin(Pin.PinName);
	if (!ensure(VoxelPin) ||
		!ensure(VoxelPin->IsPromotable()))
	{
		return;
	}

	Struct->PromotePin(*VoxelPin, NewType);

	ReconstructFromVoxelNode();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool UVoxelGraphStructNode::TryMigratePin(UEdGraphPin* OldPin, UEdGraphPin* NewPin) const
{
	if (!Super::TryMigratePin(OldPin, NewPin))
	{
		return false;
	}

	if (Struct)
	{
		if (const TSharedPtr<const FVoxelPin> VoxelPin = Struct->FindPin(NewPin->PinName))
		{
			if (VoxelPin->Metadata.bShowInDetail)
			{
				const FString DefaultValueString = Struct->GetPinDefaultValue(*VoxelPin.Get());

				FVoxelPinValue DefaultValue(VoxelPin->GetType().GetPinDefaultValueType());
				ensure(DefaultValue.ImportFromString(DefaultValueString));
				DefaultValue.ApplyToPinDefaultValue(*NewPin);
			}
		}
	}

	return true;
}

void UVoxelGraphStructNode::TryMigrateDefaultValue(const UEdGraphPin* OldPin, UEdGraphPin* NewPin) const
{
	if (const TSharedPtr<const FVoxelPin> VoxelPin = Struct->FindPin(NewPin->PinName))
	{
		if (VoxelPin->Metadata.bShowInDetail)
		{
			return;
		}
	}

	Super::TryMigrateDefaultValue(OldPin, NewPin);
}

FName UVoxelGraphStructNode::GetMigratedPinName(const FName PinName) const
{
	if (!Struct)
	{
		return PinName;
	}

	UObject* Outer;
	if (Struct->IsA<FVoxelFunctionNode>())
	{
		Outer = Struct.Get<FVoxelFunctionNode>().GetFunction();
	}
	else
	{
		Outer = Struct.GetScriptStruct();
	}

	if (!Outer)
	{
		return PinName;
	}

	const FName MigratedName = GVoxelGraphMigration->FindNewPinName(Outer, PinName);
	if (MigratedName.IsNone())
	{
		return PinName;
	}

	return MigratedName;
}

void UVoxelGraphStructNode::PreReconstructNode()
{
	VOXEL_FUNCTION_COUNTER();

	Super::PreReconstructNode();

	if (!Struct)
	{
		return;
	}

	for (const UEdGraphPin* Pin : Pins)
	{
		if (Pin->bOrphanedPin ||
			Pin->ParentPin ||
			Pin->LinkedTo.Num() > 0 ||
			!FVoxelPinType(Pin->PinType).HasPinDefaultValue())
		{
			return;
		}

		const TSharedPtr<const FVoxelPin> VoxelPin = Struct->FindPin(Pin->PinName);
		if (!ensureVoxelSlow(VoxelPin))
		{
			continue;
		}

		const FVoxelPinValue DefaultValue = FVoxelPinValue::MakeFromPinDefaultValue(*Pin);
		GetNodeDefinition()->OnPinDefaultValueChanged(Pin->PinName, DefaultValue);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void UVoxelGraphStructNode::PinDefaultValueChanged(UEdGraphPin* Pin)
{
	if (const TSharedPtr<const FVoxelPin> VoxelPin = Struct->FindPin(Pin->PinName))
	{
		const FVoxelPinValue DefaultValue = FVoxelPinValue::MakeFromPinDefaultValue(*Pin);

		if (VoxelPin->Metadata.bShowInDetail)
		{
			Struct->UpdatePropertyBoundDefaultValue(*VoxelPin.Get(), DefaultValue);
		}

		if (ensure(Pin->LinkedTo.Num() == 0) &&
			GetNodeDefinition()->OnPinDefaultValueChanged(Pin->PinName, DefaultValue))
		{
			const TSharedPtr<FVoxelGraphToolkit> Toolkit = GetToolkit();
			if (ensure(Toolkit))
			{
				Toolkit->AddNodeToReconstruct(this);
			}
		}
	}

	Super::PinDefaultValueChanged(Pin);
}

void UVoxelGraphStructNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.ChangeType == EPropertyChangeType::Redirected)
	{
		return;
	}

	if (const FProperty* Property = PropertyChangedEvent.MemberProperty)
	{
		if (Property->GetFName() == GET_MEMBER_NAME_STATIC(UVoxelGraphStructNode, Struct))
		{
			ReconstructFromVoxelNode();
		}
	}
}

void UVoxelGraphStructNode::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);

	if (Struct)
	{
		Struct->PostEditChangeProperty(PropertyChangedEvent);
	}

	if (const FEditPropertyChain::TDoubleLinkedListNode* Node = PropertyChangedEvent.PropertyChain.GetHead())
	{
		if (Node->GetValue()->GetFName() == GET_MEMBER_NAME_STATIC(UVoxelGraphStructNode, Struct))
		{
			if (const TSharedPtr<FVoxelGraphToolkit> Toolkit = GetToolkit())
			{
				Toolkit->OnGraphChanged(GetGraph());
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void UVoxelGraphStructNode::ReconstructFromVoxelNode()
{
	ReconstructNode(false);
}

void UVoxelGraphStructNode::InitializeDefaultValue(const FVoxelPin& VoxelPin, UEdGraphPin& GraphPin)
{
	const FVoxelPinType Type(GraphPin.PinType);
	ensure(Type == VoxelPin.GetType());

	if (!Type.HasPinDefaultValue())
	{
		ensure(!GraphPin.DefaultObject);
		ensure(GraphPin.DefaultValue.IsEmpty());
		ensure(GraphPin.AutogeneratedDefaultValue.IsEmpty());
		return;
	}

	const FVoxelPinType InnerExposedType = Type.GetPinDefaultValueType();
	FString DefaultValueString = Struct->GetPinDefaultValue(VoxelPin);

	if (InnerExposedType.Is<FVoxelExposedSeed>())
	{
		ensure(DefaultValueString.IsEmpty());

		FVoxelExposedSeed NewSeed;
		NewSeed.Randomize();

		const FVoxelPinValue DefaultValue = FVoxelPinValue::Make(NewSeed);
		Struct->UpdatePropertyBoundDefaultValue(VoxelPin, DefaultValue);

		GraphPin.ResetDefaultValue();
		GraphPin.DefaultValue = DefaultValue.ExportToString();
		GraphPin.AutogeneratedDefaultValue = {};
		return;
	}

	if (InnerExposedType.IsObject())
	{
		if (DefaultValueString.IsEmpty())
		{
			ensure(!GraphPin.DefaultObject);
			ensure(GraphPin.DefaultValue.IsEmpty());
			ensure(GraphPin.AutogeneratedDefaultValue.IsEmpty());
			return;
		}

		UObject* Object = FSoftObjectPtr(DefaultValueString).LoadSynchronous();
		if (!ensure(Object))
		{
			GraphPin.ResetDefaultValue();
			GraphPin.AutogeneratedDefaultValue = {};
			return;
		}

		GraphPin.ResetDefaultValue();
		GraphPin.DefaultObject = Object;
		GraphPin.AutogeneratedDefaultValue = Object->GetPathName();
		return;
	}

	// Try to fixup the default value
	if (DefaultValueString.IsEmpty())
	{
		FVoxelPinValue Value(InnerExposedType);
		DefaultValueString = Value.ExportToString();
	}

	{
		FVoxelPinValue Value(InnerExposedType);
		ensure(Value.ImportFromString(DefaultValueString));
	}

	GraphPin.ResetDefaultValue();
	GraphPin.DefaultValue = DefaultValueString;
	GraphPin.AutogeneratedDefaultValue = DefaultValueString;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedPtr<const IVoxelNodeDefinition::FNode> FVoxelGraphStructNodeDefinition::GetInputs() const
{
	return NodeDefinition->GetInputs();
}

TSharedPtr<const IVoxelNodeDefinition::FNode> FVoxelGraphStructNodeDefinition::GetOutputs() const
{
	return NodeDefinition->GetOutputs();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FString FVoxelGraphStructNodeDefinition::GetAddPinLabel() const
{
	return NodeDefinition->GetAddPinLabel();
}

FString FVoxelGraphStructNodeDefinition::GetAddPinTooltip() const
{
	return NodeDefinition->GetAddPinTooltip();
}

FString FVoxelGraphStructNodeDefinition::GetRemovePinTooltip() const
{
	return NodeDefinition->GetRemovePinTooltip();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool FVoxelGraphStructNodeDefinition::CanAddInputPin() const
{
	return NodeDefinition->CanAddInputPin();
}

void FVoxelGraphStructNodeDefinition::AddInputPin()
{
	FScope Scope(this, "Add Input Pin");
	NodeDefinition->AddInputPin();
}

bool FVoxelGraphStructNodeDefinition::CanRemoveInputPin() const
{
	return NodeDefinition->CanRemoveInputPin();
}

void FVoxelGraphStructNodeDefinition::RemoveInputPin()
{
	FScope Scope(this, "Remove Input Pin");
	NodeDefinition->RemoveInputPin();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool FVoxelGraphStructNodeDefinition::CanAddToCategory(const FName Category) const
{
	return NodeDefinition->CanAddToCategory(Category);
}

void FVoxelGraphStructNodeDefinition::AddToCategory(const FName Category)
{
	FScope Scope(this, "Add To Category");
	NodeDefinition->AddToCategory(Category);
}

bool FVoxelGraphStructNodeDefinition::CanRemoveFromCategory(const FName Category) const
{
	return NodeDefinition->CanRemoveFromCategory(Category);
}

void FVoxelGraphStructNodeDefinition::RemoveFromCategory(const FName Category)
{
	FScope Scope(this, "Remove From Category");
	NodeDefinition->RemoveFromCategory(Category);
}

bool FVoxelGraphStructNodeDefinition::CanRemoveSelectedPin(const FName PinName) const
{
	return NodeDefinition->CanRemoveSelectedPin(PinName);
}

void FVoxelGraphStructNodeDefinition::RemoveSelectedPin(const FName PinName)
{
	FScope Scope(this, "Remove " + PinName.ToString() + " Pin");
	NodeDefinition->RemoveSelectedPin(PinName);
}

void FVoxelGraphStructNodeDefinition::InsertPinBefore(const FName PinName)
{
	FScope Scope(this, "Insert Pin Before " + PinName.ToString());
	NodeDefinition->InsertPinBefore(PinName);
}

void FVoxelGraphStructNodeDefinition::DuplicatePin(const FName PinName)
{
	FScope Scope(this, "Duplicate Pin " + PinName.ToString());
	NodeDefinition->DuplicatePin(PinName);
}

bool FVoxelGraphStructNodeDefinition::IsPinVisible(const UEdGraphPin* Pin, const UEdGraphNode* GraphNode)
{
	return NodeDefinition->IsPinVisible(Pin, GraphNode);
}

bool FVoxelGraphStructNodeDefinition::OnPinDefaultValueChanged(const FName PinName, const FVoxelPinValue& NewDefaultValue)
{
	return NodeDefinition->OnPinDefaultValueChanged(PinName, NewDefaultValue);
}