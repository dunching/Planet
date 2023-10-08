// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Templates/VoxelLerpNodes.h"
#include "VoxelCompiledGraph.h"

FVoxelTemplateNode::FPin* FVoxelTemplateNode_LerpBase::ExpandPins(FNode& Node, TArray<FPin*> Pins, const TArray<FPin*>& AllPins) const
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
FVoxelPinTypeSet FVoxelTemplateNode_LerpBase::GetPromotionTypes(const FVoxelPin& Pin) const
{
	if (Pin.Name == AlphaPin ||
		Pin.Name == ResultPin)
	{
		FVoxelPinTypeSet OutTypes;
		OutTypes.Add(GetFloatTypes());
		return OutTypes;
	}

	FVoxelPinTypeSet OutTypes;
	OutTypes.Add(GetFloatTypes());
	OutTypes.Add(GetIntTypes());
	return OutTypes;
}

void FVoxelTemplateNode_LerpBase::PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType)
{
	// NewType might be int!
	Pin.SetType(NewType);
	ON_SCOPE_EXIT
	{
		ensure(Pin.GetType() == NewType);
	};

	FixupWildcards(NewType);
	EnforceSameDimensions(Pin, NewType, GetAllPins());

	// Fixup Alpha
	SetPinScalarType<float>(GetPin(AlphaPin));

	// Fixup output
	SetPinScalarType<float>(GetPin(ResultPin));

	FixupBuffers(NewType, GetAllPins());
}
#endif