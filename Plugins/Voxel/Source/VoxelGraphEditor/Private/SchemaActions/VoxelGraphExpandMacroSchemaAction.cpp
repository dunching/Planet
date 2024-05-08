// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelGraphExpandMacroSchemaAction.h"

#include "VoxelGraphSchema.h"
#include "VoxelGraphToolkit.h"

#include "Nodes/VoxelGraphMacroNode.h"
#include "Nodes/VoxelGraphMacroParameterNode.h"

UEdGraphNode* FVoxelGraphSchemaAction_ExpandMacro::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
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

	UVoxelGraph* WorkingGraph = Toolkit->Asset->FindGraph(ParentGraph);
	if (!ensure(GraphEditor->GetNumberOfSelectedNodes() == 1))
	{
		return nullptr;
	}

	UVoxelGraphMacroNode* MacroNode = Cast<UVoxelGraphMacroNode>(GraphEditor->GetSingleSelectedNode());
	if (!ensure(MacroNode))
	{
		return nullptr;
	}

	if (!MacroNode->GraphInterface)
	{
		return nullptr;
	}

	const UVoxelGraph* MacroGraph = MacroNode->GraphInterface->GetGraph();
	if (!ensure(MacroGraph))
	{
		return nullptr;
	}

	const FVoxelTransaction Transaction(Toolkit->Asset, "Expand Macro");
	FVoxelGraphDelayOnGraphChangedScope OnGraphChangedScope;

	FindNearestSuitableLocation(GraphEditor, MacroNode);
	GroupNodesToCopy(MacroGraph->MainEdGraph);

	// Copy and Paste
	{
		FString ExportedText;
		ExportNodes(ExportedText);
		ImportNodes(Toolkit->FindGraphEditor(ParentGraph), WorkingGraph, ExportedText);
	}

	ConnectNewNodes(MacroNode);

	// Add comment to surround all macro nodes
	{
		TSet<TWeakObjectPtr<UObject>> SelectedNodes;
		for (UObject* Obj : GraphEditor->GetSelectedNodes())
		{
			SelectedNodes.Add(Obj);
		}

		FVoxelGraphSchemaAction_NewComment CommentAction;
		UEdGraphNode_Comment* NewComment = Cast<UEdGraphNode_Comment>(CommentAction.PerformAction(ParentGraph, nullptr, SuitablePosition));
		NewComment->NodeComment = MacroGraph->GetGraphName() + " expanded nodes";

		// We have to wait for slate to process newly created nodes desired size to wrap comment around them
		FVoxelSystemUtilities::DelayedCall([WeakComment = MakeWeakObjectPtr(NewComment), WeakGraphEditor = MakeWeakPtr(GraphEditor), SelectedNodes]
		{
			UEdGraphNode_Comment* Comment = WeakComment.Get();
			const TSharedPtr<SGraphEditor> PinnedGraphEditor = WeakGraphEditor.Pin();
			if (!ensure(Comment) ||
				!ensure(PinnedGraphEditor))
			{
				return;
			}

			PinnedGraphEditor->ClearSelectionSet();
			for (const TWeakObjectPtr<UObject>& WeakNode : SelectedNodes)
			{
				if (UEdGraphNode* Node = Cast<UEdGraphNode>(WeakNode.Get()))
				{
					PinnedGraphEditor->SetNodeSelection(Node, true);
				}
			}

			FSlateRect Bounds;
			if (PinnedGraphEditor->GetBoundsForSelectedNodes(Bounds, 50.f))
			{
				Comment->PreEditChange(nullptr);
				Comment->SetBounds(Bounds);
				Comment->PostEditChange();
			}

			PinnedGraphEditor->NotifyGraphChanged();
			PinnedGraphEditor->ZoomToFit(true);
		}, 0.1f);
	}

	MacroNode->DestroyNode();

	GraphEditor->NotifyGraphChanged();

	return nullptr;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelGraphSchemaAction_ExpandMacro::FindNearestSuitableLocation(const TSharedPtr<SGraphEditor>& GraphEditor, const UVoxelGraphMacroNode* MacroNode)
{
	FSlateRect Bounds(-FLT_MAX, -FLT_MAX, FLT_MAX, FLT_MAX);

	GraphEditor->SelectAllNodes();
	ensure(GraphEditor->GetBoundsForSelectedNodes(Bounds, 50.f));

	auto GetDiffValue = [](const int32 Value, const int32 MinValue, const int32 MaxValue, int32& OutDiffValue, int32& OutValue, int32& OutDirection)
	{
		if (FMath::Abs(Value - MinValue) > FMath::Abs(Value - MaxValue))
		{
			OutDiffValue = FMath::Abs(Value - MaxValue);
			OutValue = MaxValue;
			OutDirection = 1;
		}
		else
		{
			OutDiffValue = FMath::Abs(Value - MinValue);
			OutValue = MinValue;
			OutDirection = -1;
		}
	};

	FIntPoint Diff;
	FIntPoint Location;
	FIntPoint Direction;
	GetDiffValue(MacroNode->NodePosX, Bounds.Left, Bounds.Right, Diff.X, Location.X, Direction.X);
	GetDiffValue(MacroNode->NodePosY, Bounds.Top, Bounds.Bottom, Diff.Y, Location.Y, Direction.Y);

	FIntPoint AvgPosition = FIntPoint::ZeroValue;
	int32 NodesCount = 0;
	for (const UEdGraphNode* Node : MacroNode->GraphInterface->GetGraph()->MainEdGraph->Nodes)
	{
		if (!ensure(Node))
		{
			continue;
		}

		if (Node->IsA<UVoxelGraphMacroParameterInputNode>() ||
			Node->IsA<UVoxelGraphMacroParameterOutputNode>())
		{
			continue;
		}

		AvgPosition.X += Node->NodePosX;
		AvgPosition.Y += Node->NodePosY;
		NodesCount++;
	}

	AvgPosition /= FMath::Max(1, NodesCount);

	if (Diff.X < Diff.Y)
	{
		SuitablePosition = { Location.X + (AvgPosition.X + 350) * Direction.X, MacroNode->NodePosY };
	}
	else
	{
		SuitablePosition = { MacroNode->NodePosX, Location.Y + (AvgPosition.Y + 350) * Direction.Y };
	}
}

void FVoxelGraphSchemaAction_ExpandMacro::GroupNodesToCopy(UEdGraph* EdGraph)
{
	for (UEdGraphNode* Node : EdGraph->Nodes)
	{
		if (!ensure(Node))
		{
			continue;
		}

		if (Node->IsA<UVoxelGraphMacroParameterInputNode>() ||
			Node->IsA<UVoxelGraphMacroParameterOutputNode>())
		{
			continue;
		}

		TSharedRef<FCopiedNode> NodeToCopy = MakeShared<FCopiedNode>();
		NodeToCopy->OriginalNode = Node;

		for (UEdGraphPin* Pin : Node->Pins)
		{
			if (!Pin->HasAnyConnections())
			{
				continue;
			}

			for (const UEdGraphPin* LinkedPin : Pin->LinkedTo)
			{
				if (!ensure(LinkedPin))
				{
					continue;
				}

				const UVoxelGraphParameterNodeBase* LinkedNode = Cast<UVoxelGraphParameterNodeBase>(LinkedPin->GetOwningNode());
				if (!LinkedNode)
				{
					continue;
				}

				if (LinkedNode->GetParameterType() != EVoxelGraphParameterType::Input &&
					LinkedNode->GetParameterType() != EVoxelGraphParameterType::Output)
				{
					continue;
				}

				NodeToCopy->MappedOriginalPinsToParams.FindOrAdd(Pin, {}).Add(LinkedNode->Guid);
			}
		}

		CopiedNodes.Add(Node->NodeGuid, NodeToCopy);
	}
}

void FVoxelGraphSchemaAction_ExpandMacro::ConnectNewNodes(const UVoxelGraphMacroNode* MacroNode)
{
	for (const auto& It : CopiedNodes)
	{
		const UEdGraphNode* NewNode = It.Value->NewNode.Get();
		if (!ensure(NewNode))
		{
			continue;
		}

		for (const auto& MappedIt : It.Value->MappedOriginalPinsToParams)
		{
			const UEdGraphPin* OriginalPin = MappedIt.Key.Get();
			if (!ensure(OriginalPin))
			{
				continue;
			}

			UEdGraphPin* NewPin = NewNode->FindPin(OriginalPin->PinName, OriginalPin->Direction);
			if (!ensure(NewPin))
			{
				continue;
			}

			for (FGuid MacroParameterId : MappedIt.Value)
			{
				UEdGraphPin* MacroPin = MacroNode->FindPin(MacroParameterId.ToString());
				if (!ensure(MacroPin))
				{
					continue;
				}

				TArray<UEdGraphPin*> LinkedPins = MacroPin->LinkedTo;
				MacroPin->BreakAllPinLinks();

				for (UEdGraphPin* LinkedPin : LinkedPins)
				{
					NewPin->MakeLinkTo(LinkedPin);
				}
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelGraphSchemaAction_ExpandMacro::ExportNodes(FString& ExportText) const
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

void FVoxelGraphSchemaAction_ExpandMacro::ImportNodes(const TSharedPtr<SGraphEditor>& GraphEditor, UVoxelGraph* Graph, const FString& ExportText)
{
	FVoxelTransaction Transaction(Graph->MainEdGraph);

	// Clear the selection set
	GraphEditor->ClearSelectionSet();

	TSet<UEdGraphNode*> PastedNodes;

	// Import the nodes
	FEdGraphUtilities::ImportNodesFromText(Graph->MainEdGraph, ExportText, PastedNodes);

	for (auto It = PastedNodes.CreateIterator(); It; ++It)
	{
		UEdGraphNode* Node = *It;
		if (!ensure(Node))
		{
			It.RemoveCurrent();
			continue;
		}

		UVoxelGraphNodeBase* VoxelNode = Cast<UVoxelGraphNodeBase>(Node);
		if (!VoxelNode ||
			VoxelNode->CanPasteVoxelNode(PastedNodes))
		{
			continue;
		}

		VoxelNode->DestroyNode();
		It.RemoveCurrent();
	}

	TMap<FGuid, FGuid> UpdatedParameters;
	for (UEdGraphNode* Node : PastedNodes)
	{
		Node->NodePosX += SuitablePosition.X;
		Node->NodePosY += SuitablePosition.Y;

		Node->SnapToGrid(SNodePanel::GetSnapGridSize());

		const TSharedPtr<FCopiedNode> CopiedNode = CopiedNodes.FindRef(Node->NodeGuid);
		if (ensure(CopiedNode))
		{
			CopiedNode->NewNode = Node;
		}

		PastedNodesBounds.Left = FMath::Min(Node->NodePosX, PastedNodesBounds.Left);
		PastedNodesBounds.Top = FMath::Min(Node->NodePosY, PastedNodesBounds.Top);

		PastedNodesBounds.Right = FMath::Max(Node->NodePosX, PastedNodesBounds.Right);
		PastedNodesBounds.Bottom = FMath::Max(Node->NodePosY, PastedNodesBounds.Bottom);

		if (UVoxelGraphParameterNodeBase* ParameterNode = Cast<UVoxelGraphParameterNodeBase>(Node))
		{
			if (const FGuid* NewParameterIdPtr = UpdatedParameters.Find(ParameterNode->Guid))
			{
				ParameterNode->Guid = *NewParameterIdPtr;
			}
			else
			{
				FGuid NewParameterId = FGuid::NewGuid();

				UpdatedParameters.Add(ParameterNode->Guid, NewParameterId);
				ParameterNode->Guid = NewParameterId;
				ParameterNode->CachedParameter.Guid = NewParameterId;
				Graph->Parameters.Add(ParameterNode->CachedParameter);
			}
		}

		// Give new node a different Guid from the old one
		Node->CreateNewGuid();
	}

	// Post paste for local variables
	for (UEdGraphNode* Node : PastedNodes)
	{
		GraphEditor->SetNodeSelection(Node, true);


		if (UVoxelGraphNodeBase* VoxelNode = Cast<UVoxelGraphNodeBase>(Node))
		{
			VoxelNode->PostPasteVoxelNode(PastedNodes);
		}
	}
}