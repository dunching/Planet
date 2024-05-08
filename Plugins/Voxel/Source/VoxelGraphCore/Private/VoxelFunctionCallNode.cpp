// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelFunctionCallNode.h"
#include "VoxelGraph.h"
#include "VoxelBuffer.h"
#include "VoxelInlineGraph.h"
#include "VoxelGraphExecutor.h"
#include "VoxelParameterValues.h"

void FVoxelNode_FunctionCall::PreCompile()
{
	VOXEL_FUNCTION_COUNTER();

	const UVoxelGraph* Graph = GetGraph();
	if (!Graph)
	{
		VOXEL_MESSAGE(Error, "{0}: Graph is null", this);
		return;
	}

	ComputeInputMap = MakeVoxelShared<FVoxelComputeInputMap>();

	for (const FVoxelPin& Pin : GetPins())
	{
		if (!Pin.bIsInput ||
			Pin.Name == FVoxelNodeNames::MacroTemplateInput ||
			Pin.Name == FVoxelNodeNames::MacroRecursiveTemplateInput)
		{
			continue;
		}

		const FVoxelParameter* InputParameter = nullptr;
		for (const FVoxelGraphParameter& Parameter : Graph->Parameters)
		{
			if (Parameter.ParameterType != EVoxelGraphParameterType::Input ||
				Pin.Name != FName(Parameter.Guid.ToString(EGuidFormats::Digits)))
			{
				continue;
			}

			ensure(!InputParameter);
			InputParameter = &Parameter;
		}

		if (!ensure(InputParameter))
		{
			continue;
		}

		const FVoxelGraphPinRef PinRef
		{
			GetNodeRef(),
			Pin.Name
		};

		FVoxelComputeInput Input;
		Input.Compute = GVoxelGraphExecutorManager->MakeCompute_GameThread(Pin.GetType(), PinRef);
		Input.Compute_Executor = GVoxelGraphExecutorManager->MakeCompute_GameThread(Pin.GetType(), PinRef, true);
		ComputeInputMap->Add_CheckNew(InputParameter->Name, Input);
	}
}

FVoxelComputeValue FVoxelNode_FunctionCall::CompileCompute(const FName PinName) const
{
	VOXEL_FUNCTION_COUNTER();

	const UVoxelGraph* Graph = GetGraph();
	if (!Graph)
	{
		VOXEL_MESSAGE(Error, "{0}: Graph is null", this);
		return nullptr;
	}

	const FVoxelParameter* OutputParameter = nullptr;
	for (const FVoxelGraphParameter& Parameter : Graph->Parameters)
	{
		if (Parameter.ParameterType != EVoxelGraphParameterType::Output ||
			PinName != FName(Parameter.Guid.ToString(EGuidFormats::Digits)))
		{
			continue;
		}

		ensure(!OutputParameter);
		OutputParameter = &Parameter;
	}

	if (!ensure(OutputParameter))
	{
		return nullptr;
	}

	if (Type == EVoxelGraphMacroType::Macro)
	{
		const TSharedRef<FVoxelInlineGraphData> Data = FVoxelInlineGraphData::Create(
			GraphInterface,
			{});

		return [
			OutputParameter = *OutputParameter,
			Data,
			Node = GetNodeRef(),
			ComputeInputMap = ComputeInputMap.ToSharedRef()](const FVoxelQuery& Query) -> FVoxelFutureValue
		{
			if (Query.GetContext().GetDepth() >= GVoxelMaxRecursionDepth)
			{
				VOXEL_MESSAGE(Error, "{0}: Max recursion depth reached", Node);
				return FVoxelRuntimePinValue(OutputParameter.Type);
			}

			ensure(!Data->ParameterPath.IsSet());

			const FVoxelChildQueryContextKey Key
			{
				Node,
				FVoxelParameterPath(),
				Data->ParameterValues.ToSharedRef(),
				Query.GetSharedContext(),
				ComputeInputMap,
				-1,
				nullptr
			};
			const TSharedRef<FVoxelQueryContext> Context = Query.GetContext().GetChildContext(Key);

			const TSharedPtr<const FVoxelComputeValue> Output = Data->FindOutput(Query, OutputParameter.Name);
			if (!ensureVoxelSlow(Output))
			{
				VOXEL_MESSAGE(Error, "{0}: No output named {1} on {2}. Valid names: {3}",
					Node,
					OutputParameter.Name,
					Data->GetWeakGraphInterface(Query),
					Data->GetValidOutputNames(Query));
				return FVoxelRuntimePinValue(OutputParameter.Type);
			}

			return (*Output)(Query.MakeNewQuery(Context));
		};
	}
	else if (Type == EVoxelGraphMacroType::Template)
	{
		return [
			OutputParameter = *OutputParameter,
			Node = GetNodeRef(),
			ComputeInputMap = ComputeInputMap.ToSharedRef(),
			WeakNodeRuntime = GetNodeRuntime().AsWeak()](const FVoxelQuery& Query) -> FVoxelFutureValue
		{
			if (Query.GetContext().GetDepth() >= GVoxelMaxRecursionDepth)
			{
				VOXEL_MESSAGE(Error, "{0}: Max recursion depth reached", Node);
				return FVoxelRuntimePinValue(OutputParameter.Type);
			}

			const TSharedPtr<const FVoxelNodeRuntime> NodeRuntime = WeakNodeRuntime.Pin();
			if (!ensure(NodeRuntime))
			{
				return FVoxelRuntimePinValue(OutputParameter.Type);
			}

			const TValue<FVoxelInlineGraph> Graph = NodeRuntime->Get<FVoxelInlineGraph>(
				FVoxelPinRef(FVoxelNodeNames::MacroTemplateInput),
				Query);

			return
				MakeVoxelTask()
				.Dependency(Graph)
				.Execute(OutputParameter.Type, [=]() -> FVoxelFutureValue
				{
					const TSharedPtr<FVoxelInlineGraphData> Data = Graph.Get_CheckCompleted().Data;
					if (!Data)
					{
						// Don't raise errors
						return {};
					}

					const FVoxelChildQueryContextKey Key
					{
						Node,
						Data->ParameterPath.IsSet() ? Data->ParameterPath.GetValue() : FVoxelParameterPath(),
						Data->ParameterPath.IsSet() ? Query.GetContext().ParameterValues : Data->ParameterValues.ToSharedRef(),
						Query.GetSharedContext(),
						ComputeInputMap,
						-1,
						nullptr
					};
					const TSharedRef<FVoxelQueryContext> Context = Query.GetContext().GetChildContext(Key);

					const TSharedPtr<const FVoxelComputeValue> Output = Data->FindOutput(Query, OutputParameter.Name);
					if (!Output)
					{
						VOXEL_MESSAGE(Error, "{0}: No output named {1} on {2}. Valid names: {3}",
							Node,
							OutputParameter.Name,
							Data->GetWeakGraphInterface(Query),
							Data->GetValidOutputNames(Query));
						return {};
					}

					return (*Output)(Query.MakeNewQuery(Context));
				});
		};
	}
	else
	{
		ensure(Type == EVoxelGraphMacroType::RecursiveTemplate);

		return [
			OutputParameter = *OutputParameter,
			Node = GetNodeRef(),
			ComputeInputMap = ComputeInputMap.ToSharedRef(),
			WeakNodeRuntime = GetNodeRuntime().AsWeak()](const FVoxelQuery& Query) -> FVoxelFutureValue
		{
			if (Query.GetContext().GetDepth() >= GVoxelMaxRecursionDepth)
			{
				VOXEL_MESSAGE(Error, "{0}: Max recursion depth reached", Node);
				return FVoxelRuntimePinValue(OutputParameter.Type);
			}

			const TSharedPtr<const FVoxelNodeRuntime> NodeRuntime = WeakNodeRuntime.Pin();
			if (!ensure(NodeRuntime))
			{
				return FVoxelRuntimePinValue(OutputParameter.Type);
			}

			const TValue<FVoxelComplexTerminalBuffer> FutureGraphArray = NodeRuntime->Get<FVoxelComplexTerminalBuffer>(
				FVoxelPinRef(FVoxelNodeNames::MacroRecursiveTemplateInput),
				Query);

			return
				MakeVoxelTask()
				.Dependency(FutureGraphArray)
				.Execute(OutputParameter.Type, [=]() -> FVoxelFutureValue
				{
					TVoxelArray<TSharedRef<const FVoxelInlineGraphData>> GraphArray;
					FutureGraphArray.Get_CheckCompleted().Foreach<FVoxelInlineGraph>([&](const FVoxelInlineGraph& Graph)
					{
						if (ensure(Graph.Data))
						{
							GraphArray.Add(Graph.Data.ToSharedRef());
						}
					});

					if (GraphArray.Num() == 0)
					{
						// Don't raise errors
						return {};
					}

					const TSharedRef<const FVoxelInlineGraphData> Data =  GraphArray.Last();
					if (Data->GetWeakGraphInterface(Query).IsExplicitlyNull())
					{
						// Don't raise errors
						return {};
					}

					const TSharedPtr<const FVoxelComputeValue> Output = Data->FindOutput(Query, OutputParameter.Name);
					if (!Output)
					{
						VOXEL_MESSAGE(Error, "{0}: No output named {1} on {2}. Valid names: {3}",
							Node,
							OutputParameter.Name,
							Data->GetWeakGraphInterface(Query),
							Data->GetValidOutputNames(Query));
						return {};
					}

					const FVoxelChildQueryContextKey Key
					{
						Node,
						Data->ParameterPath.IsSet() ? Data->ParameterPath.GetValue() : FVoxelParameterPath(),
						Data->ParameterPath.IsSet() ? Query.GetContext().ParameterValues : Data->ParameterValues.ToSharedRef(),
						Query.GetSharedContext(),
						ComputeInputMap,
						GraphArray.Num() - 1,
						::MakeSharedCopy(MoveTemp(GraphArray))
					};
					const TSharedRef<FVoxelQueryContext> Context = Query.GetContext().GetChildContext(Key);

					return (*Output)(Query.MakeNewQuery(Context));
				});
		};
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelNode_FunctionCall::FixupPins()
{
	VOXEL_FUNCTION_COUNTER();

	for (const auto& It : MakeCopy(GetPinsMap()))
	{
		RemovePin(It.Key);
	}

	const UVoxelGraph* Graph = GetGraph();
	if (!Graph)
	{
		return;
	}
	ensure(!Graph->HasAnyFlags(RF_NeedLoad));

	if (Type == EVoxelGraphMacroType::Macro)
	{
		// No additional pin
	}
	else if (Type == EVoxelGraphMacroType::Template)
	{
		CreateInputPin(
			FVoxelPinType::Make<FVoxelInlineGraph>(),
			FVoxelNodeNames::MacroTemplateInput);
	}
	else
	{
		ensure(Type == EVoxelGraphMacroType::RecursiveTemplate);

		CreateInputPin(
			FVoxelPinType::Make<FVoxelInlineGraph>().GetBufferType(),
			FVoxelNodeNames::MacroRecursiveTemplateInput);
	}

	for (const FVoxelGraphParameter& Parameter : Graph->Parameters)
	{
		const FName PinName(Parameter.Guid.ToString(EGuidFormats::Digits));

		if (Parameter.ParameterType == EVoxelGraphParameterType::Input)
		{
			// VirtualPin: inputs are virtual as they are queried by node name below
			// This avoid tricky lifetime management
			CreateInputPin(
				Parameter.Type,
				PinName,
				VOXEL_PIN_METADATA(void, nullptr, VirtualPin));
		}
		else if (Parameter.ParameterType == EVoxelGraphParameterType::Output)
		{
			CreateOutputPin(
				Parameter.Type,
				PinName);
		}
	}
}

UVoxelGraph* FVoxelNode_FunctionCall::GetGraph() const
{
	if (!GraphInterface)
	{
		return nullptr;
	}

	return GraphInterface->GetGraph();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelNode_FunctionCallInput_WithoutDefaultPin::PreCompile()
{
	ensure(DefaultValue.IsValid());

	RuntimeDefaultValue = FVoxelPinType::MakeRuntimeValue(
		GetPin(ValuePin).GetType(),
		DefaultValue);

	ensure(RuntimeDefaultValue.IsValid());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_FunctionCallInput_WithoutDefaultPin, Value)
{
	const TSharedPtr<const FVoxelComputeInputMap> ComputeInputMap = Query.GetContext().ComputeInputMap;
	if (!ComputeInputMap)
	{
		// If we don't have input values use our default value
		return RuntimeDefaultValue;
	}

	const FVoxelComputeInput* Input = ComputeInputMap->Find(Name);
	if (!Input)
	{
		// If we can't find a matching input this is likely a graph override
		return RuntimeDefaultValue;
	}

	const TSharedPtr<FVoxelQueryContext> Context = Query.GetContext().ComputeInputContext.Pin();
	if (!ensure(Context))
	{
		return {};
	}

	return (*Input->Compute)(Query.MakeNewQuery(Context.ToSharedRef()));
}

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_FunctionCallInput_WithDefaultPin, Value)
{
	const TSharedPtr<const FVoxelComputeInputMap> ComputeInputMap = Query.GetContext().ComputeInputMap;
	if (!ComputeInputMap)
	{
		// If we don't have input values use our input pin
		return Get(DefaultPin, Query);
	}

	const FVoxelComputeInput* Input = ComputeInputMap->Find(Name);
	if (!Input)
	{
		// If we can't find a matching input this is likely a graph override
		return Get(DefaultPin, Query);
	}

	const TSharedPtr<FVoxelQueryContext> Context = Query.GetContext().ComputeInputContext.Pin();
	if (!ensure(Context))
	{
		return {};
	}

	const FVoxelQuery NewQuery = Query.MakeNewQuery(Context.ToSharedRef());
	const FVoxelFutureValue FutureExecutor = (*Input->Compute_Executor)(NewQuery);
	return VOXEL_ON_COMPLETE(NewQuery, FutureExecutor)
	{
		const TSharedPtr<const FVoxelGraphExecutor> Executor = FutureExecutor.Get<FVoxelGraphExecutorRef>().Executor;
		if (!Executor)
		{
			// Failed to compile
			return {};
		}

		if (Executor->Nodes.Num() == 1)
		{
			// Use default
			return Get(DefaultPin, Query);
		}

		FVoxelTaskReferencer::Get().AddExecutor(Executor.ToSharedRef());
		return Executor->Execute(NewQuery);
	};
}