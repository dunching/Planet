// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelCompiledGraph.h"
#include "VoxelNode.h"

DEFINE_VOXEL_MEMORY_STAT(STAT_VoxelCompiledGraph);

namespace Voxel::Graph
{

thread_local TArray<FPin* const*> GActivePinViews;

void PushPinView(FPin* const* Pins)
{
	GActivePinViews.Add(Pins);
}

void PopPinView(FPin* const* Pins)
{
	ensure(GActivePinViews.Pop(false) == Pins);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

const FVoxelPinValue& FPin::GetDefaultValue() const
{
	ensure(Direction == EPinDirection::Input);
	return DefaultValue;
}

void FPin::SetDefaultValue(const FVoxelPinValue& NewDefaultValue)
{
	ensure(Direction == EPinDirection::Input);

	if (!NewDefaultValue.IsValid())
	{
		ensure(!DefaultValue.IsValid());
		ensure(!Type.HasPinDefaultValue());
		return;
	}

	ensure(Type.HasPinDefaultValue());
	ensure(!NewDefaultValue.IsArray());
	ensure(NewDefaultValue.GetType().CanBeCastedTo(Type.GetPinDefaultValueType()));
	DefaultValue = NewDefaultValue;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FPin::MakeLinkTo(FPin& Other)
{
	if (Direction == EPinDirection::Input)
	{
		ensure(Other.Type.CanBeCastedTo_Schema(Type));
	}
	else
	{
		check(Direction == EPinDirection::Output);
		ensure(Type.CanBeCastedTo_Schema(Other.Type));
	}

	ensure(Direction != Other.Direction);
	ensure(!LinkedTo.Contains(&Other));
	LinkedTo.Add(&Other);
	Other.LinkedTo.Add(this);
}

bool FPin::IsLinkedTo(FPin& Other) const
{
	ensure(LinkedTo.Contains(&Other) == Other.LinkedTo.Contains(this));
	return LinkedTo.Contains(&Other);
}

void FPin::TryMakeLinkTo(FPin& Other)
{
	if (!IsLinkedTo(Other))
	{
		MakeLinkTo(Other);
	}
}

void FPin::BreakLinkTo(FPin& Other)
{
	ensure(LinkedTo.Remove(&Other));
	ensure(Other.LinkedTo.Remove(this));
}

void FPin::BreakAllLinks()
{
	for (FPin* Other : LinkedTo)
	{
		ensure(Other->LinkedTo.Remove(this));
	}
	LinkedTo.Reset();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FPin::CopyInputPinTo(FPin& Target) const
{
	ensure(Type.CanBeCastedTo_Schema(Target.Type));
	ensure(Direction == Target.Direction);
	ensure(Direction == EPinDirection::Input);

	for (FPin* Other : LinkedTo)
	{
		Other->MakeLinkTo(Target);
	}

	if (LinkedTo.Num() == 0)
	{
		Target.SetDefaultValue(GetDefaultValue());
	}
}

void FPin::CopyOutputPinTo(FPin& Target) const
{
	ensure(Type.CanBeCastedTo_Schema(Target.Type));
	ensure(Direction == Target.Direction);
	ensure(Direction == EPinDirection::Output);

	for (FPin* Other : LinkedTo)
	{
		Other->MakeLinkTo(Target);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FPin::Check(const FGraph& Graph) const
{
	check(IsInGameThread());

#define CHECK(...) if (!ensureVoxelSlow(__VA_ARGS__)) { VOXEL_MESSAGE(Error, "{0}: Internal error: {1}", this, #__VA_ARGS__); return; }

	CHECK(!Name.IsNone());
	CHECK(Type.IsValid());
	CHECK(Graph.Nodes.Contains(&Node));

	if (Direction == EPinDirection::Input)
	{
		CHECK(Node.FindInput(Name) == this);

		if (Type.HasPinDefaultValue())
		{
			if (GetLinkedTo().Num() == 0)
			{
				CHECK(DefaultValue.IsValid());
				CHECK(DefaultValue.GetType().HasPinDefaultValue());
				CHECK(DefaultValue.GetType().CanBeCastedTo(Type.GetPinDefaultValueType()));
			}
		}
		else
		{
			CHECK(!DefaultValue.IsValid());
		}
	}
	else
	{
		check(Direction == EPinDirection::Output);
		CHECK(Node.FindOutput(Name) == this);
		CHECK(!DefaultValue.IsValid());
	}

	{
		static TSet<FPin*> StaticLinkedToSet;
		StaticLinkedToSet.Reset();
		StaticLinkedToSet.Append(LinkedTo);
		CHECK(StaticLinkedToSet.Num() == LinkedTo.Num());
	}

	CHECK(
		Direction == EPinDirection::Output ||
		LinkedTo.Num() <= 1);

	CHECK(!LinkedTo.Contains(this));
	for (const FPin* Pin : LinkedTo)
	{
		CHECK(Direction != Pin->Direction);
		CHECK(Graph.Nodes.Contains(&Pin->Node));
		CHECK(Pin->LinkedTo.Contains(this));

		if (Direction == EPinDirection::Input)
		{
			CHECK(Pin->Type.CanBeCastedTo_Schema(Type));
			CHECK(Pin->Node.FindOutput(Pin->Name) == Pin);
		}
		else
		{
			check(Direction == EPinDirection::Output);

			CHECK(Type.CanBeCastedTo_Schema(Pin->Type));
			CHECK(Pin->Node.FindInput(Pin->Name) == Pin);
		}
	}

#undef CHECK
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FNode::SetVoxelNode(const TSharedRef<const FVoxelNode>& Node)
{
	ensure(!VoxelNode.IsValid());
	VoxelNode = Node;
}

void FNode::AddError(const FString& Error)
{
	Errors.Add(Error);
}

void FNode::FlushErrors() const
{
	for (const FString& Error : Errors)
	{
		VOXEL_MESSAGE(Error, "{0}: {1}", this, Error);
	}
}

void FNode::CopyFrom(const FNode& Src)
{
	ensure(Type == Src.Type);
	ensure(NodeRef == Src.NodeRef);
	Errors = Src.Errors;
	VoxelNode = Src.VoxelNode;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FPin* FNode::FindPin(const FName Name)
{
	return PinsMap.FindRef(Name);
}

FPin* FNode::FindInput(const FName Name)
{
	if (FPin* Pin = PinsMap.FindRef(Name))
	{
		ensure(Pin->Direction == EPinDirection::Input);
		return Pin;
	}
	return nullptr;
}

FPin* FNode::FindOutput(const FName Name)
{
	if (FPin* Pin = PinsMap.FindRef(Name))
	{
		ensure(Pin->Direction == EPinDirection::Output);
		return Pin;
	}
	return nullptr;
}

FPin& FNode::FindInputChecked(const FName Name)
{
	FPin& Pin = *PinsMap.FindChecked(Name);
	check(Pin.Direction == EPinDirection::Input);
	return Pin;
}

FPin& FNode::FindOutputChecked(const FName Name)
{
	FPin& Pin = *PinsMap.FindChecked(Name);
	check(Pin.Direction == EPinDirection::Output);
	return Pin;
}

FPin& FNode::NewInputPin(const FName Name, const FVoxelPinType& PinType, const FVoxelPinValue& DefaultValue)
{
	FPin& Pin = Graph.NewPin(Name, PinType, DefaultValue, EPinDirection::Input, *this);

	Pins.Add(&Pin);
	InputPins.Add(&Pin);

	PinsMap.Add_CheckNew(Name, &Pin);

	return Pin;
}

FPin& FNode::NewOutputPin(const FName Name, const FVoxelPinType& PinType)
{
	FPin& Pin = Graph.NewPin(Name, PinType, {}, EPinDirection::Output, *this);

	Pins.Add(&Pin);
	OutputPins.Add(&Pin);

	PinsMap.Add_CheckNew(Name, &Pin);

	return Pin;
}

void FNode::Check()
{
	if (UE_BUILD_SHIPPING)
	{
		return;
	}

#define CHECK(...) if (!ensureVoxelSlow(__VA_ARGS__)) { VOXEL_MESSAGE(Error, "{0}: Internal error: {1}", this, #__VA_ARGS__); return; }

	if (Type == ENodeType::Root)
	{
		CHECK(InputPins.Num() + OutputPins.Num() == 1);
	}
	else if (Type == ENodeType::Struct)
	{
		CHECK(VoxelNode.IsValid());

		for (const FVoxelPin& VoxelPin : VoxelNode->GetPins())
		{
			const FPin* Pin = VoxelPin.bIsInput ? FindInput(VoxelPin.Name) : FindOutput(VoxelPin.Name);
			CHECK(Pin);
			CHECK(Pin->Type == VoxelPin.GetType());
		}

		for (const FPin& Pin : GetPins())
		{
			if (!Pin.GetParentName().IsNone())
			{
				continue;
			}

			const TSharedPtr<const FVoxelPin> VoxelPin = VoxelNode->FindPin(Pin.Name);
			CHECK(VoxelPin);
			CHECK(VoxelPin->GetType() == Pin.Type);
		}
	}
	else
	{
		check(Type == ENodeType::Passthrough);
	}

	for (const FPin* Pin : Pins)
	{
		Pin->Check(Graph);
	}

#undef CHECK
}

void FNode::BreakAllLinks()
{
	for (FPin* Pin : Pins)
	{
		Pin->BreakAllLinks();
	}
}

void FNode::RemovePin(FPin& Pin)
{
	check(!GActivePinViews.Contains(Pins.GetData()));
	check(!GActivePinViews.Contains(InputPins.GetData()));
	check(!GActivePinViews.Contains(OutputPins.GetData()));

	Pin.BreakAllLinks();

	ensure(Pins.Remove(&Pin));

	ensure(PinsMap[Pin.Name] == &Pin);
	PinsMap[Pin.Name] = nullptr;

	if (Pin.Direction == EPinDirection::Input)
	{
		ensure(InputPins.Remove(&Pin));
	}
	else
	{
		ensure(OutputPins.Remove(&Pin));
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FGraph::FGraph()
{
	VOXEL_FUNCTION_COUNTER();
	Nodes.Reserve(1024);
}

int64 FGraph::GetAllocatedSize() const
{
	return
		Nodes.GetAllocatedSize() +
		PinAllocations.GetAllocatedSize() +
		NodeAllocations.GetAllocatedSize();
}

FNode& FGraph::NewNode(const ENodeType Type, const FVoxelGraphNodeRef& NodeRef)
{
	ON_SCOPE_EXIT
	{
		UpdateStats();
	};

	FNode& Node = NodeAllocations.Emplace_GetRef(Type, NodeRef, *this);
	Nodes.Add(&Node);
	return Node;
}

void FGraph::Check()
{
	UpdateStats();

	if (UE_BUILD_SHIPPING)
	{
		return;
	}

	VOXEL_FUNCTION_COUNTER();

	for (FNode* Node : Nodes)
	{
		Node->Check();
	}
}

TSharedRef<FGraph> FGraph::Clone() const
{
	VOXEL_FUNCTION_COUNTER();

	const TSharedRef<FGraph> Graph = MakeVoxelShared<FGraph>();

	TVoxelAddOnlyMap<const FNode*, FNode*> OldToNewNodes;
	OldToNewNodes.Reserve(Nodes.Num());

	TVoxelAddOnlyMap<const FPin*, FPin*> OldToNewPins;
	{
		VOXEL_SCOPE_COUNTER("Reserve pins");

		int32 NumPins = 0;
		for (const FNode& Node : GetNodes())
		{
			NumPins += Node.GetPins().Num();
		}
		OldToNewPins.Reserve(NumPins);
	}

	{
		VOXEL_SCOPE_COUNTER("Clone nodes");

		for (const FNode& OldNode : GetNodes())
		{
			FNode& NewNode = Graph->NewNode(OldNode.Type, OldNode.NodeRef);
			OldToNewNodes.Add_CheckNew(&OldNode, &NewNode);

			NewNode.CopyFrom(OldNode);

			for (const FPin& OldPin : OldNode.GetInputPins())
			{
				FPin& NewPin = NewNode.NewInputPin(OldPin.Name, OldPin.Type, OldPin.GetDefaultValue());
				OldToNewPins.Add_CheckNew(&OldPin, &NewPin);
			}
			for (const FPin& OldPin : OldNode.GetOutputPins())
			{
				FPin& NewPin = NewNode.NewOutputPin(OldPin.Name, OldPin.Type);
				OldToNewPins.Add_CheckNew(&OldPin, &NewPin);
			}
		}
	}

	{
		VOXEL_SCOPE_COUNTER("Make links");

		for (const FNode& OldNode : GetNodes())
		{
			for (const FPin& OldPin : OldNode.GetPins())
			{
				FPin& NewPin = *OldToNewPins[&OldPin];
				for (FPin& OtherPin : OldPin.GetLinkedTo())
				{
					NewPin.TryMakeLinkTo(*OldToNewPins[&OtherPin]);
				}
			}
		}
	}

	if (VOXEL_DEBUG)
	{
		Graph->Check();
	}

	return Graph;
}

bool FGraph::Identical(const FGraph& Other, FString* OutDiff) const
{
	VOXEL_FUNCTION_COUNTER();

#if VOXEL_DEBUG
	for (FNode* Node : Nodes)
	{
		ensure(!Node->GetVoxelNode().HasNodeRuntime());
	}
	for (FNode* Node : Other.Nodes)
	{
		ensure(!Node->GetVoxelNode().HasNodeRuntime());
	}
#endif

	using FNodeArray = TVoxelArray<const FNode*, TVoxelInlineAllocator<1>>;
	TVoxelAddOnlyMap<uint32, FNodeArray> HashToNodesA;
	TVoxelAddOnlyMap<uint32, FNodeArray> HashToNodesB;

	for (const FNode& Node : GetNodes())
	{
		if (!ensure(Node.Type == ENodeType::Struct))
		{
			return false;
		}

		HashToNodesA.FindOrAdd(Node.GetVoxelNode().GetNodeHash()).Add(&Node);
	}
	for (const FNode& Node : Other.GetNodes())
	{
		if (!ensure(Node.Type == ENodeType::Struct))
		{
			return false;
		}

		HashToNodesB.FindOrAdd(Node.GetVoxelNode().GetNodeHash()).Add(&Node);
	}

	if (!OutDiff &&
		HashToNodesA.Num() != HashToNodesB.Num())
	{
		return false;
	}

	TVoxelArray<const FNode*> NodesOnlyInA;
	TVoxelArray<const FNode*> NodesOnlyInB;
	TVoxelAddOnlyMap<const FNode*, const FNode*> NodeAToNodeB;

	for (const auto& It : HashToNodesA)
	{
		const FNodeArray* NodesBPtr = HashToNodesB.Find(It.Key);
		if (!NodesBPtr)
		{
			NodesOnlyInA.Append(It.Value);
			continue;
		}

		FNodeArray NodesA = It.Value;
		FNodeArray NodesB = *NodesBPtr;

		for (auto NodesAIt = NodesA.CreateIterator(); NodesAIt; ++NodesAIt)
		{
			for (auto NodesBIt = NodesB.CreateIterator(); NodesBIt; ++NodesBIt)
			{
				if ((**NodesAIt).GetVoxelNode().IsNodeIdentical((**NodesBIt).GetVoxelNode()))
				{
					NodeAToNodeB.Add_CheckNew(*NodesAIt, *NodesBIt);
					NodesAIt.RemoveCurrentSwap();
					NodesBIt.RemoveCurrentSwap();
					break;
				}
			}
		}

		NodesOnlyInA.Append(NodesA);
		NodesOnlyInB.Append(NodesB);
	}

	for (const auto& It : HashToNodesB)
	{
		if (!HashToNodesA.Contains(It.Key))
		{
			NodesOnlyInB.Append(It.Value);
		}
	}

	bool bChanged = false;

	const auto AddDiff = [&](const FNode& Node, const FString& Diff)
	{
		bChanged = true;

		if (!OutDiff)
		{
			return;
		}

		if (!OutDiff->IsEmpty())
		{
			*OutDiff += ", ";
		}
		*OutDiff +=
#if WITH_EDITOR
			Node.GetVoxelNode().GetDisplayName() +
#else
			Node.GetVoxelNode().GetStruct()->GetName() +
#endif
			": " + FString(Diff);
	};

	if (NodesOnlyInA.Num() != 0 ||
		NodesOnlyInB.Num() != 0)
	{
		if (OutDiff)
		{
			for (const FNode* Node : NodesOnlyInA)
			{
				AddDiff(*Node, "deleted");
			}

			for (const FNode* Node : NodesOnlyInB)
			{
				AddDiff(*Node, "added");
			}
		}
		return false;
	}
	check(NodeAToNodeB.Num() == GetNodes().Num());
	check(NodeAToNodeB.Num() == Other.GetNodes().Num());

	TMap<const FPin*, const FPin*> PinAToPinB;
	for (const auto& It : NodeAToNodeB)
	{
		if (bChanged && !OutDiff)
		{
			return false;
		}

		const FNode& NodeA = *It.Key;
		const FNode& NodeB = *It.Value;

		if (NodeA.GetInputPins().Num() < NodeB.GetInputPins().Num())
		{
			AddDiff(NodeA, "input pin added");
			continue;
		}
		if (NodeA.GetInputPins().Num() > NodeB.GetInputPins().Num())
		{
			AddDiff(NodeA, "input pin deleted");
			continue;
		}

		if (NodeA.GetOutputPins().Num() < NodeB.GetOutputPins().Num())
		{
			AddDiff(NodeA, "input pin added");
			continue;
		}
		if (NodeA.GetOutputPins().Num() > NodeB.GetOutputPins().Num())
		{
			AddDiff(NodeA, "input pin deleted");
			continue;
		}

		for (int32 PinIndex = 0; PinIndex < NodeA.GetInputPins().Num(); PinIndex++)
		{
			const FPin& PinA = NodeA.GetInputPin(PinIndex);
			const FPin& PinB = NodeB.GetInputPin(PinIndex);

			if (PinA.GetLinkedTo().Num() == 0 &&
				PinB.GetLinkedTo().Num() == 0 &&
				PinA.GetDefaultValue() !=
				PinB.GetDefaultValue())
			{
				AddDiff(NodeA, PinA.Name.ToString() + ": default value changed");
			}

			if (PinA.GetLinkedTo().Num() != PinB.GetLinkedTo().Num())
			{
				AddDiff(NodeA, PinA.Name.ToString() + ": num linked to changed");
			}

			ensure(!PinAToPinB.Contains(&PinA));
			PinAToPinB.Add(&PinA, &PinB);
		}

		for (int32 PinIndex = 0; PinIndex < NodeA.GetOutputPins().Num(); PinIndex++)
		{
			const FPin& PinA = NodeA.GetOutputPin(PinIndex);
			const FPin& PinB = NodeB.GetOutputPin(PinIndex);

			if (PinA.GetLinkedTo().Num() != PinB.GetLinkedTo().Num())
			{
				AddDiff(NodeA, PinA.Name.ToString() + ": num linked to changed");
			}

			ensure(!PinAToPinB.Contains(&PinA));
			PinAToPinB.Add(&PinA, &PinB);
		}
	}

	if (bChanged)
	{
		return false;
	}

	for (const auto& It : NodeAToNodeB)
	{
		if (bChanged && !OutDiff)
		{
			return false;
		}

		const FNode& NodeA = *It.Key;
		const FNode& NodeB = *It.Value;

		check(NodeA.GetInputPins().Num() == NodeB.GetInputPins().Num());
		for (int32 PinIndex = 0; PinIndex < NodeA.GetInputPins().Num(); PinIndex++)
		{
			const FPin& PinA = NodeA.GetInputPin(PinIndex);
			const FPin& PinB = NodeB.GetInputPin(PinIndex);

			check(PinA.GetLinkedTo().Num() == PinB.GetLinkedTo().Num());
			for (int32 LinkedToIndex = 0; LinkedToIndex < PinA.GetLinkedTo().Num(); LinkedToIndex++)
			{
				const FPin& PinALinkedTo = PinA.GetLinkedTo()[LinkedToIndex];
				const FPin& PinBLinkedTo = PinB.GetLinkedTo()[LinkedToIndex];

				if (PinAToPinB[&PinALinkedTo] != &PinBLinkedTo)
				{
					AddDiff(NodeA, PinA.Name.ToString() + ": link changed");
				}
			}
		}

		check(NodeA.GetOutputPins().Num() == NodeB.GetOutputPins().Num());
		for (int32 PinIndex = 0; PinIndex < NodeA.GetOutputPins().Num(); PinIndex++)
		{
			const FPin& PinA = NodeA.GetOutputPin(PinIndex);
			const FPin& PinB = NodeB.GetOutputPin(PinIndex);

			check(PinA.GetLinkedTo().Num() == PinB.GetLinkedTo().Num());
			for (int32 LinkedToIndex = 0; LinkedToIndex < PinA.GetLinkedTo().Num(); LinkedToIndex++)
			{
				const FPin& PinALinkedTo = PinA.GetLinkedTo()[LinkedToIndex];
				const FPin& PinBLinkedTo = PinB.GetLinkedTo()[LinkedToIndex];

				if (PinAToPinB[&PinALinkedTo] != &PinBLinkedTo)
				{
					AddDiff(NodeA, PinA.Name.ToString() + ": link changed");
				}
			}
		}
	}

	return !bChanged;
}

FORCEINLINE FPin& FGraph::NewPin(
	const FName Name,
	const FVoxelPinType& Type,
	const FVoxelPinValue& DefaultValue,
	const EPinDirection Direction,
	FNode& Node)
{
	ON_SCOPE_EXIT
	{
		UpdateStats();
	};

	return PinAllocations.Emplace_GetRef(
		Name,
		Type,
		DefaultValue,
		Direction,
		Node);
}

}