// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelGraphParameterNode.h"
#include "VoxelGraphToolkit.h"

void UVoxelGraphParameterNode::AllocateParameterPins(const FVoxelGraphParameter& Parameter)
{
	FVoxelPinType Type = Parameter.Type;
	if (bIsBuffer)
	{
		Type = Type.GetBufferType();
	}

	UEdGraphPin* Pin = CreatePin(
		EGPD_Output,
		Type.GetEdGraphPinType(),
		STATIC_FNAME("Value"));

	Pin->PinFriendlyName = FText::FromName(Parameter.Name);
}

FText UVoxelGraphParameterNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (TitleType != ENodeTitleType::FullTitle)
	{
		return {};
	}

	return FText::FromName(GetParameterSafe().Name);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool UVoxelGraphParameterNode::CanPromotePin(const UEdGraphPin& Pin, FVoxelPinTypeSet& OutTypes) const
{
	if (Pin.ParentPin ||
		Pin.bOrphanedPin)
	{
		return false;
	}

	const FVoxelGraphParameter* Parameter = GetParameter();
	if (!Parameter)
	{
		return false;
	}

	OutTypes.Add(Parameter->Type.GetInnerType());
	OutTypes.Add(Parameter->Type.GetBufferType());

	return true;
}

void UVoxelGraphParameterNode::PromotePin(UEdGraphPin& Pin, const FVoxelPinType& NewType)
{
	Modify();

	ensure(bIsBuffer != NewType.IsBuffer());
	bIsBuffer = NewType.IsBuffer();

	ReconstructNode(false);
}