// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelRuntimeGraph.h"
#include "VoxelGraph.h"
#include "VoxelBuffer.h"
#include "VoxelRootNode.h"
#include "VoxelFunctionNode.h"
#include "VoxelGraphCompiler.h"
#include "VoxelFunctionCallNode.h"
#include "VoxelGraphCompileScope.h"
#if WITH_EDITOR
#include "EdGraph/EdGraph.h"
#endif

VOXEL_RUN_ON_STARTUP_GAME(RegisterOnNodeMessageLogged)
{
#if WITH_EDITOR
	FVoxelMessages::OnNodeMessageLogged.AddLambda([](const UEdGraph* Graph, const TSharedRef<FTokenizedMessage>& Message)
	{
		check(IsInGameThread());

		if (!Graph)
		{
			return;
		}

		const UVoxelGraph* VoxelGraph = Graph->GetTypedOuter<UVoxelGraph>();
		if (!VoxelGraph)
		{
			return;
		}

		VoxelGraph->GetRuntimeGraph().AddMessage(Message);
	});
#endif
}

#if WITH_EDITOR
IVoxelGraphEditorCompiler* GVoxelGraphEditorCompiler = nullptr;
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FName FVoxelRuntimeNode::GetNodeId() const
{
	if (!ensure(VoxelNode.IsValid()))
	{
		return {};
	}

	// NodeId needs to be unique
	// NodeId needs to avoid collision of nodes of different types: typically, function node with same node name but different function

	if (const FVoxelNode_FunctionCallOutput* OutputNode = VoxelNode->As<FVoxelNode_FunctionCallOutput>())
	{
		// Use Output.Name
		return FName("Output." + OutputNode->Name.ToString());
	}

	if (const FVoxelFunctionNode* FunctionNode = VoxelNode->As<FVoxelFunctionNode>())
	{
		if (const UFunction* Function = FunctionNode->GetFunction())
		{
			// Use Class.Function.NodeName
			return FName(Function->GetOuterUClass()->GetName() + "." + Function->GetName() + "." + EdGraphNodeName);
		}
	}

	// Use Struct.NodeName
	return FName(VoxelNode->GetStruct()->GetName() + "." + EdGraphNodeName);
}

FVoxelRuntimePin* FVoxelRuntimeNode::FindPin(const FName PinName, const bool bIsInput)
{
	TMap<FName, FVoxelRuntimePin>& Pins = bIsInput ? InputPins : OutputPins;
	return Pins.Find(PinName);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelRuntimePin* FVoxelRuntimeGraphData::FindPin(const FVoxelRuntimePinRef& Ref)
{
	FVoxelRuntimeNode* Node = NodeNameToNode.Find(Ref.NodeName);
	if (!Node)
	{
		return nullptr;
	}

	TMap<FName, FVoxelRuntimePin>& Pins = Ref.bIsInput ? Node->InputPins : Node->OutputPins;
	return Pins.Find(Ref.PinName);
}

const FVoxelNode* FVoxelRuntimeGraphData::FindMakeNode(const FVoxelPinType& Type) const
{
	const FVoxelInstancedStruct* Node = TypeToMakeNode.Find(Type);
	if (!ensure(Node) ||
		!ensure(Node->IsA<FVoxelNode>()))
	{
		return nullptr;
	}
	return &Node->Get<FVoxelNode>();
}

const FVoxelNode* FVoxelRuntimeGraphData::FindBreakNode(const FVoxelPinType& Type) const
{
	const FVoxelInstancedStruct* Node = TypeToBreakNode.Find(Type);
	if (!ensure(Node) ||
		!ensure(Node->IsA<FVoxelNode>()))
	{
		return nullptr;
	}
	return &Node->Get<FVoxelNode>();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedPtr<const Voxel::Graph::FGraph> UVoxelRuntimeGraph::GetRootGraph()
{
	if (CachedRootGraph.IsSet())
	{
		return CachedRootGraph.GetValue();
	}

	VOXEL_FUNCTION_COUNTER();
	check(IsInGameThread());
	using namespace Voxel::Graph;

	TSharedPtr<const FGraph> RootGraph = CreateRootGraph();

#if WITH_EDITOR
	if (ensure(GVoxelGraphEditorCompiler) &&
		!RootGraph)
	{
		// Try reconstructing if we errored out
		GVoxelGraphEditorCompiler->ReconstructAllNodes(*GetOuterUVoxelGraph());

		RootGraph = CreateRootGraph();
	}
#endif

	ReferencedGraphs.Reset();

	if (RootGraph)
	{
		for (const FNode& Node : RootGraph->GetNodes())
		{
			if (const FVoxelNode_FunctionCall* FunctionCall = Node.GetVoxelNode().As<FVoxelNode_FunctionCall>())
			{
				ReferencedGraphs.Add(FunctionCall->GraphInterface);
			}
		}
	}

	CachedRootGraph = RootGraph;
	return CachedRootGraph.GetValue();
}

TSharedPtr<FVoxelGraphExecutor> UVoxelRuntimeGraph::CreateExecutor(const FVoxelGraphPinRef& GraphPinRef)
{
	VOXEL_SCOPE_COUNTER_FORMAT("UVoxelRuntimeGraph::CreateExecutor %s", *GraphPinRef.ToString());
	check(IsInGameThread());
	using namespace Voxel::Graph;

	const TSharedPtr<const FGraph> RootGraph = GetRootGraph();
	if (!RootGraph)
	{
		return nullptr;
	}

	const TSharedRef<FGraph> Graph = RootGraph->Clone();

	FNode* RefNode = nullptr;
	for (FNode& Node : Graph->GetNodes())
	{
		if (Node.NodeRef != GraphPinRef.Node)
		{
			continue;
		}

		ensure(!RefNode);
		RefNode = &Node;
	}

	if (!RefNode)
	{
		// Don't raise error for deleted nodes
		// We can't always detect them (eg DebugNode being deleted)
		return FVoxelGraphExecutor::MakeDummy();
	}

	const FVoxelGraphCompileScope Scope(*GetOuterUVoxelGraph());
	ON_SCOPE_EXIT
	{
		if (Scope.GetMessages().Num() > 0)
		{
			PinRefToCompileMessages.Add(GraphPinRef, Scope.GetMessages());
		}
		else
		{
			PinRefToCompileMessages.Remove(GraphPinRef);
		}

		OnMessagesChanged.Broadcast();
	};

	FPin* RefPin = RefNode->FindInput(GraphPinRef.PinName);
	if (!RefPin)
	{
		// Array pin that was removed
		return nullptr;
	}

	if (RefPin->Type.IsWildcard())
	{
		VOXEL_MESSAGE(Error, "Wildcard pin {0} needs to be converted. Please connect it to another pin or right click it -> Convert", RefPin);
		return nullptr;
	}

	FNode& RootNode = Graph->NewNode(ENodeType::Root, RefNode->NodeRef);
	RefPin->CopyInputPinTo(RootNode.NewInputPin("Value", RefPin->Type));
	Graph->RemoveNode(*RefNode);

	FVoxelGraphCompiler::DisconnectVirtualPins(*Graph);
	FVoxelGraphCompiler::RemoveUnusedNodes(*Graph);

	if (Scope.HasError())
	{
		return nullptr;
	}

	Graph->Check();

	if (Scope.HasError())
	{
		return nullptr;
	}

	for (const FNode& Node : Graph->GetNodes())
	{
		Node.FlushErrors();
	}

	if (Scope.HasError())
	{
		return nullptr;
	}

	FNode& RuntimeRootNode = Graph->NewNode(RootNode.NodeRef);
	{
		const FVoxelPinType Type = RootNode.GetInputPin(0).Type;
		{
			const TSharedRef<FVoxelRootNode> RootVoxelNode = MakeVoxelShared<FVoxelRootNode>();
			RootVoxelNode->GetPin(RootVoxelNode->ValuePin).SetType(Type);
			RuntimeRootNode.SetVoxelNode(RootVoxelNode);
		}

		RootNode.GetInputPin(0).CopyInputPinTo(RuntimeRootNode.NewInputPin("Value", Type));
		Graph->RemoveNode(RootNode);
	}

	for (const FNode& Node : Graph->GetNodes())
	{
		if (!ensure(Node.Type == ENodeType::Struct))
		{
			VOXEL_MESSAGE(Error, "INTERNAL ERROR: Unexpected node {0}", Node);
			return nullptr;
		}
	}

	TMap<const FNode*, TSharedPtr<FVoxelNode>> Nodes;
	{
		VOXEL_SCOPE_COUNTER("Copy nodes");
		for (FNode& Node : Graph->GetNodes())
		{
			Nodes.Add(&Node, Node.GetVoxelNode().MakeSharedCopy());
		}
	}

	for (const auto& It : Nodes)
	{
		It.Value->InitializeNodeRuntime(It.Key->NodeRef, false);

		if (Scope.HasError())
		{
			return nullptr;
		}
	}

	for (const auto& It : Nodes)
	{
		const FNode& Node = *It.Key;
		FVoxelNode& VoxelNode = *It.Value;

		TArray<TSharedPtr<FVoxelPin>> Pins;
		VoxelNode.GetPinsMap().GenerateValueArray(Pins);

		ensure(Pins.Num() == Node.GetPins().Num());

		// Assign compute to input pins
		for (const TSharedPtr<FVoxelPin>& Pin : Pins)
		{
			if (!Pin->bIsInput ||
				Pin->Metadata.bVirtualPin)
			{
				continue;
			}

			const FPin& InputPin = Node.FindInputChecked(Pin->Name);
			if (!ensure(InputPin.Type == Pin->GetType()))
			{
				VOXEL_MESSAGE(Error, "{0}: Internal error", InputPin);
				return nullptr;
			}

			const FVoxelNodeRuntime::FPinData* OutputPinData = nullptr;
			if (InputPin.GetLinkedTo().Num() > 0)
			{
				check(InputPin.GetLinkedTo().Num() == 1);
				const FPin& OutputPin = InputPin.GetLinkedTo()[0];
				check(OutputPin.Direction == EPinDirection::Output);

				OutputPinData = &Nodes[&OutputPin.Node]->GetNodeRuntime().GetPinData(OutputPin.Name);
				check(!OutputPinData->bIsInput);
			}

			FVoxelNodeRuntime::FPinData& PinData = ConstCast(VoxelNode.GetNodeRuntime().GetPinData(Pin->Name));
			if (!ensure(PinData.Type == InputPin.Type))
			{
				VOXEL_MESSAGE(Error, "{0}: Internal error", InputPin);
				return nullptr;
			}

			if (OutputPinData)
			{
				PinData.Compute = OutputPinData->Compute;
			}
			else if (InputPin.Type.HasPinDefaultValue())
			{
				const FVoxelRuntimePinValue DefaultValue = FVoxelPinType::MakeRuntimeValueFromInnerValue(
					InputPin.Type,
					InputPin.GetDefaultValue());

				if (!ensureVoxelSlow(DefaultValue.IsValid()))
				{
					VOXEL_MESSAGE(Error, "{0}: Invalid default value", InputPin);
					return nullptr;
				}

				ensure(DefaultValue.GetType().CanBeCastedTo(InputPin.Type));

				if (VOXEL_DEBUG)
				{
					const FString DefaultValueString = InputPin.GetDefaultValue().ExportToString();

					FVoxelPinValue TestValue(InputPin.GetDefaultValue().GetType());
					ensure(TestValue.ImportFromString(DefaultValueString));
					ensure(TestValue == InputPin.GetDefaultValue());
				}
				if (VOXEL_DEBUG)
				{
					const FVoxelPinValue ExposedValue = FVoxelPinType::MakeExposedInnerValue(DefaultValue);
					ensure(InputPin.GetDefaultValue().GetType().CanBeCastedTo(ExposedValue.GetType()));

					// Types will be different when working with enums
					ensure(
						ExposedValue.GetType() != InputPin.GetDefaultValue().GetType() ||
						ExposedValue == InputPin.GetDefaultValue());
				}

				PinData.Compute = MakeVoxelShared<FVoxelComputeValue>([Value = FVoxelFutureValue(DefaultValue)](const FVoxelQuery&)
					{
						return Value;
					});
			}
			else
			{
				FVoxelRuntimePinValue Value(InputPin.Type);
				if (InputPin.Type.IsBufferArray())
				{
					// We don't want to have the default element
					ConstCast(Value.Get<FVoxelBuffer>()).SetAsEmpty();
				}

				PinData.Compute = MakeVoxelShared<FVoxelComputeValue>([FutureValue = FVoxelFutureValue(Value)](const FVoxelQuery&)
					{
						return FutureValue;
					});
			}
		}
	}

	for (const auto& It : Nodes)
	{
		It.Value->RemoveEditorData();
		It.Value->EnableSharedNode(It.Value.ToSharedRef());
	}

	TVoxelAddOnlySet<TSharedPtr<const FVoxelNode>> ExecutorNodes;
	for (const auto& It : Nodes)
	{
		ExecutorNodes.Add(It.Value);
	}

	const TSharedRef<FVoxelGraphExecutorInfo> GraphExecutorInfo = MakeVoxelShared<FVoxelGraphExecutorInfo>();
#if WITH_EDITOR
	GraphExecutorInfo->Graph_EditorOnly = Graph->Clone();
#endif
	GraphExecutorInfo->RuntimeInfo = this;

	return MakeVoxelShared<FVoxelGraphExecutor>(
		CastChecked<FVoxelRootNode>(Nodes[&RuntimeRootNode].ToSharedRef()),
		GraphExecutorInfo,
		MoveTemp(ExecutorNodes));
}

void UVoxelRuntimeGraph::ForceRecompile()
{
	VOXEL_FUNCTION_COUNTER();

	CachedRootGraph.Reset();
	(void)CreateRootGraph();

	// Do this after CreateRootGraph to ensure executor errors are up to date
	GVoxelGraphExecutorManager->NotifyGraphChanged(*GetOuterUVoxelGraph());
}

bool UVoxelRuntimeGraph::ShouldRecompile(const UVoxelGraphInterface& GraphThatChanged) const
{
	check(IsInGameThread());
	return
		&GraphThatChanged == GetOuterUVoxelGraph() ||
		ReferencedGraphs.Contains(&GraphThatChanged);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool UVoxelRuntimeGraph::HasNode(const UScriptStruct* Struct) const
{
	if (Data.NodeNameToNode.Num() == 0)
	{
		(void)ConstCast(this)->GetRootGraph();
	}

	for (const auto& It : Data.NodeNameToNode)
	{
		if (It.Value.VoxelNode &&
			It.Value.VoxelNode.GetScriptStruct()->IsChildOf(Struct))
		{
			return true;
		}
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void UVoxelRuntimeGraph::AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector)
{
	VOXEL_FUNCTION_COUNTER();
	using namespace Voxel::Graph;

	const TSharedPtr<const FGraph> Graph = CastChecked<UVoxelRuntimeGraph>(InThis)->CachedRootGraph.Get(nullptr);
	if (!Graph)
	{
		return;
	}

	FVoxelNullCheckReferenceCollector NullCheckCollector(Collector);

	for (const FNode& Node : Graph->GetNodes())
	{
		ConstCast(Node.GetVoxelNode()).AddStructReferencedObjects(NullCheckCollector);

		for (const FPin& Pin : Node.GetInputPins())
		{
			FVoxelObjectUtilities::AddStructReferencedObjects(
				NullCheckCollector,
				StaticStructFast<FVoxelPinValue>(),
				&ConstCast(Pin.GetDefaultValue()));
		}
	}
}

void UVoxelRuntimeGraph::Serialize(FArchive& Ar)
{
	VOXEL_FUNCTION_COUNTER();

	if (Ar.IsCooking())
	{
		// Make sure data is up to date
		(void)GetRootGraph();
	}

	Super::Serialize(Ar);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void UVoxelRuntimeGraph::AddMessage(const TSharedRef<FTokenizedMessage>& Message)
{
	check(IsInGameThread());

	if (GVoxelGraphCompileScope)
	{
		// Will be logged as compile error
		return;
	}

	RuntimeMessages.Add(Message);
	OnMessagesChanged.Broadcast();
}

bool UVoxelRuntimeGraph::HasCompileMessages() const
{
	return
		CompileMessages.Num() > 0 ||
		PinRefToCompileMessages.Num() > 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedPtr<const Voxel::Graph::FGraph> UVoxelRuntimeGraph::CreateRootGraph() const
{
	VOXEL_FUNCTION_COUNTER();
	check(IsInGameThread());
	ensure(!GVoxelGraphCompileScope);
	using namespace Voxel::Graph;

	const UVoxelGraph* VoxelGraph = GetOuterUVoxelGraph();
	check(VoxelGraph);

	const FVoxelGraphCompileScope Scope(*GetOuterUVoxelGraph());

	CompileMessages.Reset();
	RuntimeMessages.Reset();
	PinRefToCompileMessages.Reset();
	ON_SCOPE_EXIT
	{
		CompileMessages = Scope.GetMessages();
		OnMessagesChanged.Broadcast();
	};

#if WITH_EDITOR
	if (ensure(GVoxelGraphEditorCompiler))
	{
		ConstCast(this)->Data = GVoxelGraphEditorCompiler->Translate(*VoxelGraph);

		if (Scope.HasError())
		{
			return nullptr;
		}
	}
#endif

	const TSharedPtr<FVoxelGraphCompiler::FGraph> Graph = FVoxelGraphCompiler::TranslateRuntimeGraph(*this);
	if (Scope.HasError() ||
		!ensure(Graph))
	{
		return nullptr;
	}

#define RUN_PASS(Name, ...) \
	ensure(!Scope.HasError()); \
	\
	FVoxelGraphCompiler::Name(*Graph, ##__VA_ARGS__); \
	\
	if (Scope.HasError()) \
	{ \
		return nullptr;  \
	} \
	\
	Graph->Check(); \
	\
	if (Scope.HasError()) \
	{ \
		return nullptr;  \
	}

	RUN_PASS(RemoveSplitPins, *this);
	RUN_PASS(AddWildcardErrors);
	RUN_PASS(AddNoDefaultErrors);
	RUN_PASS(CheckParameters, *VoxelGraph);
	RUN_PASS(CheckInputs, *VoxelGraph);
	RUN_PASS(CheckOutputs, *VoxelGraph);
	RUN_PASS(AddPreviewNode, *this);
	RUN_PASS(AddDebugNodes, *this);
	RUN_PASS(AddToBuffer);
	RUN_PASS(RemoveLocalVariables, *VoxelGraph);
	RUN_PASS(CollapseInputs);
	RUN_PASS(AddRootExecuteNode, *VoxelGraph);
	RUN_PASS(ReplaceTemplates);
	RUN_PASS(RemovePassthroughs);
	RUN_PASS(CheckForLoops);

#undef RUN_PASS

	// Do this at the end, will assert before CheckOutputs if we have multiple outputs
	{
		TVoxelAddOnlySet<FVoxelGraphNodeRef> VisitedRefs;
		VisitedRefs.Reserve(Graph->GetNodes().Num());

		for (const FNode& Node : Graph->GetNodes())
		{
			if (VisitedRefs.Contains(Node.NodeRef))
			{
				ensure(false);
				VOXEL_MESSAGE(Error, "Duplicated node ref {0}", Node.NodeRef);
				continue;
			}

			VisitedRefs.Add(Node.NodeRef);
		}
	}

	if (Scope.HasError())
	{
		return nullptr;
	}

	return Graph->Clone();
}