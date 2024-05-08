// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelGraphNode.h"
#include "VoxelNodeLibrary.h"
#include "VoxelNodeDefinition.h"
#include "VoxelGraphSchema.h"
#include "VoxelChannelAsset_DEPRECATED.h"

void UVoxelGraphNode::AutowireNewNode(UEdGraphPin* FromPin)
{
	if (!FromPin)
	{
		return;
	}

	const UVoxelGraphSchemaBase* Schema = GetSchema();

	// Check non-promotable and same type (ignore container types) pins first
	for (UEdGraphPin* Pin : Pins)
	{
		const FPinConnectionResponse Response = Schema->CanCreateConnection(FromPin, Pin);

		if (Response.Response == CONNECT_RESPONSE_MAKE ||
			(Response.Response == CONNECT_RESPONSE_MAKE_WITH_PROMOTION && FVoxelPinType(FromPin->PinType).GetInnerType() == FVoxelPinType(Pin->PinType).GetInnerType()))
		{
			Schema->TryCreateConnection(FromPin, Pin);
			return;
		}
	}

	for (UEdGraphPin* Pin : Pins)
	{
		const FPinConnectionResponse Response = Schema->CanCreateConnection(FromPin, Pin);

		if (Response.Response == CONNECT_RESPONSE_MAKE_WITH_PROMOTION)
		{
			Schema->TryCreateConnection(FromPin, Pin);
			return;
		}
		else if (Response.Response == CONNECT_RESPONSE_BREAK_OTHERS_A)
		{
			// The pin we are creating from already has a connection that needs to be broken. We want to "insert" the new node in between, so that the output of the new node is hooked up too
			UEdGraphPin* OldLinkedPin = FromPin->LinkedTo[0];
			check(OldLinkedPin);

			FromPin->BreakAllPinLinks();

			// Hook up the old linked pin to the first valid output pin on the new node
			for (UEdGraphPin* InnerPin : Pins)
			{
				if (Schema->CanCreateConnection(OldLinkedPin, InnerPin).Response != CONNECT_RESPONSE_MAKE)
				{
					continue;
				}

				Schema->TryCreateConnection(OldLinkedPin, InnerPin);
				break;
			}

			Schema->TryCreateConnection(FromPin, Pin);
			return;
		}
	}
}

bool UVoxelGraphNode::CanSplitPin(const UEdGraphPin& Pin) const
{
	if (Pin.ParentPin ||
		Pin.bOrphanedPin ||
		Pin.LinkedTo.Num() > 0 ||
		!ensure(Pin.SubPins.Num() == 0))
	{
		return false;
	}

	if (Pin.Direction == EGPD_Input)
	{
		return FVoxelNodeLibrary::FindMakeNode(Pin.PinType) != nullptr;
	}
	else
	{
		ensure(Pin.Direction == EGPD_Output);
		return FVoxelNodeLibrary::FindBreakNode(Pin.PinType) != nullptr;
	}
}

void UVoxelGraphNode::SplitPin(UEdGraphPin& Pin)
{
	Modify();

	const FVoxelNode* NodeTemplate = Pin.Direction == EGPD_Input
		? FVoxelNodeLibrary::FindMakeNode(Pin.PinType)
		: FVoxelNodeLibrary::FindBreakNode(Pin.PinType);

	if (!ensure(NodeTemplate))
	{
		return;
	}

	const TSharedRef<FVoxelNode> Node = NodeTemplate->MakeSharedCopy();
	{
		FVoxelPin& NodePin =
			Pin.Direction == EGPD_Input
			? Node->GetUniqueOutputPin()
			: Node->GetUniqueInputPin();

		if (NodePin.IsPromotable() &&
			ensure(Node->GetPromotionTypes(NodePin).Contains(Pin.PinType)))
		{
			// Promote so the type are correct - eg if we are an array pin, the split pin should be array too
			Node->PromotePin(NodePin, Pin.PinType);
		}
	}

	TArray<const FVoxelPin*> NewPins;
	for (const FVoxelPin& NewPin : Node->GetPins())
	{
		if (Pin.Direction == EGPD_Input && !NewPin.bIsInput)
		{
			continue;
		}
		if (Pin.Direction == EGPD_Output && NewPin.bIsInput)
		{
			continue;
		}

		NewPins.Add(&NewPin);
	}

	Pin.bHidden = true;

	for (const FVoxelPin* NewPin : NewPins)
	{
		UEdGraphPin* SubPin = CreatePin(
			NewPin->bIsInput ? EGPD_Input : EGPD_Output,
			NewPin->GetType().GetEdGraphPinType(),
			NewPin->Name,
			Pins.IndexOfByKey(&Pin));

		FString Name = Pin.GetDisplayName().ToString();
		if (IsCompact())
		{
			Name.Reset();
		}
		if (!Name.IsEmpty())
		{
			Name += " ";
		}
		Name += NewPin->Metadata.DisplayName;

		SubPin->PinFriendlyName = FText::FromString(Name);
		SubPin->PinToolTip = Name + "\n" + Pin.PinToolTip;

		SubPin->ParentPin = &Pin;
		Pin.SubPins.Add(SubPin);
	}

	RefreshNode();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedRef<IVoxelNodeDefinition> UVoxelGraphNode::GetNodeDefinition()
{
	return MakeVoxelShared<IVoxelNodeDefinition>();
}

bool UVoxelGraphNode::TryMigratePin(UEdGraphPin* OldPin, UEdGraphPin* NewPin) const
{
	if (FVoxelPinType(OldPin->PinType).Is<FVoxelChannelRef_DEPRECATED>() &&
		FVoxelPinType(NewPin->PinType).Is<FVoxelChannelName>() &&
		OldPin->LinkedTo.Num() == 0 &&
		NewPin->LinkedTo.Num() == 0)
	{
		const UVoxelChannelAsset_DEPRECATED* Channel = Cast<UVoxelChannelAsset_DEPRECATED>(OldPin->DefaultObject);
		if (!ensureVoxelSlow(Channel))
		{
			return false;
		}

		NewPin->MovePersistentDataFromOldPin(*OldPin);

		NewPin->DefaultValue = FVoxelPinValue::Make(Channel->MakeChannelName()).ExportToString();
		NewPin->DefaultObject = nullptr;

		return true;
	}

	if (!FVoxelPinType(OldPin->PinType).IsWildcard() &&
		!FVoxelPinType(NewPin->PinType).IsWildcard() &&
		FVoxelPinType(OldPin->PinType) != FVoxelPinType(NewPin->PinType))
	{
		return false;
	}

	NewPin->MovePersistentDataFromOldPin(*OldPin);
	return true;
}

void UVoxelGraphNode::TryMigrateDefaultValue(const UEdGraphPin* OldPin, UEdGraphPin* NewPin) const
{
	if (!FVoxelPinType(OldPin->PinType).HasPinDefaultValue() ||
		!FVoxelPinType(NewPin->PinType).HasPinDefaultValue())
	{
		return;
	}

	const FVoxelPinValue OldValue = FVoxelPinValue::MakeFromPinDefaultValue(*OldPin);

	FVoxelPinValue NewValue(FVoxelPinType(NewPin->PinType).GetPinDefaultValueType());
	if (!NewValue.ImportFromUnrelated(OldValue))
	{
		return;
	}

	NewPin->DefaultValue = NewValue.ExportToString();
}

void UVoxelGraphNode::PostReconstructNode()
{
	// Sanitize default values
	for (UEdGraphPin* Pin : Pins)
	{
		if (Pin->bOrphanedPin)
		{
			continue;
		}

		const FVoxelPinType Type(Pin->PinType);
		if (!Type.HasPinDefaultValue())
		{
			Pin->ResetDefaultValue();
			ensure(Pin->AutogeneratedDefaultValue.IsEmpty());
			return;
		}

		const FVoxelPinType DefaultValueType = Type.GetPinDefaultValueType();
		if (DefaultValueType.IsObject() ||
			DefaultValueType.IsClass())
		{
			if (!Pin->DefaultValue.IsEmpty() &&
				!Pin->DefaultObject)
			{
				Pin->DefaultObject = LoadObject<UObject>(nullptr, *Pin->DefaultValue);
			}
			Pin->DefaultValue.Reset();
			continue;
		}
		Pin->DefaultObject = nullptr;

		if (!ensure(DefaultValueType.IsValid()))
		{
			Pin->DefaultValue.Reset();
			continue;
		}

		if (!FVoxelPinValue(DefaultValueType).ImportFromString(Pin->DefaultValue))
		{
			GetDefault<UVoxelGraphSchemaBase>()->ResetPinToAutogeneratedDefaultValue(Pin, false);
		}
	}
}

bool UVoxelGraphNode::CanCreateUnderSpecifiedSchema(const UEdGraphSchema* Schema) const
{
	return Schema->IsA<UVoxelGraphSchema>();
}