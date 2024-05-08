// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelGraphKnotNode.h"

void UVoxelGraphKnotNode::AllocateDefaultPins()
{
	UEdGraphPin* InputPin = CreatePin(EGPD_Input, FVoxelPinType::MakeWildcard().GetEdGraphPinType(), STATIC_FNAME("InputPin"));
	InputPin->bDefaultValueIsIgnored = true;

	CreatePin(EGPD_Output, FVoxelPinType::MakeWildcard().GetEdGraphPinType(), STATIC_FNAME("OutputPin"));

	Super::AllocateDefaultPins();
}

bool UVoxelGraphKnotNode::CanPromotePin(const UEdGraphPin& Pin, FVoxelPinTypeSet& OutTypes) const
{
	const UEdGraphPin* InputPin = GetPin(EGPD_Input);
	const UEdGraphPin* OutputPin = GetPin(EGPD_Output);

	if (&Pin == InputPin)
	{
		if (OutputPin->LinkedTo.Num() != 0 &&
			!FVoxelPinType(OutputPin->PinType).IsWildcard())
		{
			return false;
		}
	}
	if (&Pin == OutputPin)
	{
		if (InputPin->LinkedTo.Num() != 0 &&
			!FVoxelPinType(InputPin->PinType).IsWildcard())
		{
			return false;
		}
	}

	OutTypes = FVoxelPinTypeSet::All();
	return true;
}

void UVoxelGraphKnotNode::PromotePin(UEdGraphPin& Pin, const FVoxelPinType& NewType)
{
	Modify();

	for (UEdGraphPin* OtherPin : Pins)
	{
		OtherPin->PinType = NewType.GetEdGraphPinType();
	}

	PropagatePinType();
}

bool UVoxelGraphKnotNode::CanSplitPin(const UEdGraphPin& Pin) const
{
	return false;
}

void UVoxelGraphKnotNode::SplitPin(UEdGraphPin& Pin)
{
}

void UVoxelGraphKnotNode::PostReconstructNode()
{
	Super::PostReconstructNode();

	PropagatePinType();
}

FText UVoxelGraphKnotNode::GetTooltipText() const
{
	return INVTEXT("Reroute Node (reroutes wires)");
}

FText UVoxelGraphKnotNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (TitleType == ENodeTitleType::EditableTitle)
	{
		return FText::FromString(NodeComment);
	}
	else if (TitleType == ENodeTitleType::MenuTitle)
	{
		return INVTEXT("Add Reroute Node...");
	}
	else
	{
		return INVTEXT("Reroute Node");
	}
}

FText UVoxelGraphKnotNode::GetPinNameOverride(const UEdGraphPin& Pin) const
{
	// Keep the pin size tiny
	return FText::GetEmpty();
}

void UVoxelGraphKnotNode::OnRenameNode(const FString& NewName)
{
	NodeComment = NewName;
}

UEdGraphPin* UVoxelGraphKnotNode::GetPassThroughPin(const UEdGraphPin* FromPin) const
{
	if (FromPin && Pins.Contains(FromPin))
	{
		return FromPin == Pins[0] ? Pins[1] : Pins[0];
	}

	return nullptr;
}

bool UVoxelGraphKnotNode::ShouldDrawNodeAsControlPointOnly(int32& OutInputPinIndex, int32& OutOutputPinIndex) const
{
	OutInputPinIndex = 0;
	OutOutputPinIndex = 1;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void UVoxelGraphKnotNode::PropagatePinType()
{
	UEdGraphPin* InputPin = GetPin(EGPD_Input);
	UEdGraphPin* OutputPin = GetPin(EGPD_Output);

	if (!FVoxelPinType(InputPin->PinType).IsWildcard() ||
		!FVoxelPinType(OutputPin->PinType).IsWildcard())
	{
		return;
	}

	for (const UEdGraphPin* Inputs : InputPin->LinkedTo)
	{
		if (!FVoxelPinType(Inputs->PinType).IsWildcard())
		{
			PropagatePinTypeInDirection(EGPD_Input);
			return;
		}
	}

	for (const UEdGraphPin* Outputs : OutputPin->LinkedTo)
	{
		if (!FVoxelPinType(Outputs->PinType).IsWildcard())
		{
			PropagatePinTypeInDirection(EGPD_Output);
			return;
		}
	}

	// if all inputs/outputs are wildcards, still favor the inputs first (propagate array/reference/etc. state)
	if (InputPin->LinkedTo.Num() > 0)
	{
		// If we can't mirror from output type, we should at least get the type information from the input connection chain
		PropagatePinTypeInDirection(EGPD_Input);
		return;
	}

	if (OutputPin->LinkedTo.Num() > 0)
	{
		// Try to mirror from output first to make sure we get appropriate member references
		PropagatePinTypeInDirection(EGPD_Output);
		return;
	}

	// Revert to wildcard
	InputPin->BreakAllPinLinks();
	InputPin->PinType = FVoxelPinType::MakeWildcard().GetEdGraphPinType();

	OutputPin->BreakAllPinLinks();
	OutputPin->PinType = FVoxelPinType::MakeWildcard().GetEdGraphPinType();
}

UEdGraphPin* UVoxelGraphKnotNode::GetPin(EEdGraphPinDirection Direction) const
{
	return Direction == EGPD_Input ? Pins[0] : Pins[1];
}

TArray<UEdGraphPin*> UVoxelGraphKnotNode::GetLinkedPins(EEdGraphPinDirection Direction)
{
	TArray<UEdGraphPin*> LinkedPins;

	TArray<UVoxelGraphKnotNode*> KnotsToProcess;
	KnotsToProcess.Add(this);
	while (KnotsToProcess.Num() > 0)
	{
		const UVoxelGraphKnotNode* CurrentKnot = KnotsToProcess.Pop(false);
		for (UEdGraphPin* Pin : CurrentKnot->GetPin(Direction)->LinkedTo)
		{
			if (UVoxelGraphKnotNode* Knot = Cast<UVoxelGraphKnotNode>(Pin->GetOwningNode()))
			{
				KnotsToProcess.Add(Knot);
			}
			else
			{
				LinkedPins.Add(Pin);
			}
		}
	}

	return LinkedPins;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void UVoxelGraphKnotNode::PropagatePinTypeInDirection(EEdGraphPinDirection Direction)
{
	if (bRecursionGuard)
	{
		return;
	}
	TGuardValue<bool> RecursionGuard(bRecursionGuard, true);

	UEdGraphPin* SourcePin = GetPin(Direction);
	if (SourcePin->LinkedTo.Num() == 0)
	{
		return;
	}

	for (const UEdGraphPin* LinkedTo : SourcePin->LinkedTo)
	{
		if (UVoxelGraphKnotNode* KnotNode = Cast<UVoxelGraphKnotNode>(LinkedTo->GetOwningNode()))
		{
			KnotNode->PropagatePinTypeInDirection(Direction);
		}
	}

	const UEdGraphPin* TypeSource = SourcePin->LinkedTo[0];
	for (UEdGraphPin* Pin : Pins)
	{
		Pin->PinType = TypeSource->PinType;
	}
}