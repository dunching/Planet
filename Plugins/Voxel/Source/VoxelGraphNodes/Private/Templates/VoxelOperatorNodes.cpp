// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Templates/VoxelOperatorNodes.h"

FVoxelTemplateNode::FPin* FVoxelTemplateNode_OperatorBase::ExpandPins(FNode& Node, TArray<FPin*> Pins, const TArray<FPin*>& AllPins) const
{
	const int32 NumPins = Pins.Num();
	const int32 MaxDimension = GetMaxDimension(AllPins);

	UScriptStruct* NodeStruct;
	if (GetInt32InnerNode() &&
		All(AllPins, IsPinInt))
	{
		NodeStruct = GetInt32InnerNode();
	}
	else if (
		GetDoubleInnerNode() &&
		Any(AllPins, IsPinDouble))
	{
		NodeStruct = GetDoubleInnerNode();
		Pins = Apply(Pins, ConvertToDouble);
	}
	else
	{
		NodeStruct = GetFloatInnerNode();
		Pins = Apply(Pins, ConvertToFloat);
	}

	if (!ensure(NodeStruct))
	{
		return nullptr;
	}

	Pins = Apply(Pins, ScalarToVector, MaxDimension);
	Pins = Apply(Pins, ZeroExpandVector, MaxDimension);
	check(Pins.Num() == NumPins);

	if (NumPins == 1)
	{
		return MakeVector(Call_Multi(NodeStruct, BreakVector(Pins[0])));
	}

	return Reduce(Pins, [&](FPin* PinA, FPin* PinB)
	{
		return MakeVector(Call_Multi(NodeStruct, BreakVector(PinA), BreakVector(PinB)));
	});
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#if WITH_EDITOR
FVoxelPinTypeSet FVoxelTemplateNode_OperatorBase::GetPromotionTypes(const FVoxelPin& Pin) const
{
	FVoxelPinTypeSet OutTypes;

	if (GetFloatInnerNode())
	{
		OutTypes.Add(GetFloatTypes());
	}
	if (GetDoubleInnerNode())
	{
		OutTypes.Add(GetDoubleTypes());
	}
	if (GetInt32InnerNode())
	{
		OutTypes.Add(GetIntTypes());
	}

	return OutTypes;
}

void FVoxelTemplateNode_OperatorBase::PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType)
{
	Pin.SetType(NewType);
	ON_SCOPE_EXIT
	{
		ensure(Pin.GetType() == NewType);
	};

	FixupWildcards(NewType);
	EnforceNoPrecisionLoss(Pin, NewType, GetAllPins());
	FixupBuffers(NewType, GetAllPins());
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#if WITH_EDITOR
void FVoxelTemplateNode_CommutativeAssociativeOperator::FDefinition::AddInputPin()
{
	AddToCategory(Node.InputPins);

	FVoxelPinType Type;
	for (const FVoxelPin& Pin : Node.GetPins())
	{
		if (Pin.GetType().IsWildcard())
		{
			continue;
		}

		Type = Pin.GetType();
		break;
	}

	if (!Type.IsValid())
	{
		return;
	}

	for (FVoxelPin& Pin : Node.GetPins())
	{
		if (Pin.GetType().IsWildcard())
		{
			Pin.SetType(Type);
		}
	}
}
#endif