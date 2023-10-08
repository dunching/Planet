// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelGraphMacroParameterNode.h"
#include "VoxelGraphToolkit.h"

void UVoxelGraphMacroParameterInputNode::AllocateParameterPins(const FVoxelGraphParameter& Parameter)
{
	if (bExposeDefaultPin)
	{
		UEdGraphPin* InputPin = CreatePin(
			EGPD_Input,
			Parameter.Type.GetEdGraphPinType(),
			STATIC_FNAME("Default"));

		InputPin->PinFriendlyName = FText::FromName(Parameter.Name);
	}

	UEdGraphPin* OutputPin = CreatePin(
		EGPD_Output,
		Parameter.Type.GetEdGraphPinType(),
		STATIC_FNAME("Value"));

	OutputPin->PinFriendlyName = FText::FromName(Parameter.Name);
}

FText UVoxelGraphMacroParameterInputNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return INVTEXT("INPUT");
}

void UVoxelGraphMacroParameterInputNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetMemberPropertyName() != GET_OWN_MEMBER_NAME(bExposeDefaultPin))
	{
		return;
	}

	ReconstructNode();

	const TSharedPtr<FVoxelGraphToolkit> Toolkit = GetToolkit();
	if (!ensure(Toolkit))
	{
		return;
	}

	Toolkit->Asset->OnParametersChanged.Broadcast(UVoxelGraph::EParameterChangeType::Unknown);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void UVoxelGraphMacroParameterOutputNode::AllocateParameterPins(const FVoxelGraphParameter& Parameter)
{
	UEdGraphPin* Pin = CreatePin(
		EGPD_Input,
		Parameter.Type.GetEdGraphPinType(),
		STATIC_FNAME("Value"));

	Pin->PinFriendlyName = FText::FromName(Parameter.Name);
}

FText UVoxelGraphMacroParameterOutputNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return INVTEXT("OUTPUT");
}