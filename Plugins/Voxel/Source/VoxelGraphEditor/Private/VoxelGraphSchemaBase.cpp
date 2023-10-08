// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelGraphSchemaBase.h"
#include "VoxelEdGraph.h"
#include "VoxelGraphToolkit.h"
#include "VoxelGraphNodeBase.h"
#include "VoxelGraphSchemaAction.h"

#include "ToolMenu.h"
#include "ToolMenuSection.h"

void UVoxelGraphSchemaBase::OnGraphChanged(const UEdGraphPin* Pin)
{
	if (!ensure(Pin) ||
		!Pin->GetOwningNodeUnchecked())
	{
		return;
	}

	OnGraphChanged(Pin->GetOwningNode());
}

void UVoxelGraphSchemaBase::OnGraphChanged(const UEdGraphNode* Node)
{
	if (!ensure(Node))
	{
		return;
	}

	OnGraphChanged(Node->GetGraph());
}

void UVoxelGraphSchemaBase::OnGraphChanged(const UEdGraph* EdGraph)
{
	if (!ensure(EdGraph))
	{
		return;
	}

	const TSharedPtr<FVoxelGraphToolkit> Toolkit = GetToolkit(EdGraph);
	if (!Toolkit)
	{
		return;
	}

	ensure(!EdGraph->HasAnyFlags(RF_Transient));
	Toolkit->OnGraphChanged(EdGraph);
}

TSharedPtr<FVoxelGraphToolkit> UVoxelGraphSchemaBase::GetToolkit(const UEdGraph* Graph)
{
	if (!Graph)
	{
		return nullptr;
	}

	return CastChecked<UVoxelEdGraph>(Graph)->GetGraphToolkit();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool UVoxelGraphSchemaBase::ConnectionCausesLoop(const UEdGraphPin* InputPin, const UEdGraphPin* OutputPin) const
{
	const UEdGraphNode* StartNode = OutputPin->GetOwningNode();

	TSet<UEdGraphNode*> ProcessedNodes;

	TArray<UEdGraphNode*> NodesToProcess;
	NodesToProcess.Add(InputPin->GetOwningNode());

	while (NodesToProcess.Num() > 0)
	{
		UEdGraphNode* Node = NodesToProcess.Pop(false);
		if (ProcessedNodes.Contains(Node))
		{
			continue;
		}
		ProcessedNodes.Add(Node);

		for (const UEdGraphPin* Pin : Node->GetAllPins())
		{
			if (Pin->Direction != EGPD_Output)
			{
				continue;
			}

			for (const UEdGraphPin* LinkedToPin : Pin->LinkedTo)
			{
				check(LinkedToPin->Direction == EGPD_Input);

				UEdGraphNode* NewNode = LinkedToPin->GetOwningNode();
				ensure(NewNode);

				if (StartNode == NewNode)
				{
					return true;
				}
				NodesToProcess.Add(NewNode);
			}
		}
	}

	return false;
}

bool UVoxelGraphSchemaBase::CanCreateAutomaticConversionNode(const UEdGraphPin* PinA, const UEdGraphPin* PinB) const
{
	const UEdGraphPin* InputPin = nullptr;
	const UEdGraphPin* OutputPin = nullptr;
	if (!CategorizePinsByDirection(PinA, PinB, InputPin, OutputPin))
	{
		return false;
	}

	return FindCastAction(OutputPin->PinType, InputPin->PinType).IsValid();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void UVoxelGraphSchemaBase::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	VOXEL_FUNCTION_COUNTER();

	const TSharedPtr<FVoxelGraphToolkit> Toolkit = GetToolkit(ContextMenuBuilder.CurrentGraph);
	if (!ensure(Toolkit))
	{
		return;
	}
	const TSharedPtr<SGraphEditor> GraphEditor = Toolkit->GetActiveGraphEditor();
	if (!ensure(GraphEditor))
	{
		return;
	}

	// Comment
	if (!ContextMenuBuilder.FromPin)
	{
		bool bIsManyNodesSelected = false;
		if (ContextMenuBuilder.CurrentGraph)
		{
			bIsManyNodesSelected = GraphEditor->GetNumberOfSelectedNodes() > 0;
		}

		ContextMenuBuilder.AddAction(MakeVoxelShared<FVoxelGraphSchemaAction_NewComment>(
			FText(),
			bIsManyNodesSelected ? INVTEXT("Create Comment from Selection") : INVTEXT("Add Comment"),
			INVTEXT("Creates a comment"),
			0));
	}

	if (!ContextMenuBuilder.FromPin && Toolkit->CanPasteNodes())
	{
		ContextMenuBuilder.AddAction(MakeVoxelShared<FVoxelGraphSchemaAction_Paste>(FText(), INVTEXT("Paste here"), FText(), 0));
	}
}

bool UVoxelGraphSchemaBase::CreateAutomaticConversionNodeAndConnections(UEdGraphPin* PinA, UEdGraphPin* PinB) const
{
	UEdGraphPin* InputPin = nullptr;
	UEdGraphPin* OutputPin = nullptr;
	if (!CategorizePinsByDirection(PinA, PinB, InputPin, OutputPin))
	{
		return false;
	}

	const TSharedPtr<FEdGraphSchemaAction> CastAction = FindCastAction(OutputPin->PinType, InputPin->PinType);
	if (!CastAction)
	{
		return false;
	}

	const FVoxelTransaction Transaction(InputPin, "Create new graph node");

	FVector2D Position;
	Position.X = (InputPin->GetOwningNode()->NodePosX + OutputPin->GetOwningNode()->NodePosX) / 2.;
	Position.Y = (InputPin->GetOwningNode()->NodePosY + OutputPin->GetOwningNode()->NodePosY) / 2.;

	const UEdGraphNode* GraphNode = CastAction->PerformAction(PinA->GetOwningNode()->GetGraph(), nullptr, Position);
	const UVoxelGraphNodeBase* Node = CastChecked<UVoxelGraphNodeBase>(GraphNode);

	TryCreateConnection(OutputPin, Node->GetInputPin(0));
	TryCreateConnection(InputPin, Node->GetOutputPin(0));

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

const FPinConnectionResponse UVoxelGraphSchemaBase::CanCreateConnection(const UEdGraphPin* PinA, const UEdGraphPin* PinB) const
{
	// Make sure the pins are not on the same node
	if (PinA->GetOwningNode() == PinB->GetOwningNode())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, "Both are on the same node");
	}

	if (PinA->bOrphanedPin ||
		PinB->bOrphanedPin)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, "Cannot make new connections to orphaned pin");
	}

	if (PinA->bNotConnectable ||
		PinB->bNotConnectable)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, "Pins are not connectable");
	}

	// Compare the directions
	const UEdGraphPin* InputPin = nullptr;
	const UEdGraphPin* OutputPin = nullptr;

	if (!CategorizePinsByDirection(PinA, PinB, InputPin, OutputPin))
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, "Directions are not compatible");
	}

	check(InputPin);
	check(OutputPin);

	if (ConnectionCausesLoop(InputPin, OutputPin))
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, "Connection would cause loop");
	}

	const TOptional<FPinConnectionResponse> Override = GetCanCreateConnectionOverride(PinA, PinB);
	if (Override.IsSet())
	{
		return Override.GetValue();
	}

	if (InputPin->LinkedTo.Num() > 0)
	{
		ECanCreateConnectionResponse Response;
		if (InputPin == PinA)
		{
			Response = CONNECT_RESPONSE_BREAK_OTHERS_A;
		}
		else
		{
			Response = CONNECT_RESPONSE_BREAK_OTHERS_B;
		}
		return FPinConnectionResponse(Response, "Replace existing connections");
	}

	return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, "");
}

bool UVoxelGraphSchemaBase::TryCreateConnection(UEdGraphPin* PinA, UEdGraphPin* PinB) const
{
	switch (CanCreateConnection(PinA, PinB).Response)
	{
	default:
	{
		ensure(false);
		return false;
	}
	case CONNECT_RESPONSE_MAKE:
	{
		PinA->Modify();
		PinB->Modify();
		PinA->MakeLinkTo(PinB);
	}
	break;
	case CONNECT_RESPONSE_BREAK_OTHERS_A:
	{
		PinA->Modify();
		PinB->Modify();
		PinA->BreakAllPinLinks(true);
		PinA->MakeLinkTo(PinB);
	}
	break;
	case CONNECT_RESPONSE_BREAK_OTHERS_B:
	{
		PinA->Modify();
		PinB->Modify();
		PinB->BreakAllPinLinks(true);
		PinA->MakeLinkTo(PinB);
	}
	break;
	case CONNECT_RESPONSE_BREAK_OTHERS_AB:
	{
		PinA->Modify();
		PinB->Modify();
		PinA->BreakAllPinLinks(true);
		PinB->BreakAllPinLinks(true);
		PinA->MakeLinkTo(PinB);
	}
	break;
	case CONNECT_RESPONSE_MAKE_WITH_CONVERSION_NODE:
	{
		if (!CreateAutomaticConversionNodeAndConnections(PinA, PinB))
		{
			return false;
		}
	}
	break;
	case CONNECT_RESPONSE_MAKE_WITH_PROMOTION:
	{
		if (!CreatePromotedConnectionSafe(PinA, PinB))
		{
			return false;
		}
	}
	break;
	case CONNECT_RESPONSE_DISALLOW:
	{
		return false;
	}
	}

	PinA->GetOwningNode()->PinConnectionListChanged(PinA);
	PinB->GetOwningNode()->PinConnectionListChanged(PinB);

	OnGraphChanged(PinA);

	return true;
}

bool UVoxelGraphSchemaBase::CreatePromotedConnection(UEdGraphPin* PinA, UEdGraphPin* PinB) const
{
	// CreatePromotedConnectionSafe should be used
	ensure(false);
	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedPtr<FEdGraphSchemaAction> UVoxelGraphSchemaBase::GetCreateCommentAction() const
{
	return MakeVoxelShared<FVoxelGraphSchemaAction_NewComment>();
}

int32 UVoxelGraphSchemaBase::GetNodeSelectionCount(const UEdGraph* Graph) const
{
	const TSharedPtr<SGraphEditor> GraphEditor = GetToolkit(Graph)->GetActiveGraphEditor();
	if (!ensure(GraphEditor))
	{
		return 0;
	}

	return GraphEditor->GetNumberOfSelectedNodes();
}

void UVoxelGraphSchemaBase::GetContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	if (const UEdGraphPin* Pin = Context->Pin)
	{
		FToolMenuSection& Section = Menu->FindOrAddSection("EdGraphSchemaPinActions");

		if (Pin->bOrphanedPin)
		{
			if (Pin->LinkedTo.Num() > 0)
			{
				Section.AddMenuEntry(FGraphEditorCommands::Get().BreakPinLinks);
			}
			else
			{
				Section.AddMenuEntry(
					"RemovePin",
					INVTEXT("Remove pin"),
					INVTEXT("Remove this orphaned pin"),
					FSlateIcon(),
					FUIAction(
						MakeLambdaDelegate([=]
						{
							ResetPinToAutogeneratedDefaultValue(ConstCast(Pin), true);
						})
					)
				);
			}

			return;
		}

		if (Pin->LinkedTo.Num() > 0)
		{
			Section.AddMenuEntry(FGraphEditorCommands::Get().BreakPinLinks);
		}

		if (UVoxelGraphNodeBase* Node = Cast<UVoxelGraphNodeBase>(Pin->GetOwningNode()))
		{
			if (Node->CanRemovePin_ContextMenu(*Pin))
			{
				Section.AddMenuEntry(
					"RemovePin",
					INVTEXT("Remove pin"),
					INVTEXT("Remove this pin"),
					FSlateIcon(),
					FUIAction(
						MakeLambdaDelegate([=]
						{
							Node->RemovePin_ContextMenu(ConstCast(*Pin));
						})
					)
				);
			}

			if (Node->CanSplitPin(*Pin))
			{
				Section.AddMenuEntry(FGraphEditorCommands::Get().SplitStructPin);
			}

			if (Node->CanRecombinePin(*Pin))
			{
				Section.AddMenuEntry(FGraphEditorCommands::Get().RecombineStructPin);
			}
		}

		if (Pin->Direction == EGPD_Input && Pin->LinkedTo.Num() == 0)
		{
			Section.AddMenuEntry(FGraphEditorCommands::Get().ResetPinToDefaultValue);
		}
	}

	if (const UEdGraphNode* Node = Context->Node)
	{
		{
			FToolMenuSection& Section = Menu->AddSection("VoxelGraphNodeEdit", INVTEXT("Node Actions"));
			Section.AddMenuEntry(FGenericCommands::Get().Delete);
			Section.AddMenuEntry(FGenericCommands::Get().Cut);
			Section.AddMenuEntry(FGenericCommands::Get().Copy);
			Section.AddMenuEntry(FGenericCommands::Get().Duplicate);
		}

		{
			FToolMenuSection& Section = Menu->AddSection("EdGraphSchemaOrganization", INVTEXT("Organization"));
			Section.AddSubMenu("Alignment", INVTEXT("Alignment"), FText(), FNewMenuDelegate::CreateLambda([](FMenuBuilder& InMenuBuilder)
			{
				InMenuBuilder.BeginSection("EdGraphSchemaAlignment", INVTEXT("Align"));
				InMenuBuilder.AddMenuEntry(FGraphEditorCommands::Get().AlignNodesTop);
				InMenuBuilder.AddMenuEntry(FGraphEditorCommands::Get().AlignNodesMiddle);
				InMenuBuilder.AddMenuEntry(FGraphEditorCommands::Get().AlignNodesBottom);
				InMenuBuilder.AddMenuEntry(FGraphEditorCommands::Get().AlignNodesLeft);
				InMenuBuilder.AddMenuEntry(FGraphEditorCommands::Get().AlignNodesCenter);
				InMenuBuilder.AddMenuEntry(FGraphEditorCommands::Get().AlignNodesRight);
				InMenuBuilder.AddMenuEntry(FGraphEditorCommands::Get().StraightenConnections);
				InMenuBuilder.EndSection();

				InMenuBuilder.BeginSection("EdGraphSchemaDistribution", INVTEXT("Distribution"));
				InMenuBuilder.AddMenuEntry(FGraphEditorCommands::Get().DistributeNodesHorizontally);
				InMenuBuilder.AddMenuEntry(FGraphEditorCommands::Get().DistributeNodesVertically);
				InMenuBuilder.EndSection();
			}));
		}

		if (const UVoxelGraphNodeBase* VoxelNode = Cast<UVoxelGraphNodeBase>(Node))
		{
			bool bHasCategories = false;
			if (VoxelNode->InputPinCategories.Num() > 1 ||
				VoxelNode->OutputPinCategories.Num() > 1)
			{
				bHasCategories = true;
			}
			else
			{
				if (VoxelNode->InputPinCategories.Num() == 1 &&
					!VoxelNode->InputPinCategories.Array()[0].IsNone())
				{
					bHasCategories = true;
				}
				else if (
					VoxelNode->OutputPinCategories.Num() == 1 &&
					!VoxelNode->OutputPinCategories.Array()[0].IsNone())
				{
					bHasCategories = true;
				}
			}

			if (bHasCategories)
			{
				FToolMenuSection& Section = Menu->AddSection("VoxelGraphNodeCategories", INVTEXT("Categories Actions"));

				Section.AddMenuEntry(
					"CollapseCategories",
					INVTEXT("Collapse categories"),
					INVTEXT("Collapse all pin categories"),
					FSlateIcon(),
					FUIAction(MakeLambdaDelegate([TargetNode = ConstCast(VoxelNode)]
					{
						TargetNode->CollapsedInputCategories = TargetNode->InputPinCategories;
						TargetNode->CollapsedOutputCategories = TargetNode->OutputPinCategories;
					}),
					MakeLambdaDelegate([=]
					{
						return
							VoxelNode->InputPinCategories.Num() != VoxelNode->CollapsedInputCategories.Num() ||
							VoxelNode->OutputPinCategories.Num() != VoxelNode->CollapsedOutputCategories.Num();
					}))
				);

				Section.AddMenuEntry(
					"ExpandCategories",
					INVTEXT("Expand categories"),
					INVTEXT("Expand all pin categories"),
					FSlateIcon(),
					FUIAction(MakeLambdaDelegate([TargetNode = ConstCast(VoxelNode)]
					{
						TargetNode->CollapsedInputCategories = {};
						TargetNode->CollapsedOutputCategories = {};
					}),
					MakeLambdaDelegate([=]
					{
						return
							VoxelNode->CollapsedInputCategories.Num() > 0 ||
							VoxelNode->CollapsedOutputCategories.Num() > 0;
					}))
				);
			}
		}

		if (VOXEL_DEBUG)
		{
			FToolMenuSection& Section = Menu->AddSection("VoxelGraphNodeAdvanced", INVTEXT("Advanced"));
			Section.AddMenuEntry(
				"ReconstructNode",
				INVTEXT("Reconstruct node"),
				INVTEXT("Reconstruct this node"),
				FSlateIcon(),
				FUIAction(
					MakeLambdaDelegate([=]
					{
						const FVoxelTransaction Transaction(ConstCast(Node), "Reconstruct node");
						ConstCast(Node)->ReconstructNode();
					})
				)
			);
		}
	}
}

void UVoxelGraphSchemaBase::ResetPinToAutogeneratedDefaultValue(UEdGraphPin* Pin, const bool bCallModifyCallbacks) const
{
	const FVoxelPinType Type(Pin->PinType);

	const FVoxelTransaction Transaction(Pin, "Reset pin to default");

	if (!Type.HasPinDefaultValue())
	{
		Pin->ResetDefaultValue();
		ensure(Pin->AutogeneratedDefaultValue.IsEmpty());
	}
	else if (Type.GetPinDefaultValueType().IsObject())
	{
		Pin->DefaultValue = {};

		if (Pin->AutogeneratedDefaultValue.IsEmpty())
		{
			Pin->DefaultObject = nullptr;
		}
		else
		{
			Pin->DefaultObject = LoadObject<UObject>(nullptr, *Pin->AutogeneratedDefaultValue);
			ensure(Pin->DefaultObject);
		}
	}
	else
	{
		Pin->DefaultValue = Pin->AutogeneratedDefaultValue;
		Pin->DefaultObject = nullptr;
	}

	if (bCallModifyCallbacks)
	{
		Pin->GetOwningNode()->PinDefaultValueChanged(Pin);
	}
}

void UVoxelGraphSchemaBase::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void UVoxelGraphSchemaBase::BreakNodeLinks(UEdGraphNode& TargetNode) const
{
	const FVoxelTransaction Transaction(&TargetNode, "Break node Links");
	Super::BreakNodeLinks(TargetNode);
}

void UVoxelGraphSchemaBase::BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotification) const
{
	const FVoxelTransaction Transaction(&TargetPin, "Break Pin Links");
	Super::BreakPinLinks(TargetPin, bSendsNodeNotification);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void UVoxelGraphSchemaBase::TrySetDefaultValue(UEdGraphPin& Pin, const FString& NewDefaultValue, bool bMarkAsModified) const
{
	ensure(!Pin.DefaultObject);
	Super::TrySetDefaultValue(Pin, NewDefaultValue, bMarkAsModified);
}

void UVoxelGraphSchemaBase::TrySetDefaultObject(UEdGraphPin& Pin, UObject* NewDefaultObject, bool bMarkAsModified) const
{
	Pin.DefaultValue = {};
	Super::TrySetDefaultObject(Pin, NewDefaultObject, bMarkAsModified);
}

bool UVoxelGraphSchemaBase::ShouldHidePinDefaultValue(UEdGraphPin* Pin) const
{
	if (!Pin)
	{
		return Super::ShouldHidePinDefaultValue(Pin);
	}

	const UVoxelGraphNodeBase* VoxelNode = Cast<UVoxelGraphNodeBase>(Pin->GetOwningNode());
	if (!VoxelNode)
	{
		return false;
	}

	return
		// Hide optional pins default values
		VoxelNode->IsPinOptional(*Pin) ||
		VoxelNode->ShouldHidePinDefaultValue(*Pin);
}