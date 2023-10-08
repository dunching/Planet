// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Templates/VoxelClampNodes.h"
#include "VoxelCompiledGraph.h"

FVoxelTemplateNode::FPin* FVoxelTemplateNode_AbstractClampBase::ExpandPins(FNode& Node, TArray<FPin*> Pins, const TArray<FPin*>& AllPins) const
{
	const int32 NumPins = Pins.Num();
	const int32 MaxDimension = GetDimension(Node.GetOutputPin(0).Type);

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
	check(Pins.Num() == NumPins);

	const TArray<TArray<FPin*>> BrokenPins = ApplyVector(Pins, BreakVector);
	check(BrokenPins.Num() == NumPins);

	return MakeVector(Call_Multi(NodeStruct, BrokenPins));
}

#if WITH_EDITOR
FVoxelPinTypeSet FVoxelTemplateNode_AbstractClampBase::GetPromotionTypes(const FVoxelPin& Pin) const
{
	if (Pin.Name == ResultPin &&
		!GetInt32InnerNode())
	{
		FVoxelPinTypeSet OutTypes;
		OutTypes.Add(GetFloatTypes());
		OutTypes.Add(GetDoubleTypes());
		return OutTypes;
	}

	FVoxelPinTypeSet OutTypes;
	OutTypes.Add(GetFloatTypes());
	OutTypes.Add(GetDoubleTypes());
	OutTypes.Add(GetIntTypes());
	return OutTypes;
}

void FVoxelTemplateNode_AbstractClampBase::PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType)
{
	Pin.SetType(NewType);
	ON_SCOPE_EXIT
	{
		ensure(Pin.GetType() == NewType);
	};

	FixupWildcards(NewType);
	EnforceNoPrecisionLoss(Pin, NewType, GetAllPins());
	EnforceSameDimensions(Pin, NewType, GetAllPins());
	FixupBuffers(NewType, GetAllPins());
}
#endif