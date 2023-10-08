// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Templates/VoxelTemplatedMathNodes.h"
#include "VoxelCompiledGraph.h"

FVoxelTemplateNode::FPin* FVoxelTemplateNode_FloatMathNode::ExpandPins(FNode& Node, TArray<FPin*> Pins, const TArray<FPin*>& AllPins) const
{
	const int32 NumPins = Pins.Num();
	const int32 MaxDimension = GetDimension(Node.GetOutputPin(0).Type);

	Pins = Apply(Pins, ConvertToFloat);
	Pins = Apply(Pins, ScalarToVector, MaxDimension);
	check(Pins.Num() == NumPins);

	const TArray<TArray<FPin*>> BrokenPins = ApplyVector(Pins, BreakVector);
	check(BrokenPins.Num() == NumPins);

	return MakeVector(Call_Multi(GetInnerNode(), BrokenPins));
}

#if WITH_EDITOR
FVoxelPinTypeSet FVoxelTemplateNode_FloatMathNode::GetPromotionTypes(const FVoxelPin& Pin) const
{
	FVoxelPinTypeSet OutTypes;
	OutTypes.Add(GetFloatTypes());
	return OutTypes;
}

void FVoxelTemplateNode_FloatMathNode::PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType)
{
	// NewType might be int!
	Pin.SetType(NewType);
	ON_SCOPE_EXIT
	{
		ensure(Pin.GetType() == NewType);
	};

	FixupWildcards(NewType);
	EnforceSameDimensions(Pin, NewType, GetAllPins());

	// Fixup output
	SetPinScalarType<float>(GetPin(ReturnValuePin));

	FixupBuffers(NewType, GetAllPins());
}
#endif