// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "SVoxelGraphMessages.h"
#include "VoxelGraph.h"
#include "VoxelRuntimeGraph.h"
#include "SVoxelNotification.h"

void SVoxelGraphMessages::Construct(const FArguments& Args)
{
	ensure(Args._Graph);
	WeakGraph = Args._Graph;

	ChildSlot
	[
		SAssignNew(Tree, STree)
		.TreeItemsSource(&Nodes)
		.OnGenerateRow_Lambda([&](const TSharedPtr<INode>& Node, const TSharedRef<STableViewBase>& OwnerTable)
		{
			return
				SNew(STableRow<TSharedPtr<INode>>, OwnerTable)
				[
					Node->GetWidget()
				];
		})
		.OnGetChildren_Lambda([&](const TSharedPtr<INode>& Node, TArray<TSharedPtr<INode>>& OutChildren)
		{
			OutChildren = Node->GetChildren();
		})
		.SelectionMode(ESelectionMode::None)
	];

	Tree->SetItemExpansion(CompileNode, true);
	Tree->SetItemExpansion(RuntimeNode, true);

	UpdateNodes();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void SVoxelGraphMessages::UpdateNodes()
{
	UVoxelGraph* RootGraph = WeakGraph.Get();
	if (!ensure(RootGraph))
	{
		return;
	}

	MessageToCanonMessage.Reset();
	StringToCanonMessage.Reset();

	TSet<TSharedPtr<INode>> VisitedNodes;
	TSet<TSharedPtr<INode>> NodesToExpand;

	for (UVoxelGraph* Graph : RootGraph->GetAllGraphs())
	{
		const UVoxelRuntimeGraph& RuntimeGraph = Graph->GetRuntimeGraph();

		TSharedPtr<FGraphNode>& CompileGraphNode = CompileNode->GraphToNode.FindOrAdd(Graph);
		if (!CompileGraphNode)
		{
			CompileGraphNode = MakeVoxelShared<FGraphNode>(Graph);
			NodesToExpand.Add(CompileGraphNode);
		}

		TSharedPtr<FGraphNode>& RuntimeGraphNode = RuntimeNode->GraphToNode.FindOrAdd(Graph);
		if (!RuntimeGraphNode)
		{
			RuntimeGraphNode = MakeVoxelShared<FGraphNode>(Graph);
			NodesToExpand.Add(RuntimeGraphNode);
		}

		for (const TSharedRef<FTokenizedMessage>& Message : RuntimeGraph.CompileMessages)
		{
			VisitedNodes.Add(CompileGraphNode);

			TSharedPtr<FMessageNode>& MessageNode = CompileGraphNode->MessageToNode.FindOrAdd(Message);
			if (!MessageNode)
			{
				MessageNode = MakeVoxelShared<FMessageNode>(Message);
				NodesToExpand.Add(MessageNode);
			}
			VisitedNodes.Add(MessageNode);
		}

		for (const TSharedRef<FTokenizedMessage>& Message : RuntimeGraph.RuntimeMessages)
		{
			VisitedNodes.Add(RuntimeGraphNode);

			const TSharedRef<FTokenizedMessage> CanonMessage = GetCanonMessage(Message);

			TSharedPtr<FMessageNode>& MessageNode = RuntimeGraphNode->MessageToNode.FindOrAdd(CanonMessage);
			if (!MessageNode)
			{
				MessageNode = MakeVoxelShared<FMessageNode>(CanonMessage);
				NodesToExpand.Add(MessageNode);
			}
			VisitedNodes.Add(MessageNode);
		}

		for (const auto& It : RuntimeGraph.PinRefToCompileMessages)
		{
			if (It.Value.Num() == 0)
			{
				continue;
			}

			VisitedNodes.Add(CompileGraphNode);

			TSharedPtr<FPinNode>& PinNode = CompileGraphNode->PinRefToNode.FindOrAdd(It.Key);
			if (!PinNode)
			{
				PinNode = MakeVoxelShared<FPinNode>(It.Key);
				NodesToExpand.Add(PinNode);
			}
			VisitedNodes.Add(PinNode);

			for (const TSharedRef<FTokenizedMessage>& Message : It.Value)
			{
				TSharedPtr<FMessageNode>& MessageNode = PinNode->MessageToNode.FindOrAdd(Message);
				if (!MessageNode)
				{
					MessageNode = MakeVoxelShared<FMessageNode>(Message);
					NodesToExpand.Add(MessageNode);
				}
				VisitedNodes.Add(MessageNode);
			}
		}
	}

	const auto Cleanup = [&](FRootNode& RootNode)
	{
		for (auto GraphNodeIt = RootNode.GraphToNode.CreateIterator(); GraphNodeIt; ++GraphNodeIt)
		{
			if (!VisitedNodes.Contains(GraphNodeIt.Value()))
			{
				GraphNodeIt.RemoveCurrent();
				continue;
			}

			for (auto MessageIt = GraphNodeIt.Value()->MessageToNode.CreateIterator(); MessageIt; ++MessageIt)
			{
				if (!VisitedNodes.Contains(MessageIt.Value()))
				{
					MessageIt.RemoveCurrent();
				}
			}

			for (auto PinIt = GraphNodeIt.Value()->PinRefToNode.CreateIterator(); PinIt; ++PinIt)
			{
				if (!VisitedNodes.Contains(PinIt.Value()))
				{
					PinIt.RemoveCurrent();
					continue;
				}

				for (auto MessageIt = PinIt.Value()->MessageToNode.CreateIterator(); MessageIt; ++MessageIt)
				{
					if (!VisitedNodes.Contains(MessageIt.Value()))
					{
						MessageIt.RemoveCurrent();
					}
				}
			}
		}
	};

	Cleanup(*CompileNode);
	Cleanup(*RuntimeNode);

	// Always expand new items
	if (!Nodes.Contains(CompileNode))
	{
		NodesToExpand.Add(CompileNode);
	}
	if (!Nodes.Contains(RuntimeNode))
	{
		NodesToExpand.Add(RuntimeNode);
	}

	Nodes.Reset();

	if (CompileNode->GraphToNode.Num() > 0)
	{
		Nodes.Add(CompileNode);
	}
	if (RuntimeNode->GraphToNode.Num() > 0)
	{
		Nodes.Add(RuntimeNode);
	}

	for (const TSharedPtr<INode>& Node : NodesToExpand)
	{
		Tree->SetItemExpansion(Node, true);
	}

	Tree->RequestTreeRefresh();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedRef<SWidget> SVoxelGraphMessages::FRootNode::GetWidget() const
{
	return SNew(STextBlock).Text(FText::FromString(Text));
}

TArray<TSharedPtr<SVoxelGraphMessages::INode>> SVoxelGraphMessages::FRootNode::GetChildren() const
{
	TArray<TSharedPtr<FGraphNode>> Children;
	GraphToNode.GenerateValueArray(Children);
	return TArray<TSharedPtr<INode>>(Children);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedRef<SWidget> SVoxelGraphMessages::FGraphNode::GetWidget() const
{
	return SNew(STextBlock).Text_Lambda([Graph = Graph]
	{
		if (Graph.IsValid())
		{
			return FText::FromString(Graph->GetGraphName());
		}
		else
		{
			return INVTEXT("Invalid");
		}
	});
}

TArray<TSharedPtr<SVoxelGraphMessages::INode>> SVoxelGraphMessages::FGraphNode::GetChildren() const
{
	TArray<TSharedPtr<INode>> Children;
	for (const auto& It : MessageToNode)
	{
		Children.Add(It.Value);
	}
	for (const auto& It : PinRefToNode)
	{
		Children.Add(It.Value);
	}
	return Children;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedRef<SWidget> SVoxelGraphMessages::FPinNode::GetWidget() const
{
	return SNew(STextBlock).Text(FText::FromString(PinRef.Node.EdGraphNodeTitle.ToString() + "." + PinRef.PinName.ToString()));
}

TArray<TSharedPtr<SVoxelGraphMessages::INode>> SVoxelGraphMessages::FPinNode::GetChildren() const
{
	TArray<TSharedPtr<INode>> Children;
	for (const auto& It : MessageToNode)
	{
		Children.Add(It.Value);
	}
	return Children;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedRef<SWidget> SVoxelGraphMessages::FMessageNode::GetWidget() const
{
	return SNew(SVoxelNotification, Message);
}

TArray<TSharedPtr<SVoxelGraphMessages::INode>> SVoxelGraphMessages::FMessageNode::GetChildren() const
{
	return {};
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedRef<FTokenizedMessage> SVoxelGraphMessages::GetCanonMessage(const TSharedRef<FTokenizedMessage>& Message)
{
	if (const TSharedPtr<FTokenizedMessage> CanonMessage = MessageToCanonMessage.FindRef(Message).Pin())
	{
		return CanonMessage.ToSharedRef();
	}

	TWeakPtr<FTokenizedMessage>& WeakCanonMessage = StringToCanonMessage.FindOrAdd(Message->ToText().ToString());

	if (const TSharedPtr<FTokenizedMessage> CanonMessage = WeakCanonMessage.Pin())
	{
		MessageToCanonMessage.Add(Message, CanonMessage);
		return CanonMessage.ToSharedRef();
	}

	WeakCanonMessage = Message;
	MessageToCanonMessage.Add(Message, Message);
	return Message;
}