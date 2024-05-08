// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Templates/VoxelVectorNodes.h"
#include "Buffer/VoxelIntegerBuffers.h"

FVoxelTemplateNode::FPin* FVoxelTemplateNode_AbstractVectorBase::ExpandPins(FNode& Node, TArray<FPin*> Pins, const TArray<FPin*>& AllPins) const
{
	const int32 NumPins = Pins.Num();
	const int32 MaxDimension = GetMaxDimension(AllPins);
	const bool bIsDouble = Any(AllPins, IsPinDouble);

	if (bIsDouble)
	{
		Pins = Apply(Pins, ConvertToDouble);
	}
	else
	{
		Pins = Apply(Pins, ConvertToFloat);
	}

	Pins = Apply(Pins, ScalarToVector, MaxDimension);
	check(Pins.Num() == NumPins);

	return Call_Single(MaxDimension == 2
		? bIsDouble ? GetDoubleVector2DInnerNode() : GetVector2DInnerNode()
		: bIsDouble ? GetDoubleVector3DInnerNode() : GetVector3DInnerNode(),
		Pins);
}

#if WITH_EDITOR
FVoxelPinTypeSet FVoxelTemplateNode_AbstractVectorBase::GetPromotionTypes(const FVoxelPin& Pin) const
{
	if (Pin.Name == ReturnValuePin)
	{
		FVoxelPinTypeSet OutTypes;
		if (HasScalarOutput2D() ||
			HasScalarOutput3D())
		{
			OutTypes.Add(FVoxelPinType::Make<float>());
			OutTypes.Add(FVoxelPinType::Make<double>());
			OutTypes.Add(FVoxelPinType::Make<FVoxelFloatBuffer>());
			OutTypes.Add(FVoxelPinType::Make<FVoxelDoubleBuffer>());
		}
		if (!HasScalarOutput2D())
		{
			OutTypes.Add<FVector2D>();
			OutTypes.Add<FVoxelDoubleVector2D>();
			OutTypes.Add<FVoxelVector2DBuffer>();
			OutTypes.Add<FVoxelDoubleVector2DBuffer>();
		}
		if (!HasScalarOutput3D())
		{
			OutTypes.Add<FVector>();
			OutTypes.Add<FVoxelDoubleVector>();
			OutTypes.Add<FVoxelVectorBuffer>();
			OutTypes.Add<FVoxelDoubleVectorBuffer>();
		}
		return OutTypes;
	}

	FVoxelPinTypeSet OutTypes;
	OutTypes.Add<FVector2D>();
	OutTypes.Add<FVector>();
	OutTypes.Add<FVoxelVector2DBuffer>();
	OutTypes.Add<FVoxelVectorBuffer>();

	OutTypes.Add<FVoxelDoubleVector2D>();
	OutTypes.Add<FVoxelDoubleVector>();
	OutTypes.Add<FVoxelDoubleVector2DBuffer>();
	OutTypes.Add<FVoxelDoubleVectorBuffer>();

	OutTypes.Add<FIntPoint>();
	OutTypes.Add<FIntVector>();
	OutTypes.Add<FVoxelIntPointBuffer>();
	OutTypes.Add<FVoxelIntVectorBuffer>();
	return OutTypes;
}

void FVoxelTemplateNode_AbstractVectorBase::PromotePin(FVoxelPin& InPin, const FVoxelPinType& NewType)
{
	InPin.SetType(NewType);
	ON_SCOPE_EXIT
	{
		ensure(InPin.GetType() == NewType);
	};

	FixupWildcards(NewType);

	if (InPin.bIsInput)
	{
		ensure(
			GetDimension(NewType) == 2 ||
			GetDimension(NewType) == 3);

		for (FVoxelPin& Pin : GetPins())
		{
			if (Pin.bIsInput)
			{
				Pin.SetType(NewType);
			}
		}

		if (GetDimension(NewType) == 2)
		{
			if (HasScalarOutput2D())
			{
				SetPinDimension(GetUniqueOutputPin(), 1);
			}
			else
			{
				SetPinDimension(GetUniqueOutputPin(), 2);
			}
		}
		else
		{
			ensure(GetDimension(NewType) == 3);
			if (HasScalarOutput3D())
			{
				SetPinDimension(GetUniqueOutputPin(), 1);
			}
			else
			{
				SetPinDimension(GetUniqueOutputPin(), 3);
			}
		}

		bool bHasAnyDouble = false;
		for (const FVoxelPin& Pin : GetPins())
		{
			if (IsDouble(Pin.GetType()))
			{
				bHasAnyDouble = true;
			}
		}

		if (bHasAnyDouble)
		{
			SetPinScalarType<double>(GetUniqueOutputPin());
		}
	}
	else
	{
		// Setting output

		if (GetDimension(NewType) == 1)
		{
			if (!HasScalarOutput3D())
			{
				ensure(HasScalarOutput2D());
				// Force switch to 2D

				for (FVoxelPin& Pin : GetPins())
				{
					if (Pin.bIsInput)
					{
						SetPinDimension(Pin, 2);
					}
				}
			}
			else if (!HasScalarOutput2D())
			{
				ensure(HasScalarOutput3D());
				// Force switch to 3D

				for (FVoxelPin& Pin : GetPins())
				{
					if (Pin.bIsInput)
					{
						SetPinDimension(Pin, 3);
					}
				}
			}
			else
			{
				ensure(HasScalarOutput2D() && HasScalarOutput3D());
				// No need to fixup anything
			}
		}
		else if (GetDimension(NewType) == 2)
		{
			ensure(!HasScalarOutput2D());
			for (FVoxelPin& Pin : GetPins())
			{
				if (Pin.bIsInput)
				{
					SetPinDimension(Pin, 2);
				}
			}
		}
		else if (GetDimension(NewType) == 3)
		{
			ensure(!HasScalarOutput3D());
			for (FVoxelPin& Pin : GetPins())
			{
				if (Pin.bIsInput)
				{
					SetPinDimension(Pin, 3);
				}
			}
		}
		else
		{
			ensure(false);
		}
	}

	// Ensure inputs all have the same dimension
	const int32 Dimension = FMath::Max(2, GetMaxDimension(GetAllPins()));
	for (FVoxelPin& Pin : GetPins())
	{
		if (Pin.bIsInput)
		{
			SetPinDimension(Pin, Dimension);
		}
	}

	FixupBuffers(NewType, GetAllPins());
}
#endif