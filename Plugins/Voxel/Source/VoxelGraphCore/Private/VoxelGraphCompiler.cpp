// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelGraphCompiler.h"
#include "VoxelGraph.h"
#include "VoxelRootNode.h"
#include "VoxelExecNode.h"
#include "VoxelExecNodes.h"
#include "VoxelDebugNode.h"
#include "VoxelTemplateNode.h"
#include "VoxelFunctionNode.h"
#include "VoxelParameterNode.h"
#include "VoxelFunctionCallNode.h"
#include "VoxelGraphCompileScope.h"
#include "VoxelLocalVariableNodes.h"
#include "Preview/VoxelPreviewNode.h"
#include "FunctionLibrary/VoxelBasicFunctionLibrary.h"

TSharedPtr<Voxel::Graph::FGraph> FVoxelGraphCompiler::TranslateRuntimeGraph(const UVoxelRuntimeGraph& RuntimeGraph)
{
	VOXEL_FUNCTION_COUNTER();
	check(IsInGameThread());
	check(GVoxelGraphCompileScope);
	const FVoxelRuntimeGraphData& RuntimeGraphData = RuntimeGraph.GetData();

	// Manually call FixupPins on function calls
	// This needs to happen as late as possible as it requires graphs to be loaded
	{
		VOXEL_SCOPE_COUNTER("Fixup function calls");

		for (const auto& It : RuntimeGraphData.NodeNameToNode)
		{
			if (It.Value.VoxelNode.IsA<FVoxelNode_FunctionCall>())
			{
				ConstCast(It.Value.VoxelNode.Get<FVoxelNode_FunctionCall>()).FixupPins();
			}
		}
	}

	const TSharedRef<FGraph> Graph = MakeVoxelShared<FGraph>();

	TVoxelMap<const FVoxelRuntimeNode*, FNode*> NodesMap;
	TVoxelSet<const FVoxelRuntimeNode*> SkippedNodes;

	for (const auto& RuntimeNodeIt : RuntimeGraphData.GetNodeNameToNode())
	{
		const FVoxelRuntimeNode& RuntimeNode = RuntimeNodeIt.Value;
		VOXEL_SCOPE_COUNTER_FNAME(RuntimeNode.EdGraphNodeTitle);

		if (!RuntimeNode.VoxelNode.IsValid())
		{
			for (const auto& It : RuntimeNode.InputPins)
			{
				if (It.Value.LinkedTo.Num() > 0)
				{
					VOXEL_MESSAGE(Error, "Invalid struct on {0}", RuntimeNode);
					return nullptr;
				}
			}
			for (const auto& It : RuntimeNode.OutputPins)
			{
				if (It.Value.LinkedTo.Num() > 0)
				{
					VOXEL_MESSAGE(Error, "Invalid struct on {0}", RuntimeNode);
					return nullptr;
				}
			}

			// This node isn't connected to anything, just skip it
			VOXEL_MESSAGE(Warning, "Invalid struct on {0}", RuntimeNode);
			SkippedNodes.Add(&RuntimeNode);
			continue;
		}


		FVoxelGraphNodeRef NodeRef;
		NodeRef.Graph = RuntimeGraph.GetOuterUVoxelGraph();
		NodeRef.NodeId = RuntimeNode.GetNodeId();
		NodeRef.EdGraphNodeTitle = RuntimeNode.EdGraphNodeTitle;
		NodeRef.EdGraphNodeName = RuntimeNode.EdGraphNodeName;

		FNode& Node = Graph->NewNode(NodeRef);
		{
			VOXEL_SCOPE_COUNTER_FORMAT("Copy node %s", *RuntimeNode.VoxelNode->GetStruct()->GetName());
			Node.SetVoxelNode(RuntimeNode.VoxelNode->MakeSharedCopy());
		}
		NodesMap.Add(&RuntimeNode, &Node);

		for (const FString& Error : RuntimeNode.Errors)
		{
			Node.AddError(Error);
		}

		const FVoxelNode& VoxelNode = Node.GetVoxelNode();
		for (const FVoxelPin& Pin : VoxelNode.GetPins())
		{
			if (!(Pin.bIsInput ? RuntimeNode.InputPins : RuntimeNode.OutputPins).Contains(Pin.Name))
			{
				VOXEL_MESSAGE(Error, "Outdated node {0}: missing pin {1}", RuntimeNode, Pin.Name);
				return nullptr;
			}
		}
		for (const auto& It : RuntimeNode.InputPins)
		{
			const FVoxelRuntimePin& RuntimePin = It.Value;
			if (!RuntimePin.ParentPinName.IsNone())
			{
				// Sub-pinS
				continue;
			}

			const TSharedPtr<const FVoxelPin> Pin = VoxelNode.FindPin(RuntimePin.PinName);
			if (!Pin ||
				!Pin->bIsInput)
			{
				VOXEL_MESSAGE(Error, "Outdated node {0}: unknown pin {1}", RuntimeNode, RuntimePin.PinName);
				return nullptr;
			}

			if (RuntimePin.Type != Pin->GetType())
			{
				VOXEL_MESSAGE(Error, "Outdated node {0}: type mismatch for pin {1}", RuntimeNode, RuntimePin.PinName);
				return nullptr;
			}
		}
		for (const auto& It : RuntimeNode.OutputPins)
		{
			const FVoxelRuntimePin& RuntimePin = It.Value;
			if (!RuntimePin.ParentPinName.IsNone())
			{
				// Sub-pinS
				continue;
			}

			const TSharedPtr<const FVoxelPin> Pin = VoxelNode.FindPin(RuntimePin.PinName);
			if (!Pin ||
				Pin->bIsInput)
			{
				VOXEL_MESSAGE(Error, "Outdated node {0}: unknown pin {1}", RuntimeNode, RuntimePin.PinName);
				return nullptr;
			}

			if (RuntimePin.Type != Pin->GetType())
			{
				VOXEL_MESSAGE(Error, "Outdated node {0}: type mismatch for pin {1}", RuntimeNode, RuntimePin.PinName);
				return nullptr;
			}
		}

		for (const auto& RuntimePinIt : RuntimeNode.InputPins)
		{
			const FVoxelRuntimePin& RuntimePin = RuntimePinIt.Value;
			if (!RuntimePin.Type.IsValid())
			{
				VOXEL_MESSAGE(Error, "Invalid pin {0}.{1}", RuntimeNode, RuntimePin.PinName);
				return nullptr;
			}

			FVoxelPinValue DefaultValue;
			if (RuntimePin.Type.HasPinDefaultValue())
			{
				DefaultValue = RuntimePin.DefaultValue;

				if (!DefaultValue.IsValid() ||
					!ensureVoxelSlow(DefaultValue.GetType().CanBeCastedTo(RuntimePin.Type.GetPinDefaultValueType())))
				{
					VOXEL_MESSAGE(Error, "{0}.{1}: Invalid default value", RuntimeNode, RuntimePin.PinName);
					return nullptr;
				}
			}
			else
			{
				ensureVoxelSlow(!RuntimePin.DefaultValue.IsValid());
			}

			Node.NewInputPin(RuntimePin.PinName, RuntimePin.Type, DefaultValue).SetParentName(RuntimePin.ParentPinName);
		}

		for (const auto& RuntimePinIt : RuntimeNode.OutputPins)
		{
			const FVoxelRuntimePin& RuntimePin = RuntimePinIt.Value;
			if (!RuntimePin.Type.IsValid())
			{
				VOXEL_MESSAGE(Error, "Invalid pin {0}.{1}", RuntimeNode, RuntimePin.PinName);
				return nullptr;
			}

			Node.NewOutputPin(RuntimePin.PinName, RuntimePin.Type).SetParentName(RuntimePin.ParentPinName);
		}
	}

	VOXEL_SCOPE_COUNTER("Fixup links");

	// Fixup links after all nodes are created
	for (const auto& RuntimeNodeIt : RuntimeGraphData.GetNodeNameToNode())
	{
		const FVoxelRuntimeNode& RuntimeNode = RuntimeNodeIt.Value;
		if (SkippedNodes.Contains(&RuntimeNode))
		{
			continue;
		}

		FNode& Node = *NodesMap[&RuntimeNode];

		for (const auto& RuntimePinIt : RuntimeNode.InputPins)
		{
			const FVoxelRuntimePin& InputPin = RuntimePinIt.Value;
			if (InputPin.LinkedTo.Num() > 1)
			{
				VOXEL_MESSAGE(Error, "Too many pins linked to {0}.{1}", RuntimeNode, InputPin.PinName);
				return nullptr;
			}

			for (const FVoxelRuntimePinRef& OutputPinRef : InputPin.LinkedTo)
			{
				check(!OutputPinRef.bIsInput);

				const FVoxelRuntimePin* OutputPin = RuntimeGraphData.FindPin(OutputPinRef);
				if (!ensure(OutputPin))
				{
					VOXEL_MESSAGE(Error, "Invalid pin ref on {0}", RuntimeNode);
					continue;
				}

				const FVoxelRuntimeNode& OtherRuntimeNode = RuntimeGraphData.GetNodeNameToNode()[OutputPinRef.NodeName];
				FNode& OtherNode = *NodesMap[&OtherRuntimeNode];

				if (!OutputPin->Type.CanBeCastedTo_Schema(InputPin.Type))
				{
					VOXEL_MESSAGE(Error, "Invalid pin link from {0}.{1} to {2}.{3}: type mismatch: {4} vs {5}",
						RuntimeNode,
						OutputPin->PinName,
						OtherRuntimeNode,
						InputPin.PinName,
						OutputPin->Type.ToString(),
						InputPin.Type.ToString());

					continue;
				}

				Node.FindInputChecked(InputPin.PinName).MakeLinkTo(OtherNode.FindOutputChecked(OutputPinRef.PinName));
			}
		}
	}

	// Input links are used to populate all links, check they're correct with output links
	for (const auto& RuntimeNodeIt : RuntimeGraphData.GetNodeNameToNode())
	{
		const FVoxelRuntimeNode& RuntimeNode = RuntimeNodeIt.Value;
		if (SkippedNodes.Contains(&RuntimeNode))
		{
			continue;
		}

		FNode& Node = *NodesMap[&RuntimeNode];

		for (const auto& RuntimePinIt : RuntimeNode.OutputPins)
		{
			const FVoxelRuntimePin& OutputPin = RuntimePinIt.Value;
			for (const FVoxelRuntimePinRef& InputPinRef : OutputPin.LinkedTo)
			{
				check(InputPinRef.bIsInput);

				const FVoxelRuntimePin* InputPin = RuntimeGraphData.FindPin(InputPinRef);
				if (!ensure(InputPin))
				{
					VOXEL_MESSAGE(Error, "Invalid pin ref on {0}", RuntimeNode);
					continue;
				}

				const FVoxelRuntimeNode& OtherRuntimeNode = RuntimeGraphData.GetNodeNameToNode()[InputPinRef.NodeName];
				FNode& OtherNode = *NodesMap[&OtherRuntimeNode];

				if (!OutputPin.Type.CanBeCastedTo_Schema(InputPin->Type))
				{
					VOXEL_MESSAGE(Error, "Invalid pin link from {0}.{1} to {2}.{3}: type mismatch: {4} vs {5}",
						RuntimeNode,
						OutputPin.PinName,
						OtherRuntimeNode,
						InputPin->PinName,
						OutputPin.Type.ToString(),
						InputPin->Type.ToString());

					continue;
				}

				if (!ensure(Node.FindOutputChecked(OutputPin.PinName).IsLinkedTo(OtherNode.FindInputChecked(InputPinRef.PinName))))
				{
					VOXEL_MESSAGE(Error, "Translation error: {0} -> {1}", Node, OtherNode);
				}
			}
		}
	}

	Graph->Check();
	return Graph;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelGraphCompiler::RemoveSplitPins(FGraph& Graph, const UVoxelRuntimeGraph& RuntimeGraph)
{
	VOXEL_FUNCTION_COUNTER();
	const FVoxelRuntimeGraphData& RuntimeGraphData = RuntimeGraph.GetData();

	for (FNode& Node : Graph.GetNodesCopy())
	{
		TVoxelMap<FPin*, FNode*> ParentPinToMakeBreakNodes;
		TVoxelArray<FPin*> PinsToRemove;
		for (FPin& SubPin : Node.GetPins())
		{
			if (SubPin.GetParentName().IsNone())
			{
				continue;
			}

			FPin* ParentPin = Node.FindPin(SubPin.GetParentName());
			if (!ensure(ParentPin))
			{
				VOXEL_MESSAGE(Error, "{0}: Invalid parent pin", SubPin);
				return;
			}

			FNode*& MakeBreakNode = ParentPinToMakeBreakNodes.FindOrAdd(ParentPin);
			if (!MakeBreakNode)
			{
				if (ParentPin->Direction == EPinDirection::Input)
				{
					FNode& MakeNode = Graph.NewNode(Node.NodeRef.WithSuffix("Make_" + ParentPin->Name.ToString()));
					MakeBreakNode = &MakeNode;

					const FVoxelNode* MakeVoxelNodeTemplate = RuntimeGraphData.FindMakeNode(ParentPin->Type);
					if (!ensure(MakeVoxelNodeTemplate))
					{
						VOXEL_MESSAGE(Error, "{0}: No make node for type {1}", ParentPin, ParentPin->Type.ToString());
						return;
					}

					const TSharedRef<FVoxelNode> MakeVoxelNode = MakeVoxelNodeTemplate->MakeSharedCopy();
					MakeVoxelNode->PromotePin_Runtime(MakeVoxelNode->GetUniqueOutputPin(), ParentPin->Type);
					MakeNode.SetVoxelNode(MakeVoxelNode);

					MakeNode.NewOutputPin(
						MakeVoxelNode->GetUniqueOutputPin().Name,
						MakeVoxelNode->GetUniqueOutputPin().GetType()).MakeLinkTo(*ParentPin);

					for (const FVoxelPin& Pin : MakeVoxelNode->GetPins())
					{
						if (!Pin.bIsInput)
						{
							continue;
						}

						MakeNode.NewInputPin(Pin.Name, Pin.GetType());
					}
				}
				else
				{
					check(ParentPin->Direction == EPinDirection::Output);

					FNode& BreakNode = Graph.NewNode(Node.NodeRef.WithSuffix("Break_" + ParentPin->Name.ToString()));
					MakeBreakNode = &BreakNode;

					const FVoxelNode* BreakVoxelNodeTemplate = RuntimeGraphData.FindBreakNode(ParentPin->Type);
					if (!ensure(BreakVoxelNodeTemplate))
					{
						VOXEL_MESSAGE(Error, "{0}: No break node for type {1}", ParentPin, ParentPin->Type.ToString());
						return;
					}

					const TSharedRef<FVoxelNode> BreakVoxelNode = BreakVoxelNodeTemplate->MakeSharedCopy();
					BreakVoxelNode->PromotePin_Runtime(BreakVoxelNode->GetUniqueInputPin(), ParentPin->Type);
					BreakNode.SetVoxelNode(BreakVoxelNode);

					BreakNode.NewInputPin(
						BreakVoxelNode->GetUniqueInputPin().Name,
						BreakVoxelNode->GetUniqueInputPin().GetType()).MakeLinkTo(*ParentPin);

					for (const FVoxelPin& Pin : BreakVoxelNode->GetPins())
					{
						if (Pin.bIsInput)
						{
							continue;
						}

						BreakNode.NewOutputPin(Pin.Name, Pin.GetType());
					}
				}
			}
			check(MakeBreakNode);

			FPin* NewPin = MakeBreakNode->FindPin(SubPin.Name);
			if (!ensure(NewPin))
			{
				VOXEL_MESSAGE(Error, "{0}: Invalid sub-pin", SubPin);
				return;
			}
			ensure(NewPin->GetLinkedTo().Num() == 0);

			if (SubPin.Direction == EPinDirection::Input)
			{
				SubPin.CopyInputPinTo(*NewPin);
			}
			else
			{
				check(SubPin.Direction == EPinDirection::Output);
				SubPin.CopyOutputPinTo(*NewPin);
			}

			PinsToRemove.Add(&SubPin);
		}

		for (FPin* Pin : PinsToRemove)
		{
			Node.RemovePin(*Pin);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelGraphCompiler::AddWildcardErrors(FGraph& Graph)
{
	VOXEL_FUNCTION_COUNTER();

	for (FNode& Node : Graph.GetNodes())
	{
		for (const FPin& Pin : Node.GetPins())
		{
			if (Pin.Type.IsWildcard())
			{
				Node.AddError("Wildcard pin " + Pin.Name.ToString() + " needs to be converted. Please connect it to another pin or right click it -> Convert");
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelGraphCompiler::AddNoDefaultErrors(FGraph& Graph)
{
	VOXEL_FUNCTION_COUNTER();

	for (FNode& Node : Graph.GetNodes())
	{
		for (const FPin& Pin : Node.GetPins())
		{
			if (Pin.GetLinkedTo().Num() > 0)
			{
				continue;
			}

			const TSharedPtr<const FVoxelPin> VoxelPin = Node.GetVoxelNode().FindPin(Pin.Name);
			if (!ensure(VoxelPin) ||
				!VoxelPin->Metadata.bNoDefault)
			{
				continue;
			}

			Node.AddError("Pin " + Pin.Name.ToString() + " needs to be connected");
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelGraphCompiler::CheckParameters(const FGraph& Graph, const UVoxelGraph& VoxelGraph)
{
	VOXEL_FUNCTION_COUNTER();

	for (const FNode& Node : Graph.GetNodes())
	{
		const FVoxelNode_Parameter* ParameterNode = Node.GetVoxelNode().As<FVoxelNode_Parameter>();
		if (!ParameterNode)
		{
			continue;
		}

		if (!VoxelGraph.FindParameterByGuid(ParameterNode->ParameterGuid))
		{
			VOXEL_MESSAGE(Error, "{0}: No parameter named {1}", Node, ParameterNode->ParameterName);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelGraphCompiler::CheckInputs(const FGraph& Graph, const UVoxelGraph& VoxelGraph)
{
	VOXEL_FUNCTION_COUNTER();

	TVoxelMap<FName, TArray<const FNode*>> InputNameToDefaultNodes;
	for (const FNode& Node : Graph.GetNodes())
	{
		if (const FVoxelNode_FunctionCallInput_WithDefaultPin* InputNode = Node.GetVoxelNode().As<FVoxelNode_FunctionCallInput_WithDefaultPin>())
		{
			if (!VoxelGraph.FindParameterByName(EVoxelGraphParameterType::Input, InputNode->Name))
			{
				VOXEL_MESSAGE(Error, "{0}: No input named {1}", Node, InputNode->Name);
			}

			InputNameToDefaultNodes.FindOrAdd(InputNode->Name).Add(&Node);
		}

		if (const FVoxelNode_FunctionCallInput_WithoutDefaultPin* InputNode = Node.GetVoxelNode().As<FVoxelNode_FunctionCallInput_WithoutDefaultPin>())
		{
			if (!VoxelGraph.FindParameterByName(EVoxelGraphParameterType::Input, InputNode->Name))
			{
				VOXEL_MESSAGE(Error, "{0}: No input named {1}", Node, InputNode->Name);
			}
		}
	}

	for (const auto& It : InputNameToDefaultNodes)
	{
		if (It.Value.Num() > 1)
		{
			VOXEL_MESSAGE(Error, "Multiple input nodes with a default pin for input {0}: {1}", It.Key, It.Value);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelGraphCompiler::CheckOutputs(const FGraph& Graph, const UVoxelGraph& VoxelGraph)
{
	VOXEL_FUNCTION_COUNTER();

	TVoxelMap<FName, const FNode*> OutputNameToNode;
	for (const FNode& Node : Graph.GetNodes())
	{
		const FVoxelNode_FunctionCallOutput* OutputNode = Node.GetVoxelNode().As<FVoxelNode_FunctionCallOutput>();
		if (!OutputNode)
		{
			continue;
		}

		if (!VoxelGraph.FindParameterByName(EVoxelGraphParameterType::Output, OutputNode->Name))
		{
			VOXEL_MESSAGE(Error, "{0}: No output named {1}", Node, OutputNode->Name);
			continue;
		}

		const FNode*& RootNode = OutputNameToNode.FindOrAdd(OutputNode->Name);
		if (RootNode)
		{
			VOXEL_MESSAGE(Error, "Duplicated output nodes: {0}, {1}", RootNode, Node);
			continue;
		}

		RootNode = &Node;
	}

	for (const FVoxelGraphParameter& Parameter : VoxelGraph.Parameters)
	{
		if (Parameter.ParameterType != EVoxelGraphParameterType::Output ||
			OutputNameToNode.Contains(Parameter.Name))
		{
			continue;
		}

		VOXEL_MESSAGE(Error, "Missing output node for output {0}", Parameter.Name);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelGraphCompiler::AddPreviewNode(FGraph& Graph, const UVoxelRuntimeGraph& RuntimeGraph)
{
	VOXEL_FUNCTION_COUNTER();

	// Make sure to run AddPreviewNode before any new node is spawned, otherwise the find below will fail
	// and find compile nodes instead

	const FVoxelRuntimePinRef PreviewedRuntimePin = RuntimeGraph.GetData().PreviewedPin;
	if (PreviewedRuntimePin.NodeName.IsNone())
	{
		ensure(PreviewedRuntimePin.PinName.IsNone());
		ensure(!PreviewedRuntimePin.bIsInput);
		return;
	}

	FPin* PreviewedPin = nullptr;
	for (FNode& Node : Graph.GetNodes())
	{
		if (Node.NodeRef.EdGraphNodeName != PreviewedRuntimePin.NodeName)
		{
			continue;
		}

		ensure(!PreviewedPin);
		PreviewedPin = Node.FindPin(PreviewedRuntimePin.PinName);
		ensure(PreviewedPin);
	}

	if (!PreviewedPin)
	{
		VOXEL_MESSAGE(Warning, "Invalid preview pin");
		return;
	}

	FNode& PreviewNode = Graph.NewNode(FVoxelGraphNodeRef
	{
		RuntimeGraph.GetOuterUVoxelGraph(),
		FVoxelNodeNames::PreviewNodeId
	});

	const TSharedRef<FVoxelPreviewNode> PreviewVoxelNode = MakeVoxelShared<FVoxelPreviewNode>();
	PreviewVoxelNode->PromotePin_Runtime(PreviewVoxelNode->GetUniqueInputPin(), PreviewedPin->Type);

	PreviewNode.SetVoxelNode(PreviewVoxelNode);
	PreviewNode.NewInputPin(VOXEL_PIN_NAME(FVoxelPreviewNode, ValuePin), PreviewedPin->Type);

	if (PreviewedPin->Direction == EPinDirection::Input)
	{
		// TODO Is previewing an input allowed?
		PreviewedPin->CopyInputPinTo(PreviewNode.GetInputPin(0));
	}
	else
	{
		check(PreviewedPin->Direction == EPinDirection::Output);
		PreviewedPin->MakeLinkTo(PreviewNode.GetInputPin(0));
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelGraphCompiler::AddDebugNodes(FGraph& Graph, const UVoxelRuntimeGraph& RuntimeGraph)
{
	VOXEL_FUNCTION_COUNTER();

	for (const FName NodeName : RuntimeGraph.GetData().DebuggedNodes)
	{
		for (FNode& Node : Graph.GetNodes())
		{
			if (Node.NodeRef.EdGraphNodeName != NodeName)
			{
				continue;
			}

			for (FPin& Pin : Node.GetOutputPins())
			{
				FNode& DebugNode = Graph.NewNode(Node.NodeRef.WithSuffix("DebugNode").WithSuffix(Pin.Name.ToString()));

				const TSharedRef<FVoxelDebugNode> DebugVoxelNode = MakeVoxelShared<FVoxelDebugNode>();
				DebugVoxelNode->PromotePin_Runtime(DebugVoxelNode->GetUniqueInputPin(), Pin.Type);
				DebugVoxelNode->PromotePin_Runtime(DebugVoxelNode->GetUniqueOutputPin(), Pin.Type);

				DebugNode.SetVoxelNode(DebugVoxelNode);
				DebugNode.NewInputPin(VOXEL_PIN_NAME(FVoxelDebugNode, InPin), Pin.Type);
				DebugNode.NewOutputPin(VOXEL_PIN_NAME(FVoxelDebugNode, OutPin), Pin.Type);

				Pin.CopyOutputPinTo(DebugNode.GetOutputPin(0));
				Pin.BreakAllLinks();

				Pin.MakeLinkTo(DebugNode.GetInputPin(0));
			}

			break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelGraphCompiler::AddToBuffer(FGraph& Graph)
{
	VOXEL_FUNCTION_COUNTER();

	for (FNode& Node : Graph.GetNodesCopy())
	{
		for (FPin& ToPin : Node.GetInputPins())
		{
			if (!ToPin.Type.IsBuffer())
			{
				continue;
			}
			FNode* ToBufferNode = nullptr;

			for (FPin& FromPin : ToPin.GetLinkedTo())
			{
				if (FromPin.Type.IsBuffer())
				{
					continue;
				}

				if (!ToBufferNode)
				{
					ToBufferNode = &Graph.NewNode(Node.NodeRef.WithSuffix(ToPin.Name.ToString() + "_ToBuffer"));

					const TSharedRef<FVoxelFunctionNode> FunctionNode = FVoxelFunctionNode::Make(FindUFunctionChecked(UVoxelBasicFunctionLibrary, ToBuffer));
					FunctionNode->PromotePin_Runtime(FunctionNode->GetUniqueInputPin(), FromPin.Type);
					FunctionNode->PromotePin_Runtime(FunctionNode->GetUniqueOutputPin(), ToPin.Type);
					ToBufferNode->SetVoxelNode(FunctionNode);

					ToBufferNode->NewInputPin(FunctionNode->GetUniqueInputPin().Name, FromPin.Type);
					ToBufferNode->NewOutputPin(FunctionNode->GetUniqueOutputPin().Name, ToPin.Type);
				}

				FromPin.MakeLinkTo(ToBufferNode->GetInputPin(0));
				ToPin.MakeLinkTo(ToBufferNode->GetOutputPin(0));
				ToPin.BreakLinkTo(FromPin);
			}
		}
	}

	for (const FNode& Node : Graph.GetNodes())
	{
		for (const FPin& ToPin : Node.GetInputPins())
		{
			for (const FPin& FromPin : ToPin.GetLinkedTo())
			{
				ensure(FromPin.Type.CanBeCastedTo(ToPin.Type));
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelGraphCompiler::RemoveLocalVariables(FGraph& Graph, const UVoxelGraph& VoxelGraph)
{
	VOXEL_FUNCTION_COUNTER();

	TMap<FGuid, FNode*> GuidToDeclaration;
	for (FNode& Node : Graph.GetNodes())
	{
		const FVoxelLocalVariableDeclaration* Declaration = Node.GetVoxelNode().As<FVoxelLocalVariableDeclaration>();
		if (!Declaration)
		{
			continue;
		}

		if (!VoxelGraph.FindParameterByGuid(Declaration->Guid))
		{
			VOXEL_MESSAGE(
				Error,
				"Invalid local variable node: {0}",
				Declaration);
			return;
		}

		if (const FNode* ExistingNode = GuidToDeclaration.FindRef(Declaration->Guid))
		{
			VOXEL_MESSAGE(
				Error,
				"Multiple local variable declarations not supported: {0}, {1}",
				ExistingNode,
				Node);
			return;
		}

		GuidToDeclaration.Add(Declaration->Guid, &Node);
	}

	TMap<FNode*, TArray<FNode*>> DeclarationToUsage;
	for (FNode& Node : Graph.GetNodes())
	{
		const FVoxelLocalVariableUsage* Usage = Node.GetVoxelNode().As<FVoxelLocalVariableUsage>();
		if (!Usage)
		{
			continue;
		}

		FNode* Declaration = GuidToDeclaration.FindRef(Usage->Guid);
		if (!Declaration)
		{
			VOXEL_MESSAGE(
				Error,
				"Invalid local variable node: {0}",
				Node);
			return;
		}

		DeclarationToUsage.FindOrAdd(Declaration).Add(&Node);
	}

	// Remove usages
	for (auto& It : DeclarationToUsage)
	{
		FNode& Declaration = *It.Key;

		const FPin& InputPin = Declaration.GetInputPin(0);
		for (FNode* Usage : It.Value)
		{
			for (FPin& LinkedTo : Usage->GetOutputPin(0).GetLinkedTo())
			{
				InputPin.CopyInputPinTo(LinkedTo);
			}

			Graph.RemoveNode(*Usage);
		}
	}

	// Remove declarations
	for (const auto& It : GuidToDeclaration)
	{
		FNode& Declaration = *It.Value;

		const FPin& InputPin = Declaration.GetInputPin(0);
		for (FPin& LinkedTo : Declaration.GetOutputPin(0).GetLinkedTo())
		{
			InputPin.CopyInputPinTo(LinkedTo);
		}
		Graph.RemoveNode(Declaration);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelGraphCompiler::CollapseInputs(FGraph& Graph)
{
	VOXEL_FUNCTION_COUNTER();

	TVoxelMap<FName, FNode*> InputNameToNode;

	// Find which input node to use for each input name
	for (FNode& Node : Graph.GetNodes())
	{
		if (const FVoxelNode_FunctionCallInput_WithDefaultPin* InputNode = Node.GetVoxelNode().As<FVoxelNode_FunctionCallInput_WithDefaultPin>())
		{
			FNode*& RootNode = InputNameToNode.FindOrAdd(InputNode->Name);

			// Should be handled by CheckInputs
			ensure(
				!RootNode ||
				// Other input pins without a default pins are allowed
				RootNode->GetVoxelNode().IsA<FVoxelNode_FunctionCallInput_WithoutDefaultPin>());

			RootNode = &Node;
		}

		if (const FVoxelNode_FunctionCallInput_WithoutDefaultPin* InputNode = Node.GetVoxelNode().As<FVoxelNode_FunctionCallInput_WithoutDefaultPin>())
		{
			FNode*& RootNode = InputNameToNode.FindOrAdd(InputNode->Name);
			if (!RootNode)
			{
				RootNode = &Node;
			}
		}
	}

	// Move other nodes link to the root input nodes
	for (FNode& Node : Graph.GetNodesCopy())
	{
		// No need to check for WithDefault, it's always going to be the root node
		const FVoxelNode_FunctionCallInput_WithoutDefaultPin* InputNode = Node.GetVoxelNode().As<FVoxelNode_FunctionCallInput_WithoutDefaultPin>();
		if (!InputNode)
		{
			continue;
		}

		FNode* RootNode = InputNameToNode.FindRef(InputNode->Name);
		if (!ensure(RootNode) ||
			RootNode == &Node)
		{
			continue;
		}

		ensure(Node.GetInputPins().Num() == 0);
		ensure(Node.GetOutputPins().Num() == 1);

		Node.GetOutputPin(0).CopyOutputPinTo(RootNode->GetOutputPin(0));
		Graph.RemoveNode(Node);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelGraphCompiler::AddRootExecuteNode(FGraph& Graph, const UVoxelGraph& VoxelGraph)
{
	VOXEL_FUNCTION_COUNTER();

	FNode& MergeNode = Graph.NewNode(FVoxelGraphNodeRef
	{
		&VoxelGraph,
		FVoxelNodeNames::MergeNodeId
	});

	{
		const TSharedRef<FVoxelNode_MergeExecs> MergeVoxelNode = MakeVoxelShared<FVoxelNode_MergeExecs>();
		MergeNode.NewInputPin("Execs_0", FVoxelPinType::Make<FVoxelExec>(), FVoxelPinValue::Make<FVoxelExec>());
		MergeNode.NewOutputPin(VOXEL_PIN_NAME(FVoxelNode_MergeExecs, ExecPin), FVoxelPinType::Make<FVoxelExec>());
		MergeNode.SetVoxelNode(MergeVoxelNode);

		for (FNode& Node : Graph.GetNodesCopy())
		{
			if (!Node.GetVoxelNode().IsA<FVoxelNode_Execute>())
			{
				continue;
			}

			// Add stats to each Execute node
			FNode& DummyMergeNode = Graph.NewNode(Node.NodeRef);
			const TSharedRef<FVoxelNode_MergeExecs> DummyMergeVoxelNode = MakeVoxelShared<FVoxelNode_MergeExecs>();
			FPin& DummyInputPin = DummyMergeNode.NewInputPin("Execs_0", FVoxelPinType::Make<FVoxelExec>(), FVoxelPinValue::Make<FVoxelExec>());
			FPin& DummyOutputPin = DummyMergeNode.NewOutputPin(VOXEL_PIN_NAME(FVoxelNode_MergeExecs, ExecPin), FVoxelPinType::Make<FVoxelExec>());
			DummyMergeNode.SetVoxelNode(DummyMergeVoxelNode);

			Node.FindInputChecked(VOXEL_PIN_NAME(FVoxelNode_Execute, ExecPin)).CopyInputPinTo(DummyInputPin);

			const FName Name = MergeVoxelNode->AddPinToArray(VOXEL_PIN_NAME(FVoxelNode_MergeExecs, ExecsPins));
			FPin& Pin = MergeNode.NewInputPin(Name, FVoxelPinType::Make<FVoxelExec>());
			DummyOutputPin.MakeLinkTo(Pin);

			Graph.RemoveNode(Node);
		}

		for (FNode& Node : Graph.GetNodesCopy())
		{
			if (!Node.GetVoxelNode().IsA<FVoxelExecNode>())
			{
				continue;
			}

			FPin& ExecPin = Node.FindOutputChecked(VOXEL_PIN_NAME(FVoxelExecNode, ExecPin));
			if (ExecPin.GetLinkedTo().Num() > 0)
			{
				continue;
			}

			const FName Name = MergeVoxelNode->AddPinToArray(VOXEL_PIN_NAME(FVoxelNode_MergeExecs, ExecsPins));
			FPin& Pin = MergeNode.NewInputPin(Name, FVoxelPinType::Make<FVoxelExec>());
			ExecPin.MakeLinkTo(Pin);
		}
	}

	// Add an execute node that the runtime will use to execute the graph
	// Make sure to do so after iterating all nodes to not auto execute this node

	FNode& ExecuteNode = Graph.NewNode(FVoxelGraphNodeRef
	{
		&VoxelGraph,
		FVoxelNodeNames::ExecuteNodeId
	});

	ExecuteNode.SetVoxelNode(MakeVoxelShared<FVoxelRootExecuteNode>());
	ExecuteNode.NewInputPin(VOXEL_PIN_NAME(FVoxelRootExecuteNode, ExecInPin), FVoxelPinType::Make<FVoxelExec>());
	ExecuteNode.NewInputPin(VOXEL_PIN_NAME(FVoxelRootExecuteNode, EnableNodePin), FVoxelPinType::Make<bool>(), FVoxelPinValue::Make(true));
	ExecuteNode.NewOutputPin(VOXEL_PIN_NAME(FVoxelRootExecuteNode, ExecPin), FVoxelPinType::Make<FVoxelExec>());
	ExecuteNode.GetInputPin(0).MakeLinkTo(MergeNode.GetOutputPin(0));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelGraphCompiler::ReplaceTemplates(FGraph& Graph)
{
	VOXEL_FUNCTION_COUNTER();

	while (true)
	{
		if (!ReplaceTemplatesImpl(Graph))
		{
			// No nodes removed, exit
			break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelGraphCompiler::RemovePassthroughs(FGraph& Graph)
{
	VOXEL_FUNCTION_COUNTER();

	Graph.RemoveNodes([&](FNode& Node)
	{
		if (Node.Type != ENodeType::Passthrough)
		{
			return false;
		}

		const int32 Num = Node.GetInputPins().Num();
		check(Num == Node.GetOutputPins().Num());

		for (int32 Index = 0; Index < Num; Index++)
		{
			const FPin& InputPin = Node.GetInputPin(Index);
			const FPin& OutputPin = Node.GetOutputPin(Index);

			for (FPin& LinkedTo : OutputPin.GetLinkedTo())
			{
				InputPin.CopyInputPinTo(LinkedTo);
			}
		}

		return true;
	});
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelGraphCompiler::DisconnectVirtualPins(FGraph& Graph)
{
	VOXEL_FUNCTION_COUNTER();

	for (FNode& Node : Graph.GetNodes())
	{
		if (Node.Type != ENodeType::Struct)
		{
			continue;
		}

		for (FPin& Pin : Node.GetInputPins())
		{
			const TSharedPtr<const FVoxelPin> VoxelPin = Node.GetVoxelNode().FindPin(Pin.Name);
			if (!ensure(VoxelPin) ||
				!VoxelPin->Metadata.bVirtualPin)
			{
				continue;
			}
			ensure(Pin.Direction == EPinDirection::Input);

			Pin.BreakAllLinks();

			if (Pin.Type.HasPinDefaultValue())
			{
				Pin.SetDefaultValue(FVoxelPinValue(Pin.Type.GetPinDefaultValueType()));
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelGraphCompiler::RemoveUnusedNodes(FGraph& Graph)
{
	VOXEL_FUNCTION_COUNTER();

	TArray<const FNode*> Nodes;
	for (const FNode& Node : Graph.GetNodes())
	{
		if (Node.Type == ENodeType::Root)
		{
			Nodes.Add(&Node);
		}
	}
	ensure(Nodes.Num() == 1);

	TSet<const FNode*> ValidNodes;

	TArray<const FNode*> NodesToVisit = Nodes;
	while (NodesToVisit.Num() > 0)
	{
		const FNode* Node = NodesToVisit.Pop(false);
		if (ValidNodes.Contains(Node))
		{
			continue;
		}
		ValidNodes.Add(Node);

		for (const FPin& Pin : Node->GetInputPins())
		{
			for (const FPin& LinkedTo : Pin.GetLinkedTo())
			{
				NodesToVisit.Add(&LinkedTo.Node);
			}
		}
	}

	Graph.RemoveNodes([&](const FNode& Node)
	{
		return !ValidNodes.Contains(&Node);
	});
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelGraphCompiler::CheckForLoops(FGraph& Graph)
{
	VOXEL_FUNCTION_COUNTER();

	TSet<FNode*> VisitedNodes;
	TArray<FNode*> NodesToSort = Graph.GetNodesArray();

	while (NodesToSort.Num() > 0)
	{
		bool bHasRemovedNode = false;
		for (int32 Index = 0; Index < NodesToSort.Num(); Index++)
		{
			FNode* Node = NodesToSort[Index];

			const bool bHasInputPin = INLINE_LAMBDA
			{
				for (const FPin& Pin : Node->GetInputPins())
				{
					for (const FPin& LinkedTo : Pin.GetLinkedTo())
					{
						if (!VisitedNodes.Contains(&LinkedTo.Node))
						{
							return true;
						}
					}
				}
				return false;
			};

			if (bHasInputPin)
			{
				continue;
			}

			VisitedNodes.Add(Node);

			NodesToSort.RemoveAtSwap(Index);
			Index--;
			bHasRemovedNode = true;
		}

		if (!bHasRemovedNode &&
			NodesToSort.Num() > 0)
		{
			VOXEL_MESSAGE(Error, "Loop detected: {0}", NodesToSort);
			return;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool FVoxelGraphCompiler::ReplaceTemplatesImpl(FGraph& Graph)
{
	VOXEL_FUNCTION_COUNTER();

	for (FNode& Node : Graph.GetNodes())
	{
		if (Node.Type != ENodeType::Struct ||
			!Node.GetVoxelNode().IsA<FVoxelTemplateNode>())
		{
			continue;
		}

		bool bHasWildcardPin = false;
		for (const FPin& Pin : Node.GetPins())
		{
			if (Pin.Type.IsWildcard())
			{
				bHasWildcardPin = true;
			}
		}

		if (bHasWildcardPin)
		{
			// Can't replace template with a wildcard
			// If this node is unused, it'll be removed by RemoveUnusedNodes down the pipeline
			// If it is used, AddWildcardErrors will catch it
			continue;
		}

		FVoxelTemplateNodeContext Context(Node.NodeRef);

		ensure(!GVoxelTemplateNodeContext);
		GVoxelTemplateNodeContext = &Context;
		ON_SCOPE_EXIT
		{
			ensure(GVoxelTemplateNodeContext == &Context);
			GVoxelTemplateNodeContext = nullptr;
		};

		InitializeTemplatesPassthroughNodes(Graph, Node);

		Node.GetVoxelNode<FVoxelTemplateNode>().ExpandNode(Graph, Node);
		Graph.RemoveNode(Node);

		return true;
	}

	return false;
}

void FVoxelGraphCompiler::InitializeTemplatesPassthroughNodes(FGraph& Graph, FNode& Node)
{
	for (FPin& InputPin : Node.GetInputPins())
	{
		FNode& Passthrough = Graph.NewNode(ENodeType::Passthrough, FVoxelTemplateNodeUtilities::GetNodeRef());
		FPin& PassthroughInputPin = Passthrough.NewInputPin("Input" + InputPin.Name, InputPin.Type);
		FPin& PassthroughOutputPin = Passthrough.NewOutputPin(InputPin.Name, InputPin.Type);

		InputPin.CopyInputPinTo(PassthroughInputPin);

		InputPin.BreakAllLinks();
		InputPin.MakeLinkTo(PassthroughOutputPin);
	}

	for (FPin& OutputPin : Node.GetOutputPins())
	{
		FNode& Passthrough = Graph.NewNode(ENodeType::Passthrough, FVoxelTemplateNodeUtilities::GetNodeRef());
		FPin& PassthroughInputPin = Passthrough.NewInputPin(OutputPin.Name, OutputPin.Type);
		if (OutputPin.Type.HasPinDefaultValue())
		{
			PassthroughInputPin.SetDefaultValue(FVoxelPinValue(OutputPin.Type.GetPinDefaultValueType()));
		}
		FPin& PassthroughOutputPin = Passthrough.NewOutputPin("Output" + OutputPin.Name, OutputPin.Type);

		OutputPin.CopyOutputPinTo(PassthroughOutputPin);

		OutputPin.BreakAllLinks();
		OutputPin.MakeLinkTo(PassthroughInputPin);
	}
}