// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "K2Node_SetVoxelGraphParameter.h"
#include "K2Node_GetVoxelGraphParameter.h"
#include "FunctionLibrary/VoxelParameterFunctionLibrary.h"

#include "KismetCompiler.h"
#include "K2Node_MakeArray.h"
#include "K2Node_GetArrayItem.h"

UK2Node_SetVoxelGraphParameter::UK2Node_SetVoxelGraphParameter()
{
	FunctionReference.SetExternalMember(GET_FUNCTION_NAME_CHECKED(UVoxelParameterFunctionLibrary, K2_SetVoxelParameter), UVoxelParameterFunctionLibrary::StaticClass());
}

void UK2Node_SetVoxelGraphParameter::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	if (UEdGraphPin* OutValuePin = FindPin(STATIC_FNAME("OutValue")))
	{
		OutValuePin->PinFriendlyName = INVTEXT("Value");
	}

	{
		const int32 ValuePinIndex = Pins.IndexOfByPredicate([](const UEdGraphPin* Pin)
		{
			return Pin->PinName == STATIC_FNAME("Value");
		});
		const int32 AssetPinIndex = Pins.IndexOfByPredicate([](const UEdGraphPin* Pin)
		{
			return Pin->PinName == AssetPinName;
		});

		if (!ensure(ValuePinIndex != -1) ||
			!ensure(AssetPinIndex != -1))
		{
			return;
		}

		Pins.Swap(ValuePinIndex, AssetPinIndex);
	}

	{
		const int32 ValuePinIndex = Pins.IndexOfByPredicate([](const UEdGraphPin* Pin)
		{
			return Pin->PinName == STATIC_FNAME("Value");
		});
		const int32 ParameterPinIndex = Pins.IndexOfByPredicate([](const UEdGraphPin* Pin)
		{
			return Pin->PinName == ParameterPinName;
		});

		if (!ensure(ValuePinIndex != -1) ||
			!ensure(ParameterPinIndex != -1))
		{
			return;
		}

		Pins.Swap(ValuePinIndex, ParameterPinIndex);
	}
}

void UK2Node_SetVoxelGraphParameter::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	UEdGraphPin* NamePin = FindPin(STATIC_FNAME("Name"));
	UEdGraphPin* OutValuePin = FindPin(STATIC_FNAME("OutValue"));
	UEdGraphPin* ParameterContainerPin = FindPin(STATIC_FNAME("ParameterContainer"));

	if (!ensure(NamePin) ||
		!ensure(OutValuePin) ||
		!ensure(ParameterContainerPin))
	{
		return;
	}

	if (OutValuePin->LinkedTo.Num() > 0)
	{
		UK2Node_GetVoxelGraphParameter* Function = CompilerContext.SpawnIntermediateNode<UK2Node_GetVoxelGraphParameter>(this, SourceGraph);
		Function->AllocateDefaultPins();

		UEdGraphPin* FunctionNamePin = Function->FindPin(STATIC_FNAME("Name"));
		UEdGraphPin* FunctionValuePin = Function->FindPin(STATIC_FNAME("Value"));
		UEdGraphPin* FunctionParameterContainerPin = Function->FindPin(STATIC_FNAME("ParameterContainer"));

		if (!ensure(FunctionNamePin) ||
			!ensure(FunctionValuePin) ||
			!ensure(FunctionParameterContainerPin))
		{
			return;
		}

		Function->CachedParameter = CachedParameter;
		FunctionNamePin->DefaultValue = NamePin->DefaultValue;

		Function->PostReconstructNode();

		CompilerContext.MovePinLinksToIntermediate(*GetThenPin(), *Function->GetThenPin());
		CompilerContext.CopyPinLinksToIntermediate(*ParameterContainerPin, *FunctionParameterContainerPin);
		CompilerContext.CopyPinLinksToIntermediate(*NamePin, *FunctionNamePin);
		CompilerContext.MovePinLinksToIntermediate(*OutValuePin, *FunctionValuePin);

		GetThenPin()->MakeLinkTo(Function->GetExecPin());

		Function->PostReconstructNode();

		CompilerContext.MessageLog.NotifyIntermediateObjectCreation(Function, this);
	}
}

bool UK2Node_SetVoxelGraphParameter::IsPinWildcard(const UEdGraphPin& Pin) const
{
	return
		Pin.PinName == STATIC_FNAME("Value") ||
		Pin.PinName == STATIC_FNAME("OutValue");
}

UEdGraphPin* UK2Node_SetVoxelGraphParameter::GetParameterNamePin() const
{
	return FindPin(STATIC_FNAME("Name"));
}