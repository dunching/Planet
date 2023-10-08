// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelGraphCollapseToMacroSchemaAction.h"

#include "VoxelGraphSchema.h"
#include "VoxelGraphToolkit.h"
#include "VoxelGraphNodeBase.h"
#include "Nodes/VoxelGraphLocalVariableNode.h"
#include "Nodes/VoxelGraphParameterNodeBase.h"

#include "SNodePanel.h"
#include "Nodes/VoxelGraphMacroNode.h"
#include "Nodes/VoxelGraphParameterNode.h"

UEdGraphNode* FVoxelGraphSchemaAction_CollapseToMacro::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	const TSharedPtr<FVoxelGraphToolkit> Toolkit = UVoxelGraphSchema::GetToolkit(ParentGraph);
	if (!ensure(Toolkit))
	{
		return nullptr;
	}

	const TSharedPtr<SGraphEditor> GraphEditor = Toolkit->FindGraphEditor(ParentGraph);
	if (!ensure(GraphEditor))
	{
		return nullptr;
	}

	const FVoxelTransaction Transaction(Toolkit->Asset, "Collapse nodes to Macro");

	UVoxelGraph* NewMacroGraph;
	{
		FVoxelGraphSchemaAction_NewInlineMacro Action;
		// If we open newly created macro graph, undo action break parent graph and does not show recreated nodes
		Action.bOpenNewGraph = false;
		Action.PerformAction(Toolkit->GetActiveEdGraph(), nullptr, FVector2D::ZeroVector);
		NewMacroGraph = Action.NewMacro;
	}

	if (!ensure(NewMacroGraph))
	{
		return nullptr;
	}

	const FGraphPanelSelectionSet& SelectedNodes = GraphEditor->GetSelectedNodes();
	GroupSelectedNodes(SelectedNodes);

	// Copy and Paste
	{
		FString ExportedText;
		ExportNodes(ExportedText);
		ImportNodes(NewMacroGraph, ExportedText);
	}

	UpdateParameters(NewMacroGraph);
	AddParameterInputs(NewMacroGraph);
	AddDeclarationOutputs(NewMacroGraph);
	AddOuterParameters(NewMacroGraph);

	UVoxelGraphMacroNode* NewMacroNode;
	{
		FVoxelGraphSchemaAction_NewMacroNode Action;
		Action.Graph = NewMacroGraph;
		NewMacroNode = Cast<UVoxelGraphMacroNode>(Action.PerformAction(ParentGraph, nullptr, AvgNodePosition));
	}

	if (!ensure(NewMacroNode))
	{
		return nullptr;
	}

	FixupMainGraph(NewMacroNode, ParentGraph);

	GraphEditor->NotifyGraphChanged();
	Toolkit->OnGraphChanged(ParentGraph);

	return NewMacroNode;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelGraphSchemaAction_CollapseToMacro::GroupSelectedNodes(const TSet<UObject*>& SelectedNodes)
{
	for (UObject* SelectedNode : SelectedNodes)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(SelectedNode);
		if (!ensure(Node))
		{
			continue;
		}

		TMap<FEdGraphPinReference, TSet<FEdGraphPinReference>> OuterPins;
		for (UEdGraphPin* Pin : Node->Pins)
		{
			if (!Pin->HasAnyConnections())
			{
				continue;
			}

			for (const UEdGraphPin* LinkedTo : Pin->LinkedTo)
			{
				if (!ensure(LinkedTo))
				{
					continue;
				}

				const UEdGraphNode* LinkedToNode = LinkedTo->GetOwningNode();
				if (!ensure(LinkedToNode))
				{
					continue;
				}

				if (!SelectedNodes.Contains(LinkedToNode))
				{
					OuterPins.FindOrAdd(Pin).Add(LinkedTo);
				}
			}
		}

		TSharedRef<FCopiedNode> CopiedNode = MakeShared<FCopiedNode>(Node, OuterPins);
		CopiedNodes.Add(Node->NodeGuid, CopiedNode);

		if (const UVoxelGraphParameterNodeBase* ParameterNode = Cast<UVoxelGraphParameterNodeBase>(Node))
		{
			TSharedPtr<FCopiedParameter> CopiedParameter = CopiedParameters.FindRef(ParameterNode->Guid);
			if (!CopiedParameter)
			{
				FVoxelGraphParameter* Parameter = ParameterNode->GetParameter();
				if (!ensure(Parameter))
				{
					continue;
				}

				CopiedParameter = MakeShared<FCopiedParameter>(Parameter->ParameterType, ParameterNode->Guid);
				CopiedParameters.Add(ParameterNode->Guid, CopiedParameter);
			}

			if (ParameterNode->IsA<UVoxelGraphLocalVariableDeclarationNode>())
			{
				CopiedParameter->DeclarationNode = CopiedNode;
			}
			else
			{
				CopiedParameter->UsageNodes.Add(CopiedNode);
			}
		}
	}
}

void FVoxelGraphSchemaAction_CollapseToMacro::UpdateParameters(UVoxelGraph* Graph)
{
	for (const auto& It : CopiedParameters)
	{
		if (!ensure(It.Value->NewParameterId.IsValid()))
		{
			continue;
		}

		if (It.Value->ParameterType != EVoxelGraphParameterType::Parameter)
		{
			continue;
		}

		FGuid OldParameterId = It.Value->NewParameterId;

		{
			FVoxelGraphParameter* ExistingParameter = Graph->FindParameterByGuid(OldParameterId);
			if (!ensure(ExistingParameter))
			{
				continue;
			}

			const FVoxelTransaction Transaction(Graph, "Change parameter to local variable");

			FVoxelGraphParameter NewParameter = *ExistingParameter;
			NewParameter.ParameterType = EVoxelGraphParameterType::LocalVariable;
			NewParameter.Guid = FGuid::NewGuid();

			ExistingParameter->Name.SetNumber(ExistingParameter->Name.GetNumber() + 1);

			Graph->Parameters.Add(NewParameter);

			It.Value->NewParameterId = NewParameter.Guid;
		}

		const FVoxelGraphParameter* Parameter = Graph->FindParameterByGuid(It.Value->NewParameterId);
		if (!ensure(Parameter))
		{
			continue;
		}

		for (const TSharedPtr<FCopiedNode>& UsageNode : It.Value->UsageNodes)
		{
			UVoxelGraphParameterNodeBase* Node = UsageNode->GetNewNode<UVoxelGraphParameterNodeBase>();
			if (!ensure(Node))
			{
				continue;
			}

			FVoxelGraphSchemaAction_NewParameterUsage Action;
			Action.bLocalVariable_IsDeclaration = false;
			Action.Guid = Parameter->Guid;
			Action.ParameterType = EVoxelGraphParameterType::LocalVariable;
			Action.PinType = Parameter->Type;

			UVoxelGraphNodeBase* NewLocalVariableNode = Cast<UVoxelGraphNodeBase>(Action.PerformAction(Graph->MainEdGraph, nullptr, FVector2D(Node->NodePosX, Node->NodePosY), false));
			if (!ensure(NewLocalVariableNode))
			{
				continue;
			}

			UsageNode->NewNode = NewLocalVariableNode;

			const FVoxelTransaction Transaction(NewLocalVariableNode, "Fixup Local Variable node");

			if (Node->Pins.Num() > 1)
			{
				if (ensure(NewLocalVariableNode->CanSplitPin(*NewLocalVariableNode->GetPinAt(0))))
				{
					NewLocalVariableNode->SplitPin(*NewLocalVariableNode->GetPinAt(0));
				}
			}

			for (UEdGraphPin* Pin : Node->Pins)
			{
				const FName TargetPinName = Pin->PinName == STATIC_FNAME("Value") ? STATIC_FNAME("OutputPin") : Pin->PinName;
				UEdGraphPin* NewPin = NewLocalVariableNode->FindPin(TargetPinName, Pin->Direction);

				for (UEdGraphPin* LinkedToPin : Pin->LinkedTo)
				{
					if (!ensure(LinkedToPin))
					{
						continue;
					}

					NewPin->MakeLinkTo(LinkedToPin);
				}
			}

			NewLocalVariableNode->bCommentBubblePinned = Node->bCommentBubblePinned;
			NewLocalVariableNode->bCommentBubbleVisible = Node->bCommentBubbleVisible;
			NewLocalVariableNode->bCommentBubbleMakeVisible = Node->bCommentBubbleMakeVisible;
			NewLocalVariableNode->NodeComment = Node->NodeComment;

			Node->DestroyNode();
		}

		const int32 Index = Graph->Parameters.IndexOfByKey(OldParameterId);
		if (Index != -1)
		{
			const FVoxelTransaction Transaction(Graph, "Remove parameter");
			Graph->Parameters.RemoveAt(Index);
		}
	}
}

void FVoxelGraphSchemaAction_CollapseToMacro::AddParameterInputs(UVoxelGraph* Graph)
{
	for (const auto& It : CopiedParameters)
	{
		if (It.Value->ParameterType == EVoxelGraphParameterType::Input ||
			It.Value->ParameterType == EVoxelGraphParameterType::Output ||
			It.Value->DeclarationNode)
		{
			continue;
		}

		FVoxelGraphParameter* Parameter = Graph->FindParameterByGuid(It.Value->NewParameterId);
		if (!ensure(Parameter))
		{
			continue;
		}

		FVector2D Position = InputDeclarationPosition;
		InputDeclarationPosition.Y += 250.f;

		// We create macro inputs for local variables / parameters and assign them to newly created local variables, to use them in multiple places
		UVoxelGraphParameterNodeBase* MacroInput;
		{
			FVoxelGraphSchemaAction_NewParameter Action;
			Action.ParameterType = EVoxelGraphParameterType::Input;
			Action.PinType = Parameter->Type;
			Action.ParameterName = Parameter->Name;
			Parameter->Name = *(Parameter->Name.ToString() + "_Local");

			MacroInput = Cast<UVoxelGraphParameterNodeBase>(Action.PerformAction(Graph->MainEdGraph, nullptr, Position, false));
		}

		if (!ensure(MacroInput))
		{
			continue;
		}

		UVoxelGraphLocalVariableDeclarationNode* LocalVariableDeclaration;
		{
			FVoxelGraphSchemaAction_NewParameterUsage Action;
			Action.ParameterType = EVoxelGraphParameterType::LocalVariable;
			Action.Guid = It.Value->NewParameterId;
			Action.bLocalVariable_IsDeclaration = true;

			Position.X += 350.f;

			LocalVariableDeclaration = Cast<UVoxelGraphLocalVariableDeclarationNode>(Action.PerformAction(Graph->MainEdGraph, MacroInput->GetOutputPin(0), Position, false));
		}

		if (!ensure(LocalVariableDeclaration))
		{
			MacroInput->DestroyNode();
			continue;
		}

		It.Value->InputId = MacroInput->Guid;
	}
}

void FVoxelGraphSchemaAction_CollapseToMacro::AddDeclarationOutputs(UVoxelGraph* Graph)
{
	for (const auto& It : CopiedParameters)
	{
		if (!It.Value->DeclarationNode)
		{
			continue;
		}

		if (!ensure(It.Value->ParameterType == EVoxelGraphParameterType::LocalVariable))
		{
			continue;
		}

		const FVoxelGraphParameter* Parameter = Graph->FindParameterByGuid(It.Value->NewParameterId);
		if (!ensure(Parameter))
		{
			continue;
		}

		FVector2D Position = OutputDeclarationPosition;
		OutputDeclarationPosition.Y += 250.f;

		UVoxelGraphLocalVariableUsageNode* LocalVariableUsage;
		{
			FVoxelGraphSchemaAction_NewParameterUsage Action;
			Action.ParameterType = EVoxelGraphParameterType::LocalVariable;
			Action.Guid = It.Value->NewParameterId;
			Action.bLocalVariable_IsDeclaration = false;

			LocalVariableUsage = Cast<UVoxelGraphLocalVariableUsageNode>(Action.PerformAction(Graph->MainEdGraph, nullptr, Position, false));
		}

		if (!ensure(LocalVariableUsage))
		{
			continue;
		}

		// We create macro inputs for local variables / parameters and assign them to newly created local variables, to use them in multiple places
		UVoxelGraphParameterNodeBase* MacroOutput;
		{
			FVoxelGraphSchemaAction_NewParameter Action;
			Action.ParameterType = EVoxelGraphParameterType::Output;
			Action.PinType = Parameter->Type;
			Action.ParameterName = Parameter->Name;

			Position.X += 350.f;

			MacroOutput = Cast<UVoxelGraphParameterNodeBase>(Action.PerformAction(Graph->MainEdGraph, LocalVariableUsage->GetOutputPin(0), Position, false));
		}

		if (!ensure(MacroOutput))
		{
			LocalVariableUsage->DestroyNode();
			continue;
		}

		It.Value->OutputId = MacroOutput->Guid;
	}
}

void FVoxelGraphSchemaAction_CollapseToMacro::AddOuterParameters(UVoxelGraph* Graph)
{
	for (const auto& It : CopiedNodes)
	{
		const UVoxelGraphNodeBase* NewNode = It.Value->GetNewNode<UVoxelGraphNodeBase>();
		const UVoxelGraphNodeBase* OriginalNode = It.Value->GetOriginalNode<UVoxelGraphNodeBase>();
		if (!ensure(NewNode) ||
			!ensure(OriginalNode))
		{
			continue;
		}

		for (auto& OuterPinIt : It.Value->OutsideConnectedPins)
		{
			const UEdGraphPin* OriginalPin = OuterPinIt.Key.Get();
			if (!ensure(OriginalPin))
			{
				continue;
			}

			FName TargetPin = OriginalPin->PinName;

			// Parameter node pin is named Value and since we reconstruct parameter
			// nodes as Local Variable nodes, latter ones has OutputPin instead of Value
			if (OriginalNode->IsA<UVoxelGraphParameterNode>() &&
				TargetPin == STATIC_FNAME("Value"))
			{
				TargetPin = STATIC_FNAME("OutputPin");
			}

			UEdGraphPin* NewPin = NewNode->FindPin(TargetPin, OriginalPin->Direction);
			if (!ensure(NewPin))
			{
				continue;
			}

			// We create macro inputs/outputs for pins which were connected to outer nodes
			const UVoxelGraphParameterNodeBase* MacroInputOutput = nullptr;
			for (const FEdGraphPinReference& WeakOuterPin : OuterPinIt.Value)
			{
				const UEdGraphPin* OuterPin = WeakOuterPin.Get();
				if (!ensure(OuterPin))
				{
					continue;
				}

				UEdGraphNode* OuterNode = OuterPin->GetOwningNode();
				if (!ensure(OuterNode))
				{
					continue;
				}

				// If pin is input and does reference already created parameter, connect it to its usage
				if (NewPin->Direction == EGPD_Input)
				{
					if (const UVoxelGraphParameterNodeBase* OuterParameterNode = Cast<UVoxelGraphParameterNodeBase>(OuterNode))
					{
						if (const TSharedPtr<FCopiedParameter>& CopiedParameter = CopiedParameters.FindRef(OuterParameterNode->Guid))
						{
							FVoxelGraphSchemaAction_NewParameterUsage Action;
							Action.bLocalVariable_IsDeclaration = false;
							Action.Guid = CopiedParameter->NewParameterId;
							Action.ParameterType = EVoxelGraphParameterType::LocalVariable;
							Action.PerformAction(Graph->MainEdGraph, NewPin, GetNodePosition(OuterParameterNode), false);
							continue;
						}
					}
				}

				if (!MacroInputOutput)
				{
					FVoxelGraphSchemaAction_NewParameter Action;
					FVector2D Position(NewNode->NodePosX, NewNode->NodePosY - 200.f);
					if (NewPin->Direction == EGPD_Input)
					{
						Action.ParameterType = EVoxelGraphParameterType::Input;
						Position.X -= 350.f;
						Position.Y += NewNode->GetInputPins().IndexOfByKey(NewPin) * 100.f;
					}
					else
					{
						Action.ParameterType = EVoxelGraphParameterType::Output;
						Position.X += 350.f;
						Position.Y += NewNode->GetOutputPins().IndexOfByKey(NewPin) * 100.f;
					}
					Action.PinType = NewPin->PinType;
					MacroInputOutput = Cast<UVoxelGraphParameterNodeBase>(Action.PerformAction(Graph->MainEdGraph, NewPin, Position, false));
				}

				if (!ensure(MacroInputOutput))
				{
					continue;
				}

				It.Value->MappedInputsOutputs.FindOrAdd(MacroInputOutput->Guid, {}).Add(WeakOuterPin);
			}
		}
	}
}

void FVoxelGraphSchemaAction_CollapseToMacro::FixupMainGraph(const UVoxelGraphMacroNode* MacroNode, UEdGraph* EdGraph)
{
	// Remove all original nodes
	{
		for (const auto& It : CopiedNodes)
		{
			UEdGraphNode* Node = It.Value->GetOriginalNode<UEdGraphNode>();
			if (!ensure(Node))
			{
				continue;
			}

			Node->DestroyNode();
		}
	}

	// Create outer node links
	{
		for (const auto& It : CopiedNodes)
		{
			for (auto& OuterLinkIt : It.Value->MappedInputsOutputs)
			{
				UEdGraphPin* MacroPin = MacroNode->FindPin(OuterLinkIt.Key.ToString());
				if (!ensure(MacroPin))
				{
					continue;
				}

				for (const FEdGraphPinReference& WeakOuterPin : OuterLinkIt.Value)
				{
					UEdGraphPin* OuterPin = WeakOuterPin.Get();
					if (!ensure(OuterPin))
					{
						continue;
					}

					MacroPin->MakeLinkTo(OuterPin);
				}
			}
		}
	}

	// Link parameters/local variables
	{
		for (const auto& It : CopiedParameters)
		{
			if (It.Value->InputId.IsValid())
			{
				UEdGraphPin* InputPin = MacroNode->FindPin(It.Value->InputId.ToString());
				if (!ensure(InputPin))
				{
					continue;
				}

				if (InputPin->LinkedTo.Num() > 0)
				{
					continue;
				}

				FVoxelGraphSchemaAction_NewParameterUsage Action;
				Action.ParameterType = It.Value->ParameterType;
				Action.Guid = It.Value->OriginalParameterId;
				Action.bLocalVariable_IsDeclaration = false;

				FVector2D Position(AvgNodePosition.X - 200.f, AvgNodePosition.Y);
				Position.Y += MacroNode->GetInputPins().IndexOfByKey(InputPin) * 100.f;

				ensure(Action.PerformAction(EdGraph, InputPin, Position, false));
			}

			if (It.Value->OutputId.IsValid())
			{
				UEdGraphPin* OutputPin = MacroNode->FindPin(It.Value->OutputId.ToString());
				if (!ensure(OutputPin))
				{
					continue;
				}

				if (OutputPin->LinkedTo.Num() > 0)
				{
					continue;
				}

				ensure(It.Value->ParameterType == EVoxelGraphParameterType::LocalVariable);

				FVoxelGraphSchemaAction_NewParameterUsage Action;
				Action.ParameterType = It.Value->ParameterType;
				Action.Guid = It.Value->OriginalParameterId;
				Action.bLocalVariable_IsDeclaration = true;

				FVector2D Position(AvgNodePosition.X + 200.f, AvgNodePosition.Y);
				Position.Y += MacroNode->GetInputPins().IndexOfByKey(OutputPin) * 100.f;

				ensure(Action.PerformAction(EdGraph, OutputPin, Position, false));
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelGraphSchemaAction_CollapseToMacro::ExportNodes(FString& ExportText) const
{
	TSet<UObject*> Nodes;
	for (const auto& It : CopiedNodes)
	{
		UEdGraphNode* Node = It.Value->OriginalNode.Get();
		if (!ensure(Node) ||
			!Node->CanDuplicateNode())
		{
			continue;
		}

		Node->PrepareForCopying();
		Nodes.Add(Node);
	}

	FEdGraphUtilities::ExportNodesToText(Nodes, ExportText);
}

void FVoxelGraphSchemaAction_CollapseToMacro::ImportNodes(UVoxelGraph* Graph, const FString& ExportText)
{
	const FVoxelTransaction Transaction(Graph, "Paste nodes");

	TSet<UEdGraphNode*> PastedNodes;

	// Import the nodes
	FEdGraphUtilities::ImportNodesFromText(Graph->MainEdGraph, ExportText, PastedNodes);

	// Average position of nodes so we can move them while still maintaining relative distances to each other
	AvgNodePosition = FVector2D::ZeroVector;

	for (auto It = PastedNodes.CreateIterator(); It; ++It)
	{
		UEdGraphNode* Node = *It;
		if (!ensure(Node))
		{
			It.RemoveCurrent();
			continue;
		}

		AvgNodePosition.X += Node->NodePosX;
		AvgNodePosition.Y += Node->NodePosY;

		UVoxelGraphNodeBase* VoxelNode = Cast<UVoxelGraphNodeBase>(Node);
		if (!VoxelNode ||
			VoxelNode->CanPasteVoxelNode(PastedNodes))
		{
			continue;
		}

		VoxelNode->DestroyNode();
		It.RemoveCurrent();
	}

	if (PastedNodes.Num() > 0)
	{
		AvgNodePosition.X /= PastedNodes.Num();
		AvgNodePosition.Y /= PastedNodes.Num();
	}

	// Top left corner of all nodes
	InputDeclarationPosition.X = MAX_FLT;
	InputDeclarationPosition.Y = MAX_FLT;

	// Top right corner of all nodes
	OutputDeclarationPosition.X = -MAX_FLT;
	OutputDeclarationPosition.Y = MAX_FLT;

	for (UEdGraphNode* Node : PastedNodes)
	{
		const FVector2D NodePosition = GetNodePosition(Node);
		Node->NodePosX = NodePosition.X;
		Node->NodePosY = NodePosition.Y;

		InputDeclarationPosition.X = FMath::Min(Node->NodePosX, InputDeclarationPosition.X);
		InputDeclarationPosition.Y = FMath::Min(Node->NodePosY, InputDeclarationPosition.Y);

		OutputDeclarationPosition.X = FMath::Max(Node->NodePosX, OutputDeclarationPosition.X);
		OutputDeclarationPosition.Y = FMath::Min(Node->NodePosY, OutputDeclarationPosition.Y);

		Node->SnapToGrid(SNodePanel::GetSnapGridSize());

		const TSharedPtr<FCopiedNode> CopiedNode = CopiedNodes.FindRef(Node->NodeGuid);
		if (ensure(CopiedNode))
		{
			CopiedNode->NewNode = Node;

			if (const UVoxelGraphParameterNodeBase* OriginalNode = CopiedNode->GetOriginalNode<UVoxelGraphParameterNodeBase>())
			{
				const TSharedPtr<FCopiedParameter> CopiedParameter = CopiedParameters.FindRef(OriginalNode->Guid);
				const UVoxelGraphParameterNodeBase* NewParameterNode = Cast<UVoxelGraphParameterNodeBase>(Node);
				if (ensure(CopiedParameter) &&
					ensure(NewParameterNode))
				{
					CopiedParameter->NewParameterId = NewParameterNode->Guid;
				}
			}
		}

		// Give new node a different Guid from the old one
		Node->CreateNewGuid();
	}

	// Post paste for local variables
	for (UEdGraphNode* Node : PastedNodes)
	{
		if (UVoxelGraphNodeBase* VoxelNode = Cast<UVoxelGraphNodeBase>(Node))
		{
			VoxelNode->PostPasteVoxelNode(PastedNodes);
		}
	}

	InputDeclarationPosition.X -= 800.f;
	OutputDeclarationPosition.X += 800.f;
}

FVector2D FVoxelGraphSchemaAction_CollapseToMacro::GetNodePosition(const UEdGraphNode* Node) const
{
	return FVector2D(Node->NodePosX - AvgNodePosition.X, Node->NodePosY - AvgNodePosition.Y);
}