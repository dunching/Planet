// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Templates/VoxelBoolNodes.h"
#include "VoxelCompiledGraph.h"

FVoxelTemplateNode::FPin* FVoxelTemplateNode_EqualityBase::ExpandPins(FNode& Node, TArray<FPin*> Pins, const TArray<FPin*>& AllPins) const
{
	const int32 MaxDimension = GetMaxDimension(AllPins);

	UScriptStruct* NodeStruct;
	if (All(Pins, IsPinBool))
	{
		NodeStruct = GetBoolInnerNode();
	}
	else if (All(Pins, IsPinByte))
	{
		Pins = Apply(Pins, ConvertToByte);
		NodeStruct = GetByteInnerNode();
	}
	else if (All(Pins, IsPinInt))
	{
		NodeStruct = GetInt32InnerNode();
	}
	else if (All(Pins, IsPinObject))
	{
		NodeStruct = GetObjectInnerNode();
	}
	else if (!Any(Pins, IsPinDouble))
	{
		Pins = Apply(Pins, ConvertToFloat);
		NodeStruct = GetFloatInnerNode();
	}
	else
	{
		Pins = Apply(Pins, ConvertToDouble);
		NodeStruct = GetDoubleInnerNode();
	}

	Pins = Apply(Pins, ScalarToVector, MaxDimension);
	check(Pins.Num() == 2);

	const TArray<TArray<FPin*>> BrokenPins = ApplyVector(Pins, BreakVector);
	check(BrokenPins.Num() == 2);

	const TArray<FPin*> BooleanPins = Call_Multi(NodeStruct, BrokenPins);

	return Reduce(BooleanPins, [&](FPin* PinA, FPin* PinB)
	{
		return Call_Single(GetConnectionInnerNode(), PinA, PinB);
	});
}

#if WITH_EDITOR
FVoxelPinTypeSet FVoxelTemplateNode_EqualityBase::GetPromotionTypes(const FVoxelPin& Pin) const
{
	if (Pin.Name == ResultPin)
	{
		FVoxelPinTypeSet OutTypes;
		OutTypes.Add<bool>();
		OutTypes.Add<FVoxelBoolBuffer>();
		return OutTypes;
	}

	FVoxelPinTypeSet OutTypes;
	if (GetBoolInnerNode())
	{
		OutTypes.Add<bool>();
		OutTypes.Add<FVoxelBoolBuffer>();
	}
	if (GetByteInnerNode())
	{
		OutTypes.Add(GetByteTypes());
	}
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
	if (GetObjectInnerNode())
	{
		OutTypes.Add(GetObjectTypes());
	}

	return OutTypes;
}

void FVoxelTemplateNode_EqualityBase::PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType)
{
	Pin.SetType(NewType);
	ON_SCOPE_EXIT
	{
		ensure(Pin.GetType() == NewType);
	};

	FixupWildcards(NewType);

	if (IsObject(NewType))
	{
		GetPin(APin).SetType(NewType);
		GetPin(BPin).SetType(NewType);
	}
	else
	{
		if (Pin.Name != ResultPin)
		{
			if (IsBool(GetPin(APin).GetType()) != IsBool(NewType) ||
				IsByte(GetPin(APin).GetType()) != IsByte(NewType) ||
				IsObject(GetPin(APin).GetType()) != IsObject(NewType))
			{
				GetPin(APin).SetType(NewType);
			}
			if (IsBool(GetPin(BPin).GetType()) != IsBool(NewType) ||
				IsByte(GetPin(BPin).GetType()) != IsByte(NewType) ||
				IsObject(GetPin(BPin).GetType()) != IsObject(NewType))
			{
				GetPin(BPin).SetType(NewType);
			}

			if (!IsBool(NewType) &&
				!IsByte(NewType))
			{
				EnforceSameDimensions(Pin, NewType, { APin, BPin });
			}
		}
	}

	FixupBuffers(NewType, GetAllPins());
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelTemplateNode::FPin* FVoxelTemplateNode_MultiInputBooleanNode::ExpandPins(FNode& Node, TArray<FPin*> Pins, const TArray<FPin*>& AllPins) const
{
	return Reduce(Pins, [&](FPin* PinA, FPin* PinB)
	{
		return Call_Single(GetBooleanNode(), PinA, PinB);
	});
}

#if WITH_EDITOR
FVoxelPinTypeSet FVoxelTemplateNode_MultiInputBooleanNode::GetPromotionTypes(const FVoxelPin& Pin) const
{
	FVoxelPinTypeSet OutTypes;
	OutTypes.Add<bool>();
	OutTypes.Add<FVoxelBoolBuffer>();
	return OutTypes;
}

void FVoxelTemplateNode_MultiInputBooleanNode::PromotePin(FVoxelPin& InPin, const FVoxelPinType& NewType)
{
	for (FVoxelPin& Pin : GetPins())
	{
		Pin.SetType(NewType);
	}
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#if WITH_EDITOR
void FVoxelTemplateNode_MultiInputBooleanNode::FDefinition::AddInputPin()
{
	AddToCategory(Node.InputPins);

	const FVoxelPinType BoolType = Node.GetPin(Node.ResultPin).GetType();
	for (FVoxelPin& Pin : Node.GetPins())
	{
		Pin.SetType(BoolType);
	}
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#if WITH_EDITOR
FVoxelPinTypeSet FVoxelTemplateNode_EqualitySingleDimension::GetPromotionTypes(const FVoxelPin& Pin) const
{
	FVoxelPinTypeSet OutTypes;

	if (Pin.Name == ResultPin)
	{
		OutTypes.Add<bool>();
		OutTypes.Add<FVoxelBoolBuffer>();
		return OutTypes;
	}

	if (GetFloatInnerNode())
	{
		OutTypes.Add<float>();
		OutTypes.Add<FVoxelFloatBuffer>();
	}
	if (GetDoubleInnerNode())
	{
		OutTypes.Add<double>();
		OutTypes.Add<FVoxelDoubleBuffer>();
	}
	if (GetInt32InnerNode())
	{
		OutTypes.Add<int32>();
		OutTypes.Add<FVoxelInt32Buffer>();
	}

	return OutTypes;
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelTemplateNode::FPin* FVoxelTemplateNode_NearlyEqual::ExpandPins(FNode& Node, TArray<FPin*> Pins, const TArray<FPin*>& AllPins) const
{
	const int32 MaxDimension = GetMaxDimension(AllPins);

	const bool bIsDouble = Any(Pins, IsPinDouble);

	if (bIsDouble)
	{
		Pins = Apply(Pins, ConvertToDouble);
	}
	else
	{
		ensure(All(Pins, IsPinFloat));
	}

	Pins = Apply(Pins, ScalarToVector, MaxDimension);
	check(Pins.Num() == 3);

	const TArray<TArray<FPin*>> BrokenPins = ApplyVector(Pins, BreakVector);
	check(BrokenPins.Num() == 3);

	const TArray<FPin*> BooleanPins = bIsDouble
		? Call_Multi<FVoxelNode_NearlyEqual_Double>(BrokenPins)
		: Call_Multi<FVoxelNode_NearlyEqual_Float>(BrokenPins);

	return Reduce(BooleanPins, [&](FPin* PinA, FPin* PinB)
	{
		return Call_Single<FVoxelNode_BooleanAND>(PinA, PinB);
	});
}

#if WITH_EDITOR
FVoxelPinTypeSet FVoxelTemplateNode_NearlyEqual::GetPromotionTypes(const FVoxelPin& Pin) const
{
	if (Pin.Name == ErrorTolerancePin)
	{
		FVoxelPinTypeSet OutTypes;
		OutTypes.Add<float>();
		OutTypes.Add<double>();
		OutTypes.Add<FVoxelFloatBuffer>();
		OutTypes.Add<FVoxelDoubleBuffer>();
		return OutTypes;
	}

	if (Pin.Name == ResultPin)
	{
		FVoxelPinTypeSet OutTypes;
		OutTypes.Add<bool>();
		OutTypes.Add<FVoxelBoolBuffer>();
		return OutTypes;
	}

	FVoxelPinTypeSet OutTypes;
	OutTypes.Add(GetFloatTypes());
	OutTypes.Add(GetDoubleTypes());
	return OutTypes;
}

void FVoxelTemplateNode_NearlyEqual::PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType)
{
	Pin.SetType(NewType);
	ON_SCOPE_EXIT
	{
		ensure(Pin.GetType() == NewType);
	};

	FixupWildcards(NewType);
	EnforceSameDimensions(Pin, NewType, { APin, BPin });

	// Fixup ErrorTolerance
	SetPinDimension(GetPin(ErrorTolerancePin), 1);

	FixupBuffers(NewType, GetAllPins());
}
#endif