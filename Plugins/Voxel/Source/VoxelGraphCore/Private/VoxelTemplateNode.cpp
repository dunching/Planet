// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelTemplateNode.h"
#include "VoxelMathNodes.h"
#include "VoxelFunctionNode.h"
#include "VoxelObjectPinType.h"
#include "VoxelCompiledGraph.h"
#include "FunctionLibrary/VoxelMathFunctionLibrary.h"
#include "FunctionLibrary/VoxelAutocastFunctionLibrary.h"

FVoxelTemplateNodeContext* GVoxelTemplateNodeContext = nullptr;

void FVoxelTemplateNodeUtilities::SetPinDimension(FVoxelPin& Pin, const int32 Dimension)
{
	Pin.SetType(GetVectorType(Pin.GetType(), Dimension));
}

TConstVoxelArrayView<FVoxelPinType> FVoxelTemplateNodeUtilities::GetByteTypes()
{
	static TArray<FVoxelPinType> Types;
	if (Types.Num() == 0)
	{
		Types.Add(FVoxelPinType::Make<uint8>());
		Types.Add(FVoxelPinType::Make<FVoxelByteBuffer>());

		ForEachObjectOfClass<UEnum>([&](UEnum* Enum)
		{
			const FVoxelPinType Type = FVoxelPinType::MakeEnum(Enum);
			Types.Add(Type);
			Types.Add(Type.GetBufferType());
		});
	}
	return Types;
}

TConstVoxelArrayView<FVoxelPinType> FVoxelTemplateNodeUtilities::GetFloatTypes()
{
	static const TArray<FVoxelPinType> Types =
	{
		FVoxelPinType::Make<float>(),
		FVoxelPinType::Make<FVector2D>(),
		FVoxelPinType::Make<FVector>(),
		FVoxelPinType::Make<FLinearColor>(),

		FVoxelPinType::Make<FVoxelFloatBuffer>(),
		FVoxelPinType::Make<FVoxelVector2DBuffer>(),
		FVoxelPinType::Make<FVoxelVectorBuffer>(),
		FVoxelPinType::Make<FVoxelLinearColorBuffer>()
	};
	return Types;
}

TConstVoxelArrayView<FVoxelPinType> FVoxelTemplateNodeUtilities::GetDoubleTypes()
{
	static const TArray<FVoxelPinType> Types =
	{
		FVoxelPinType::Make<double>(),
		FVoxelPinType::Make<FVoxelDoubleVector2D>(),
		FVoxelPinType::Make<FVoxelDoubleVector>(),
		FVoxelPinType::Make<FVoxelDoubleLinearColor>(),

		FVoxelPinType::Make<FVoxelDoubleBuffer>(),
		FVoxelPinType::Make<FVoxelDoubleVector2DBuffer>(),
		FVoxelPinType::Make<FVoxelDoubleVectorBuffer>(),
		FVoxelPinType::Make<FVoxelDoubleLinearColorBuffer>()
	};
	return Types;
}

TConstVoxelArrayView<FVoxelPinType> FVoxelTemplateNodeUtilities::GetIntTypes()
{
	static const TArray<FVoxelPinType> Types =
	{
		FVoxelPinType::Make<int32>(),
		FVoxelPinType::Make<FIntPoint>(),
		FVoxelPinType::Make<FIntVector>(),
		FVoxelPinType::Make<FIntVector4>(),

		FVoxelPinType::Make<FVoxelInt32Buffer>(),
		FVoxelPinType::Make<FVoxelIntPointBuffer>(),
		FVoxelPinType::Make<FVoxelIntVectorBuffer>(),
		FVoxelPinType::Make<FVoxelIntVector4Buffer>()
	};
	return Types;
}

TConstVoxelArrayView<FVoxelPinType> FVoxelTemplateNodeUtilities::GetObjectTypes()
{
	static TArray<FVoxelPinType> Types;
	if (Types.Num() == 0)
	{
		for (const auto& It : FVoxelObjectPinType::StructToPinType())
		{
			FVoxelPinType Type = FVoxelPinType::MakeStruct(ConstCast(It.Key));
			Types.Add(Type.GetInnerType());
			Types.Add(Type.GetBufferType().WithBufferArray(false));
		}
	}
	return Types;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool FVoxelTemplateNodeUtilities::IsPinBool(const FPin* Pin)
{
	return IsBool(Pin->Type);
}

bool FVoxelTemplateNodeUtilities::IsPinByte(const FPin* Pin)
{
	return IsByte(Pin->Type);
}

bool FVoxelTemplateNodeUtilities::IsPinFloat(const FPin* Pin)
{
	return IsFloat(Pin->Type);
}

bool FVoxelTemplateNodeUtilities::IsPinDouble(const FPin* Pin)
{
	return IsDouble(Pin->Type);
}

bool FVoxelTemplateNodeUtilities::IsPinInt(const FPin* Pin)
{
	return IsInt(Pin->Type);
}

bool FVoxelTemplateNodeUtilities::IsPinObject(const FPin* Pin)
{
	return IsObject(Pin->Type);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int32 FVoxelTemplateNodeUtilities::GetDimension(const FVoxelPinType& PinType)
{
	if (PinType.IsBuffer())
	{
		return GetDimension(PinType.GetInnerType());
	}

	if (PinType.Is<float>() ||
		PinType.Is<double>() ||
		PinType.Is<int32>() ||
		PinType.Is<bool>() ||
		PinType.Is<uint8>())
	{
		return 1;
	}

	if (PinType.Is<FVector2D>() ||
		PinType.Is<FVoxelDoubleVector2D>() ||
		PinType.Is<FIntPoint>())
	{
		return 2;
	}

	if (PinType.Is<FVector>() ||
		PinType.Is<FVoxelDoubleVector>() ||
		PinType.Is<FIntVector>())
	{
		return 3;
	}

	if (PinType.Is<FLinearColor>() ||
		PinType.Is<FVoxelDoubleLinearColor>() ||
		PinType.Is<FIntVector4>())
	{
		return 4;
	}

	if (PinType.GetInnerExposedType().IsObject())
	{
		return 1;
	}

	ensure(false);
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelPinType FVoxelTemplateNodeUtilities::GetVectorType(const FVoxelPinType& PinType, const int32 Dimension)
{
	if (PinType.IsBuffer())
	{
		const FVoxelPinType Type = GetVectorType(PinType.GetInnerType(), Dimension);
		return Type.GetBufferType();
	}

	if (PinType.Is<float>() ||
		PinType.Is<FVector2D>() ||
		PinType.Is<FVector>() ||
		PinType.Is<FLinearColor>())
	{
		switch (Dimension)
		{
		default: ensure(false);
		case 1: return FVoxelPinType::Make<float>();
		case 2: return FVoxelPinType::Make<FVector2D>();
		case 3: return FVoxelPinType::Make<FVector>();
		case 4: return FVoxelPinType::Make<FLinearColor>();
		}
	}

	if (PinType.Is<double>() ||
		PinType.Is<FVoxelDoubleVector2D>() ||
		PinType.Is<FVoxelDoubleVector>() ||
		PinType.Is<FVoxelDoubleLinearColor>())
	{
		switch (Dimension)
		{
		default: ensure(false);
		case 1: return FVoxelPinType::Make<double>();
		case 2: return FVoxelPinType::Make<FVoxelDoubleVector2D>();
		case 3: return FVoxelPinType::Make<FVoxelDoubleVector>();
		case 4: return FVoxelPinType::Make<FVoxelDoubleLinearColor>();
		}
	}

	if (PinType.Is<int32>() ||
		PinType.Is<FIntPoint>() ||
		PinType.Is<FIntVector>() ||
		PinType.Is<FIntVector4>())
	{
		switch (Dimension)
		{
		default: ensure(false);
		case 1: return FVoxelPinType::Make<int32>();
		case 2: return FVoxelPinType::Make<FIntPoint>();
		case 3: return FVoxelPinType::Make<FIntVector>();
		case 4: return FVoxelPinType::Make<FIntVector4>();
		}
	}

	ensure(false);
	return PinType;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedPtr<FVoxelNode> FVoxelTemplateNodeUtilities::GetBreakNode(const FVoxelPinType& PinType)
{
	if (PinType.IsBuffer())
	{
		TSharedPtr<FVoxelNode> Node = GetBreakNode(PinType.GetInnerType());
		if (!ensure(Node))
		{
			return nullptr;
		}
		Node->PromotePin_Runtime(Node->GetUniqueInputPin(), PinType);
		return Node;
	}

	if (PinType.Is<FVector2D>())
	{
		return FVoxelFunctionNode::Make(FindUFunctionChecked(UVoxelMathFunctionLibrary, BreakVector2D));
	}
	else if (PinType.Is<FVector>())
	{
		return FVoxelFunctionNode::Make(FindUFunctionChecked(UVoxelMathFunctionLibrary, BreakVector));
	}
	else if (PinType.Is<FLinearColor>())
	{
		return FVoxelFunctionNode::Make(FindUFunctionChecked(UVoxelMathFunctionLibrary, BreakLinearColor));
	}

	if (PinType.Is<FVoxelDoubleVector2D>())
	{
		return FVoxelFunctionNode::Make(FindUFunctionChecked(UVoxelMathFunctionLibrary, BreakDoubleVector2D));
	}
	else if (PinType.Is<FVoxelDoubleVector>())
	{
		return FVoxelFunctionNode::Make(FindUFunctionChecked(UVoxelMathFunctionLibrary, BreakDoubleVector));
	}
	else if (PinType.Is<FVoxelDoubleLinearColor>())
	{
		return FVoxelFunctionNode::Make(FindUFunctionChecked(UVoxelMathFunctionLibrary, BreakDoubleLinearColor));
	}

	if (PinType.Is<FIntPoint>())
	{
		return FVoxelFunctionNode::Make(FindUFunctionChecked(UVoxelMathFunctionLibrary, BreakIntPoint));
	}
	else if (PinType.Is<FIntVector>())
	{
		return FVoxelFunctionNode::Make(FindUFunctionChecked(UVoxelMathFunctionLibrary, BreakIntVector));
	}
	else if (PinType.Is<FIntVector4>())
	{
		return FVoxelFunctionNode::Make(FindUFunctionChecked(UVoxelMathFunctionLibrary, BreakIntVector4));
	}

	ensure(false);
	return nullptr;
}

TSharedPtr<FVoxelNode> FVoxelTemplateNodeUtilities::GetMakeNode(const FVoxelPinType& PinType)
{
	if (PinType.IsBuffer())
	{
		TSharedPtr<FVoxelNode> Node = GetMakeNode(PinType.GetInnerType());
		if (!ensure(Node))
		{
			return nullptr;
		}
		Node->PromotePin_Runtime(Node->GetUniqueOutputPin(), PinType);
		return Node;
	}

	if (PinType.Is<FVector2D>())
	{
		return FVoxelFunctionNode::Make(FindUFunctionChecked(UVoxelMathFunctionLibrary, MakeVector2D));
	}
	else if (PinType.Is<FVector>())
	{
		return FVoxelFunctionNode::Make(FindUFunctionChecked(UVoxelMathFunctionLibrary, MakeVector));
	}
	else if (PinType.Is<FLinearColor>())
	{
		return FVoxelFunctionNode::Make(FindUFunctionChecked(UVoxelMathFunctionLibrary, MakeLinearColor));
	}

	if (PinType.Is<FVoxelDoubleVector2D>())
	{
		return FVoxelFunctionNode::Make(FindUFunctionChecked(UVoxelMathFunctionLibrary, MakeDoubleVector2D));
	}
	else if (PinType.Is<FVoxelDoubleVector>())
	{
		return FVoxelFunctionNode::Make(FindUFunctionChecked(UVoxelMathFunctionLibrary, MakeDoubleVector));
	}
	else if (PinType.Is<FVoxelDoubleLinearColor>())
	{
		return FVoxelFunctionNode::Make(FindUFunctionChecked(UVoxelMathFunctionLibrary, MakeDoubleLinearColor));
	}

	if (PinType.Is<FIntPoint>())
	{
		return FVoxelFunctionNode::Make(FindUFunctionChecked(UVoxelMathFunctionLibrary, MakeIntPoint));
	}
	else if (PinType.Is<FIntVector>())
	{
		return FVoxelFunctionNode::Make(FindUFunctionChecked(UVoxelMathFunctionLibrary, MakeIntVector));
	}
	else if (PinType.Is<FIntVector4>())
	{
		return FVoxelFunctionNode::Make(FindUFunctionChecked(UVoxelMathFunctionLibrary, MakeIntVector4));
	}

	ensure(false);
	return nullptr;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedPtr<FVoxelNode> FVoxelTemplateNodeUtilities::GetConvertToFloatNode(const FPin* Pin)
{
	const int32 Dimension = GetDimension(Pin->Type);
	if (IsPinInt(Pin))
	{
		switch (Dimension)
		{
		default: ensure(false);
		case 1:
		{
			return FVoxelFunctionNode::Make(FindUFunctionChecked(UVoxelAutocastFunctionLibrary, IntegerToFloat));
		}
		case 2:
		{
			return FVoxelFunctionNode::Make(FindUFunctionChecked(UVoxelAutocastFunctionLibrary, IntPointToVector2D));
		}
		case 3:
		{
			return FVoxelFunctionNode::Make(FindUFunctionChecked(UVoxelAutocastFunctionLibrary, IntVectorToVector));
		}
		case 4:
		{
			return FVoxelFunctionNode::Make(FindUFunctionChecked(UVoxelAutocastFunctionLibrary, IntVector4ToLinearColor));
		}
		}
	}
	else if (IsPinDouble(Pin))
	{
		switch (Dimension)
		{
		default: ensure(false);
		case 1:
		{
			return FVoxelFunctionNode::Make(FindUFunctionChecked(UVoxelAutocastFunctionLibrary, DoubleToFloat));
		}
		case 2:
		{
			return FVoxelFunctionNode::Make(FindUFunctionChecked(UVoxelAutocastFunctionLibrary, DoubleVector2DToVector2D));
		}
		case 3:
		{
			return FVoxelFunctionNode::Make(FindUFunctionChecked(UVoxelAutocastFunctionLibrary, DoubleVectorToVector));
		}
		case 4:
		{
			return FVoxelFunctionNode::Make(FindUFunctionChecked(UVoxelAutocastFunctionLibrary, DoubleLinearColorToLinearColor));
		}
		}
	}
	else
	{
		ensure(false);
		return nullptr;
	}
}

TSharedPtr<FVoxelNode> FVoxelTemplateNodeUtilities::GetConvertToDoubleNode(const FPin* Pin)
{
	const int32 Dimension = GetDimension(Pin->Type);

	if (IsPinInt(Pin))
	{
		switch (Dimension)
		{
		default: ensure(false);
		case 1:
		{
			return FVoxelFunctionNode::Make(FindUFunctionChecked(UVoxelAutocastFunctionLibrary, IntegerToDouble));
		}
		case 2:
		{
			return FVoxelFunctionNode::Make(FindUFunctionChecked(UVoxelAutocastFunctionLibrary, IntPointToDoubleVector2D));
		}
		case 3:
		{
			return FVoxelFunctionNode::Make(FindUFunctionChecked(UVoxelAutocastFunctionLibrary, IntVectorToDoubleVector));
		}
		case 4:
		{
			return FVoxelFunctionNode::Make(FindUFunctionChecked(UVoxelAutocastFunctionLibrary, IntVector4ToDoubleLinearColor));
		}
		}
	}
	else if (IsPinFloat(Pin))
	{
		switch (Dimension)
		{
		default: ensure(false);
		case 1:
		{
			return FVoxelFunctionNode::Make(FindUFunctionChecked(UVoxelAutocastFunctionLibrary, FloatToDouble));
		}
		case 2:
		{
			return FVoxelFunctionNode::Make(FindUFunctionChecked(UVoxelAutocastFunctionLibrary, Vector2DToDoubleVector2D));
		}
		case 3:
		{
			return FVoxelFunctionNode::Make(FindUFunctionChecked(UVoxelAutocastFunctionLibrary, VectorToDoubleVector));
		}
		case 4:
		{
			return FVoxelFunctionNode::Make(FindUFunctionChecked(UVoxelAutocastFunctionLibrary, LinearColorToDoubleLinearColor));
		}
		}
	}
	else
	{
		ensure(false);
		return nullptr;
	}
}

TSharedPtr<FVoxelNode> FVoxelTemplateNodeUtilities::GetMakeNode(const FPin* Pin, const int32 Dimension)
{
	ensure(Dimension > 1);

	if (IsPinFloat(Pin))
	{
		switch (Dimension)
		{
		default: ensure(false);
		case 2:
		{
			return FVoxelFunctionNode::Make(FindUFunctionChecked(UVoxelMathFunctionLibrary, MakeVector2D));
		}
		case 3:
		{
			return FVoxelFunctionNode::Make(FindUFunctionChecked(UVoxelMathFunctionLibrary, MakeVector));
		}
		case 4:
		{
			return FVoxelFunctionNode::Make(FindUFunctionChecked(UVoxelMathFunctionLibrary, MakeLinearColor));
		}
		}
	}
	else if (IsPinDouble(Pin))
	{
		switch (Dimension)
		{
		default: ensure(false);
		case 2:
		{
			return FVoxelFunctionNode::Make(FindUFunctionChecked(UVoxelMathFunctionLibrary, MakeDoubleVector2D));
		}
		case 3:
		{
			return FVoxelFunctionNode::Make(FindUFunctionChecked(UVoxelMathFunctionLibrary, MakeDoubleVector));
		}
		case 4:
		{
			return FVoxelFunctionNode::Make(FindUFunctionChecked(UVoxelMathFunctionLibrary, MakeDoubleLinearColor));
		}
		}
	}
	else if (IsPinInt(Pin))
	{
		switch (Dimension)
		{
		default: ensure(false);
		case 2:
		{
			return FVoxelFunctionNode::Make(FindUFunctionChecked(UVoxelMathFunctionLibrary, MakeIntPoint));
		}
		case 3:
		{
			return FVoxelFunctionNode::Make(FindUFunctionChecked(UVoxelMathFunctionLibrary, MakeIntVector));
		}
		case 4:
		{
			return FVoxelFunctionNode::Make(FindUFunctionChecked(UVoxelMathFunctionLibrary, MakeIntVector4));
		}
		}
	}
	else
	{
		ensure(false);
		return nullptr;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int32 FVoxelTemplateNodeUtilities::GetMaxDimension(const TArray<FPin*>& Pins)
{
	int32 HighestDimension = 0;
	for (const FPin* Pin : Pins)
	{
		HighestDimension = FMath::Max(HighestDimension, GetDimension(Pin->Type));
	}

	return HighestDimension;
}

FVoxelTemplateNodeUtilities::FPin* FVoxelTemplateNodeUtilities::GetLinkedPin(FPin* Pin)
{
	ensure(Pin->GetLinkedTo().Num() == 1);
	return &Pin->GetLinkedTo()[0];
}

FVoxelTemplateNodeUtilities::FPin* FVoxelTemplateNodeUtilities::ConvertToByte(FPin* Pin)
{
	ensure(IsPinByte(Pin));

	if (!Pin->Type.GetInnerType().GetEnum())
	{
		return Pin;
	}

	FVoxelPinType OutputType = FVoxelPinType::Make<uint8>();
	if (Pin->Type.IsBuffer())
	{
		OutputType = OutputType.GetBufferType();
	}

	FNode& Passthrough = Pin->Node.Graph.NewNode(ENodeType::Passthrough, GetNodeRef());
	FPin& InputPin = Passthrough.NewInputPin("ConvertToByteIn", Pin->Type, FVoxelPinValue(Pin->Type));
	FPin& OutputPin = Passthrough.NewOutputPin("ConvertToByteOut", OutputType);

	Pin->MakeLinkTo(InputPin);

	return &OutputPin;
}

FVoxelTemplateNodeUtilities::FPin* FVoxelTemplateNodeUtilities::ConvertToFloat(FPin* Pin)
{

	if (IsPinObject(Pin))
	{
		return Pin;
	}

	if (IsPinFloat(Pin))
	{
		return Pin;
	}

	if (!ensure(IsPinInt(Pin) || IsPinDouble(Pin)))
	{
		return Pin;
	}

	const TSharedPtr<FVoxelNode> VoxelNode = GetConvertToFloatNode(Pin);
	if (!ensure(VoxelNode))
	{
		return Pin;
	}

	FVoxelPinType ResultType = GetVectorType(FVoxelPinType::Make<float>(), GetDimension(Pin->Type));
	if (Pin->Type.IsBuffer())
	{
		ResultType = ResultType.GetBufferType();
	}

	FNode& ConvNode = Pin->Node.Graph.NewNode(GetNodeRef());
	ConvNode.SetVoxelNode(VoxelNode.ToSharedRef());

	FPin* ResultPin = nullptr;
	for (const FVoxelPin& ConvNodePin : ConvNode.GetVoxelNode().GetPins())
	{
		if (ConvNodePin.bIsInput)
		{
			FPin& InputPin = ConvNode.NewInputPin(ConvNodePin.Name, Pin->Type);
			Pin->MakeLinkTo(InputPin);
		}
		else
		{
			FPin& OutputPin = ConvNode.NewOutputPin(ConvNodePin.Name, ResultType);
			ResultPin = &OutputPin;
		}
	}
	return ResultPin;
}

FVoxelTemplateNodeUtilities::FPin* FVoxelTemplateNodeUtilities::ConvertToDouble(FPin* Pin)
{
	if (IsPinDouble(Pin))
	{
		return Pin;
	}

	if (!ensure(IsPinInt(Pin) || IsPinFloat(Pin)))
	{
		return Pin;
	}

	const TSharedPtr<FVoxelNode> VoxelNode = GetConvertToDoubleNode(Pin);
	if (!ensure(VoxelNode))
	{
		return Pin;
	}

	FVoxelPinType ResultType = GetVectorType(FVoxelPinType::Make<double>(), GetDimension(Pin->Type));
	if (Pin->Type.IsBuffer())
	{
		ResultType = ResultType.GetBufferType();
	}

	FNode& ConvNode = Pin->Node.Graph.NewNode(GetNodeRef());
	ConvNode.SetVoxelNode(VoxelNode.ToSharedRef());

	FPin* ResultPin = nullptr;
	for (const FVoxelPin& ConvNodePin : ConvNode.GetVoxelNode().GetPins())
	{
		if (ConvNodePin.bIsInput)
		{
			FPin& InputPin = ConvNode.NewInputPin(ConvNodePin.Name, Pin->Type);
			Pin->MakeLinkTo(InputPin);
		}
		else
		{
			FPin& OutputPin = ConvNode.NewOutputPin(ConvNodePin.Name, ResultType);
			ResultPin = &OutputPin;
		}
	}
	return ResultPin;
}

FVoxelTemplateNodeUtilities::FPin* FVoxelTemplateNodeUtilities::ScalarToVector(FPin* Pin, const int32 HighestDimension)
{
	if (HighestDimension == 1 ||
		GetDimension(Pin->Type) != 1)
	{
		return Pin;
	}

	const TSharedPtr<FVoxelNode> VoxelNode = GetMakeNode(Pin, HighestDimension);
	if (!ensure(VoxelNode))
	{
		return Pin;
	}

	FNode& MakeNode = CreateNode(Pin, VoxelNode.ToSharedRef());
	ensure(MakeNode.GetInputPins().Num() == HighestDimension);

	for (FPin& MakeNodePin : MakeNode.GetInputPins())
	{
		Pin->MakeLinkTo(MakeNodePin);
	}

	return &MakeNode.GetOutputPin(0);
}

FVoxelTemplateNodeUtilities::FPin* FVoxelTemplateNodeUtilities::ZeroExpandVector(FPin* Pin, const int32 HighestDimension)
{
	const int32 PinDimension = GetDimension(Pin->Type);

	if (PinDimension == HighestDimension)
	{
		return Pin;
	}

	const TSharedPtr<FVoxelNode> VoxelBreakNode = GetBreakNode(Pin->Type);
	if (!ensure(VoxelBreakNode))
	{
		return Pin;
	}

	const TSharedPtr<FVoxelNode> VoxelMakeNode = GetMakeNode(Pin, HighestDimension);
	if (!ensure(VoxelMakeNode))
	{
		return Pin;
	}

	TArray<FPin*> ScalarPins;
	{
		FNode& BreakNode = CreateNode(Pin, VoxelBreakNode.ToSharedRef());
		ensure(BreakNode.GetOutputPins().Num() == PinDimension);

		Pin->MakeLinkTo(BreakNode.GetInputPin(0));

		for (FPin& BreakNodePin : BreakNode.GetOutputPins())
		{
			ScalarPins.Add(&BreakNodePin);
		}
	}

	FNode& Passthrough = Pin->Node.Graph.NewNode(ENodeType::Passthrough, GetNodeRef());
	Passthrough.NewInputPin("ZeroScalarInput", ScalarPins[0]->Type, FVoxelPinValue(ScalarPins[0]->Type));
	FPin* ZeroScalarPin = &Passthrough.NewOutputPin("ZeroScalarOutput", ScalarPins[0]->Type);

	FNode& MakeNode = CreateNode(Pin, VoxelMakeNode.ToSharedRef());
	ensure(MakeNode.GetInputPins().Num() == HighestDimension);

	for (int32 Index = 0; Index < MakeNode.GetInputPins().Num(); Index++)
	{
		if (ScalarPins.IsValidIndex(Index))
		{
			ScalarPins[Index]->MakeLinkTo(MakeNode.GetInputPin(Index));
		}
		else
		{
			ZeroScalarPin->MakeLinkTo(MakeNode.GetInputPin(Index));
		}
	}

	return &MakeNode.GetOutputPin(0);
}

TArray<FVoxelTemplateNodeUtilities::FPin*> FVoxelTemplateNodeUtilities::BreakVector(FPin* Pin)
{
	const int32 PinDimension = GetDimension(Pin->Type);
	if (PinDimension == 1)
	{
		return { Pin };
	}
	ensure(PinDimension > 1);

	return BreakNode(Pin, GetBreakNode(Pin->Type), PinDimension);
}

TArray<FVoxelTemplateNodeUtilities::FPin*> FVoxelTemplateNodeUtilities::BreakNode(FPin* Pin, const TSharedPtr<FVoxelNode>& BreakVoxelNode, int32 NumExpectedOutputPins)
{
	if (!ensure(BreakVoxelNode))
	{
		return {};
	}

	TArray<FPin*> ResultPins;

	if (Pin->Type.IsBuffer())
	{
		BreakVoxelNode->PromotePin_Runtime(BreakVoxelNode->GetUniqueInputPin(), Pin->Type);
	}

	FNode& BreakNode = CreateNode(Pin, BreakVoxelNode.ToSharedRef());
	ensure(BreakNode.GetPins().Num() == NumExpectedOutputPins + 1);

	Pin->MakeLinkTo(BreakNode.GetInputPin(0));

	for (FPin& BreakNodePin : BreakNode.GetOutputPins())
	{
		ResultPins.Add(&BreakNodePin);
	}

	return ResultPins;
}

FVoxelTemplateNodeUtilities::FPin* FVoxelTemplateNodeUtilities::MakeVector(TArray<FPin*> Pins)
{
	if (Pins.Num() == 1)
	{
		return Pins[0];
	}
	ensure(Pins.Num() > 1);

	const TSharedPtr<FVoxelNode> VoxelNode = GetMakeNode(Pins[0], Pins.Num());
	if (!ensure(VoxelNode))
	{
		return {};
	}

	FNode& MakeNode = CreateNode(Pins[0], VoxelNode.ToSharedRef());
	ensure(MakeNode.GetInputPins().Num() == Pins.Num());

	for (int32 Index = 0; Index < Pins.Num(); Index++)
	{
		Pins[Index]->MakeLinkTo(MakeNode.GetInputPin(Index));
	}

	return &MakeNode.GetOutputPin(0);
}

bool FVoxelTemplateNodeUtilities::IsPinOfName(const FPin* Pin, const TSet<FName> Names)
{
	return Names.Contains(Pin->Name);
}

FVoxelTemplateNodeUtilities::FPin* FVoxelTemplateNodeUtilities::MakeConstant(const FNode& Node, const FVoxelPinValue& Value)
{
	FNode& Passthrough = Node.Graph.NewNode(ENodeType::Passthrough, GetNodeRef());
	Passthrough.NewInputPin("ConstantInput", Value.GetType(), Value);
	return &Passthrough.NewOutputPin("ConstantOutput", Value.GetType());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelTemplateNodeUtilities::FNode& FVoxelTemplateNodeUtilities::CreateNode(const FPin* Pin, const TSharedRef<FVoxelNode>& VoxelNode)
{
	const FNode& Node = Pin->Node;

	FNode& StructNode = Node.Graph.NewNode(GetNodeRef());

	for (FVoxelPin& StructPin : VoxelNode->GetPins())
	{
		if (StructPin.IsPromotable() ||
			EnumHasAllFlags(StructPin.Flags, EVoxelPinFlags::TemplatePin))
		{
			if (Pin->Type.IsBuffer())
			{
				StructPin.SetType(StructPin.GetType().GetBufferType());
			}
			else
			{
				StructPin.SetType(StructPin.GetType().GetInnerType());
			}
		}

		if (StructPin.bIsInput)
		{
			StructNode.NewInputPin(StructPin.Name, StructPin.GetType());
		}
		else
		{
			StructNode.NewOutputPin(StructPin.Name, StructPin.GetType());
		}
	}

	StructNode.SetVoxelNode(VoxelNode);
	return StructNode;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool FVoxelTemplateNodeUtilities::Any(const TArray<FPin*>& Pins, const TFunctionRef<bool(FPin*)> Lambda)
{
	for (FPin* Pin : Pins)
	{
		if (Lambda(Pin))
		{
			return true;
		}
	}

	return false;
}

bool FVoxelTemplateNodeUtilities::All(const TArray<FPin*>& Pins, const TFunctionRef<bool(FPin*)> Lambda)
{
	for (FPin* Pin : Pins)
	{
		if (!Lambda(Pin))
		{
			return false;
		}
	}

	return true;
}

TArray<FVoxelTemplateNodeUtilities::FPin*> FVoxelTemplateNodeUtilities::Filter(const TArray<FPin*>& Pins, const TFunctionRef<bool(const FPin*)> Lambda)
{
	TArray<FPin*> Result;
	for (FPin* Pin : Pins)
	{
		if (Lambda(Pin))
		{
			Result.Add(Pin);
		}
	}

	return Result;
}

FVoxelTemplateNodeUtilities::FPin* FVoxelTemplateNodeUtilities::Reduce(TArray<FPin*> Pins, TFunctionRef<FPin*(FPin*, FPin*)> Lambda)
{
	ensure(Pins.Num() > 0);

	while (Pins.Num() > 1)
	{
		FPin* PinB = Pins.Pop(false);
		FPin* PinA = Pins.Pop(false);
		Pins.Add(Lambda(PinA, PinB));
	}

	return Pins[0];
}

FVoxelTemplateNodeUtilities::FPin* FVoxelTemplateNodeUtilities::Call_Single(
	const UScriptStruct* NodeStruct,
	const TArray<FPin*>& Pins,
	const TOptional<FVoxelPinType> OutputPinType)
{
	if (!ensure(Pins.Num() > 0))
	{
		return nullptr;
	}

	TArray<TArray<FPin*>> MultiPins;
	for (auto Pin : Pins)
	{
		MultiPins.Add({ Pin });
	}

	TArray<FPin*> OutputPins = Call_Multi(NodeStruct, MultiPins, OutputPinType);

	if (!ensure(OutputPins.Num() == 1))
	{
		return nullptr;
	}

	return OutputPins[0];
}

TArray<FVoxelTemplateNodeUtilities::FPin*> FVoxelTemplateNodeUtilities::Call_Multi(
	const UScriptStruct* NodeStruct,
	const TArray<TArray<FPin*>>& Pins,
	const TOptional<FVoxelPinType> OutputPinType)
{
	int32 NumDimensions = -1;

	const FNode* TargetNode = nullptr;
	for (const TArray<FPin*>& PinsArray : Pins)
	{
		if (NumDimensions == -1)
		{
			NumDimensions = PinsArray.Num();
		}
		else
		{
			ensure(NumDimensions == PinsArray.Num());
		}

		for (const FPin* Pin : PinsArray)
		{
			if (!TargetNode)
			{
				TargetNode = &Pin->Node;
				break;
			}
		}
	}

	if (!ensure(TargetNode) ||
		!ensure(NumDimensions != -1))
	{
		return {};
	}

	TArray<FPin*> ResultPins;

	for (int32 DimensionIndex = 0; DimensionIndex < NumDimensions; DimensionIndex++)
	{
		FNode& StructNode = TargetNode->Graph.NewNode(GetNodeRef());
		const TSharedRef<FVoxelNode> VoxelNode = MakeSharedStruct<FVoxelNode>(NodeStruct);

		int32 PinIndex = 0;
		for (FVoxelPin& Pin : VoxelNode->GetPins())
		{
			if (!Pin.bIsInput)
			{
				FVoxelPinType Type = Pin.GetType();
				if (Type.IsWildcard() &&
					ensure(OutputPinType.IsSet()))
				{
					Type = OutputPinType.GetValue();
					VoxelNode->PromotePin_Runtime(Pin, Type);
				}

				ResultPins.Add(&StructNode.NewOutputPin(Pin.Name, Type));
				continue;
			}

			FPin* TargetPin = nullptr;
			if (ensure(Pins.IsValidIndex(PinIndex) &&
				Pins[PinIndex].IsValidIndex(DimensionIndex)))
			{
				TargetPin = Pins[PinIndex++][DimensionIndex];

				if (Pin.GetType().IsWildcard() ||
					EnumHasAllFlags(Pin.Flags, EVoxelPinFlags::TemplatePin))
				{
					VoxelNode->PromotePin_Runtime(Pin, TargetPin->Type);
				}
			}

			FPin& NewInputPin = StructNode.NewInputPin(Pin.Name, TargetPin ? TargetPin->Type : Pin.GetType());

			if (TargetPin)
			{
				TargetPin->MakeLinkTo(NewInputPin);
			}
		}

		StructNode.SetVoxelNode(VoxelNode);
		StructNode.Check();
	}

	return ResultPins;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelTemplateNode::ExpandNode(FGraph& Graph, FNode& Node) const
{
	TArray<FPin*> InputPins = Node.GetInputPins().Array();
	InputPins = Apply(InputPins, &GetLinkedPin);

	TArray<FPin*> Pins = InputPins;
	Pins.Append(Node.GetOutputPins().Array());

	TArray<FPin*> OutputPins;
	ExpandPins(Node, InputPins, Pins, OutputPins);

	ensure(Node.GetOutputPins().Num() == OutputPins.Num());

	for (int32 Index = 0; Index < OutputPins.Num(); Index++)
	{
		const FPin& SourceOutputPin = Node.GetOutputPin(Index);
		FPin& TargetOutputPin = *OutputPins[Index];

		if (!ensure(SourceOutputPin.Type == TargetOutputPin.Type))
		{
			VOXEL_MESSAGE(Error, "{0}: internal error when expanding template node", Node);
			return;
		}

		SourceOutputPin.CopyOutputPinTo(TargetOutputPin);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TVoxelArray<FVoxelPinRef> FVoxelTemplateNode::GetAllPins() const
{
	TVoxelArray<FVoxelPinRef> AllPins;
	for (const FVoxelPin& PinIt : GetPins())
	{
		AllPins.Add(FVoxelPinRef(PinIt.Name));
	}
	return AllPins;
}

int32 FVoxelTemplateNode::GetMaxDimension(const TVoxelArray<FVoxelPinRef>& Pins) const
{
	int32 MaxTypeDimension = 0;
	for (const FVoxelPinRef& PinRef : Pins)
	{
		const FVoxelPin& Pin = GetPin(PinRef);
		MaxTypeDimension = FMath::Max(MaxTypeDimension, GetDimension(Pin.GetType()));
	}
	return MaxTypeDimension;
}

void FVoxelTemplateNode::FixupWildcards(const FVoxelPinType& NewType)
{
	for (FVoxelPin& Pin : GetPins())
	{
		if (Pin.GetType().IsWildcard())
		{
			Pin.SetType(NewType);
		}
	}
}

void FVoxelTemplateNode::FixupBuffers(
	const FVoxelPinType& NewType,
	const TVoxelArray<FVoxelPinRef>& Pins)
{
	for (const FVoxelPinRef& PinRef : Pins)
	{
		FVoxelPin& Pin = GetPin(PinRef);
		Pin.SetType(NewType.IsBuffer() ? Pin.GetType().GetBufferType() : Pin.GetType().GetInnerType());
	}
}

void FVoxelTemplateNode::EnforceNoPrecisionLoss(
	const FVoxelPin& InPin,
	const FVoxelPinType& NewType,
	const TVoxelArray<FVoxelPinRef>& Pins)
{
	ON_SCOPE_EXIT
	{
		ensure(GetDimension(GetUniqueOutputPin().GetType()) == GetMaxDimension(Pins));
	};

	if (InPin.bIsInput)
	{
		FVoxelPin& OutputPin = GetUniqueOutputPin();

		// Output needs to have a dimension same or equal as max input
		SetPinDimension(OutputPin, GetMaxDimension(Pins));

		bool bAnyFloat = false;
		bool bAnyDouble = false;
		for (const FVoxelPinRef& PinRef : Pins)
		{
			FVoxelPin& Pin = GetPin(PinRef);
			if (!Pin.bIsInput)
			{
				continue;
			}

			bAnyFloat |= IsFloat(Pin.GetType());
			bAnyDouble |= IsDouble(Pin.GetType());
		}

		if (bAnyDouble)
		{
			// If we have a double input, output needs to be double
			SetPinScalarType<double>(OutputPin);
		}
		else if (bAnyFloat)
		{
			// If we have a float input, output needs to be float
			SetPinScalarType<float>(OutputPin);
		}

		return;
	}

	// We are setting the output type

	// Reduce the dimension of inputs if they're too big
	const int32 NewDimension = GetDimension(NewType);
	for (const FVoxelPinRef& PinRef : Pins)
	{
		FVoxelPin& Pin = GetPin(PinRef);
		if (GetDimension(Pin.GetType()) > NewDimension)
		{
			SetPinDimension(Pin, NewDimension);
		}
	}

	// Convert double to float
	if (IsFloat(NewType))
	{
		for (const FVoxelPinRef& PinRef : Pins)
		{
			FVoxelPin& Pin = GetPin(PinRef);
			if (IsDouble(Pin.GetType()))
			{
				SetPinScalarType<float>(Pin);
			}
		}
	}

	// Convert float & double to int
	if (IsInt(NewType))
	{
		for (const FVoxelPinRef& PinRef : Pins)
		{
			FVoxelPin& Pin = GetPin(PinRef);
			if (IsFloat(Pin.GetType()) ||
				IsDouble(Pin.GetType()))
			{
				SetPinScalarType<int32>(Pin);
			}
		}
	}
}

void FVoxelTemplateNode::EnforceSameDimensions(
	const FVoxelPin& InPin,
	const FVoxelPinType& NewType,
	const TVoxelArray<FVoxelPinRef>& Pins)
{
	const int32 NewDimension = GetDimension(NewType);
	if (NewDimension != 1)
	{
		ensure(
			NewDimension == 2 ||
			NewDimension == 3 ||
			NewDimension == 4);

		// We have a vector pin, force all other vector pins to have the same size
		for (const FVoxelPinRef& PinRef : Pins)
		{
			FVoxelPin& Pin = GetPin(PinRef);

			const int32 PinDimension = GetDimension(Pin.GetType());
			if (PinDimension == 1)
			{
				// Scalar, allowed
				continue;
			}

			SetPinDimension(Pin, NewDimension);
		}
	}
	else
	{
		if (!InPin.bIsInput)
		{
			// If we're setting the output to be scalar, everything needs to be scalar
			for (const FVoxelPinRef& PinRef : Pins)
			{
				FVoxelPin& Pin = GetPin(PinRef);
				SetPinDimension(Pin, 1);
			}
		}
	}

	// Fixup output
	for (const FVoxelPinRef& PinRef : Pins)
	{
		FVoxelPin& Pin = GetPin(PinRef);
		if (Pin.bIsInput)
		{
			continue;
		}

		SetPinDimension(Pin, GetMaxDimension(GetAllPins()));
	}
}