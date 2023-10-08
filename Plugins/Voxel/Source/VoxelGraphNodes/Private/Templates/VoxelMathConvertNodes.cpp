// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Templates/VoxelMathConvertNodes.h"
#include "VoxelCompiledGraph.h"

FVoxelTemplateNode::FPin* FVoxelTemplateNode_AbstractMathConvert::ExpandPins(FNode& Node, TArray<FPin*> Pins, const TArray<FPin*>& AllPins) const
{
	UScriptStruct* NodeStruct = nullptr;

	if (IsPinFloat(&Node.GetOutputPin(0)))
	{
		Pins = Apply(Pins, ConvertToFloat);
		NodeStruct = GetFloatToFloatInnerNode();
	}
	else if (IsPinDouble(&Node.GetOutputPin(0)))
	{
		Pins = Apply(Pins, ConvertToDouble);
		NodeStruct = GetDoubleToDoubleInnerNode();
	}
	else if (IsPinInt(&Node.GetOutputPin(0)))
	{
		if (IsPinFloat(&Node.GetInputPin(0)))
		{
			NodeStruct = GetFloatToInt32InnerNode();
		}
		else if (ensure(IsPinDouble(&Node.GetInputPin(0))))
		{
			NodeStruct = GetDoubleToInt32InnerNode();
		}
	}

	if (!ensure(NodeStruct))
	{
		return nullptr;
	}

	const int32 MaxDimension = GetMaxDimension(AllPins);

	Pins = Apply(Pins, ScalarToVector, MaxDimension);
	check(Pins.Num() == 1);

	const TArray<TArray<FPin*>> BrokenPins = ApplyVector(Pins, BreakVector);
	check(BrokenPins.Num() == 1);

	return MakeVector(Call_Multi(NodeStruct, BrokenPins));
}

#if WITH_EDITOR
FVoxelPinTypeSet FVoxelTemplateNode_AbstractMathConvert::GetPromotionTypes(const FVoxelPin& Pin) const
{
	if (Pin.Name == ResultPin)
	{
		FVoxelPinTypeSet OutTypes;
		OutTypes.Add(GetFloatTypes());
		OutTypes.Add(GetDoubleTypes());
		OutTypes.Add(GetIntTypes());
		return OutTypes;
	}

	FVoxelPinTypeSet OutTypes;
	OutTypes.Add(GetFloatTypes());
	OutTypes.Add(GetDoubleTypes());
	return OutTypes;
}

void FVoxelTemplateNode_AbstractMathConvert::PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType)
{
	Pin.SetType(NewType);
	ON_SCOPE_EXIT
	{
		ensure(Pin.GetType() == NewType);
	};

	FixupWildcards(NewType);
	EnforceSameDimensions(Pin, NewType, GetAllPins());

	// Input cannot be int
	if (IsInt(GetPin(ValuePin).GetType()))
	{
		SetPinScalarType<float>(GetPin(ValuePin));
	}

	FixupBuffers(NewType, GetAllPins());
}
#endif