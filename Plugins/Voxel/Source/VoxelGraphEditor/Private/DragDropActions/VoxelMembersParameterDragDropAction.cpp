// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "DragDropActions/VoxelMembersParameterDragDropAction.h"
#include "VoxelGraph.h"
#include "VoxelEdGraph.h"
#include "VoxelGraphSchema.h"
#include "VoxelGraphVisuals.h"
#include "Nodes/VoxelGraphParameterNodeBase.h"

void FVoxelMembersParameterDragDropAction::HoverTargetChanged()
{
	const UVoxelGraph* Graph = WeakGraph.Get();
	if (!ensure(Graph))
	{
		return;
	}

	if (GetHoveredGraph() &&
		GetHoveredGraph() != Graph->MainEdGraph)
	{
		SetFeedbackMessageError("Cannot use in a different graph");
		return;
	}

	const FVoxelGraphParameter* Parameter = Graph->FindParameterByGuid(ParameterGuid);
	if (!ensure(Parameter))
	{
		return;
	}

	if (const UEdGraphPin* Pin = GetHoveredPin())
	{
		if (Pin->bOrphanedPin)
		{
			SetFeedbackMessageError("Cannot make connection to orphaned pin " + Pin->PinName.ToString() + "");
			return;
		}

		if (Pin->PinType != Parameter->Type ||
			Pin->bNotConnectable)
		{
			SetFeedbackMessageError("The type of '" + Parameter->Name.ToString() + "' is not compatible with " + Pin->PinName.ToString());
			return;
		}

		switch (Parameter->ParameterType)
		{
		default: ensure(false);
		case EVoxelGraphParameterType::Parameter:
		case EVoxelGraphParameterType::Input:
		{
			if (Pin->Direction == EGPD_Output)
			{
				SetFeedbackMessageError("Cannot connect to output pin");
				return;
			}

			SetFeedbackMessageOK("Make " + Pin->PinName.ToString() + " = " + Parameter->Name.ToString() + "");
			return;
		}
		case EVoxelGraphParameterType::Output:
		{
			if (Pin->Direction == EGPD_Input)
			{
				SetFeedbackMessageError("Cannot connect to input pin");
				return;
			}

			SetFeedbackMessageOK("Make " + Parameter->Name.ToString() + " = " + Pin->PinName.ToString());
			return;
		}
		case EVoxelGraphParameterType::LocalVariable:
		{
			if (Pin->Direction == EGPD_Input)
			{
				SetFeedbackMessageOK("Make " + Pin->PinName.ToString() + " = " + Parameter->Name.ToString() + "");
				return;
			}
			else
			{
				SetFeedbackMessageOK("Make " + Parameter->Name.ToString() + " = " + Pin->PinName.ToString() + "");
				return;
			}
		}
		}
	}

	if (const UVoxelGraphParameterNodeBase* ParameterNode = Cast<UVoxelGraphParameterNodeBase>(GetHoveredNode()))
	{
		if (ParameterNode->GetParameterType() == Parameter->ParameterType)
		{
			SetFeedbackMessageOK("Change node parameter");
			return;
		}
	}

	FVoxelMembersBaseDragDropAction::HoverTargetChanged();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FReply FVoxelMembersParameterDragDropAction::DroppedOnPin(FVector2D ScreenPosition, FVector2D GraphPosition)
{
	UVoxelGraph* Graph = WeakGraph.Get();
	if (!ensure(Graph))
	{
		return FReply::Unhandled();
	}

	if (Graph->MainEdGraph != GetHoveredGraph())
	{
		return FReply::Unhandled();
	}

	const FVoxelGraphParameter* Parameter = Graph->FindParameterByGuid(ParameterGuid);
	if (!ensure(Parameter))
	{
		return FReply::Unhandled();
	}

	UEdGraphPin* Pin = GetHoveredPin();
	if (!ensure(Pin))
	{
		return FReply::Unhandled();
	}

	if (Pin->bOrphanedPin ||
		Pin->PinType != Parameter->Type ||
		Pin->bNotConnectable)
	{
		return FReply::Unhandled();
	}

	FVoxelGraphSchemaAction_NewParameterUsage Action;
	Action.Guid = ParameterGuid;
	Action.ParameterType = Parameter->ParameterType;
	Action.bLocalVariable_IsDeclaration = Pin->Direction == EGPD_Output;
	Action.PerformAction(Pin->GetOwningNode()->GetGraph(), Pin, GraphPosition, true);

	return FReply::Handled();
}

FReply FVoxelMembersParameterDragDropAction::DroppedOnNode(FVector2D ScreenPosition, FVector2D GraphPosition)
{
	UVoxelGraph* Graph = WeakGraph.Get();
	if (!ensure(Graph))
	{
		return FReply::Unhandled();
	}

	if (Graph->MainEdGraph != GetHoveredGraph())
	{
		return FReply::Unhandled();
	}

	const FVoxelGraphParameter* Parameter = Graph->FindParameterByGuid(ParameterGuid);
	if (!ensure(Parameter))
	{
		return FReply::Unhandled();
	}

	UEdGraphNode* Node = GetHoveredNode();
	if (!ensure(Node))
	{
		return FReply::Unhandled();
	}

	if (UVoxelGraphParameterNodeBase* ParameterNode = Cast<UVoxelGraphParameterNodeBase>(Node))
	{
		if (ParameterNode->GetParameterType() == Parameter->ParameterType)
		{
			const FVoxelTransaction Transaction(Graph, "Replace parameter");
			ParameterNode->Guid = ParameterGuid;
			ParameterNode->CachedParameter = *Parameter;
			ParameterNode->ReconstructNode();

			return FReply::Handled();
		}
	}

	return FReply::Unhandled();
}

FReply FVoxelMembersParameterDragDropAction::DroppedOnPanel(const TSharedRef<SWidget>& Panel, FVector2D ScreenPosition, FVector2D GraphPosition, UEdGraph& EdGraph)
{
	UVoxelGraph* Graph = WeakGraph.Get();
	if (!ensure(Graph))
	{
		return FReply::Unhandled();
	}

	if (Graph->MainEdGraph != GetHoveredGraph())
	{
		return FReply::Unhandled();
	}

	const FVoxelGraphParameter* Parameter = Graph->FindParameterByGuid(ParameterGuid);
	if (!ensure(Parameter))
	{
		return FReply::Unhandled();
	}

	if (Parameter->ParameterType == EVoxelGraphParameterType::LocalVariable)
	{
		const FModifierKeysState ModifierKeys = FSlateApplication::Get().GetModifierKeys();
		const bool bModifiedKeysActive = ModifierKeys.IsControlDown() || ModifierKeys.IsAltDown();
		const bool bAutoCreateGetter = bModifiedKeysActive ? ModifierKeys.IsControlDown() : bControlDrag;
		const bool bAutoCreateSetter = bModifiedKeysActive ? ModifierKeys.IsAltDown() : bAltDrag;

		if (bAutoCreateGetter)
		{
			FVoxelGraphSchemaAction_NewParameterUsage Action;
			Action.Guid = ParameterGuid;
			Action.ParameterType = EVoxelGraphParameterType::LocalVariable;
			Action.bLocalVariable_IsDeclaration = false;
			Action.PerformAction(&EdGraph, nullptr, GraphPosition, true);
		}
		else if (bAutoCreateSetter)
		{
			FVoxelGraphSchemaAction_NewParameterUsage Action;
			Action.Guid = ParameterGuid;
			Action.ParameterType = EVoxelGraphParameterType::LocalVariable;
			Action.bLocalVariable_IsDeclaration = true;
			Action.PerformAction(&EdGraph, nullptr, GraphPosition, true);
		}
		else
		{
			FMenuBuilder MenuBuilder(true, nullptr);

			MenuBuilder.BeginSection("BPVariableDroppedOn", FText::FromName(Parameter->Name));
			{
				MenuBuilder.AddMenuEntry(
					FText::FromString("Get " + Parameter->Name.ToString()),
					FText::FromString("Create Getter for local variable '" + Parameter->Name.ToString() + "'\n(Ctrl-drag to automatically create a getter)"),
					FSlateIcon(),
					FUIAction(MakeLambdaDelegate([Guid = ParameterGuid, Position = GraphPosition, WeakGraph = MakeWeakObjectPtr(&EdGraph)]
					{
						UEdGraph* PinnedEdGraph = WeakGraph.Get();
						if (!PinnedEdGraph)
						{
							return;
						}

						FVoxelGraphSchemaAction_NewParameterUsage Action;
						Action.Guid = Guid;
						Action.ParameterType = EVoxelGraphParameterType::LocalVariable;
						Action.bLocalVariable_IsDeclaration = false;
						Action.PerformAction(PinnedEdGraph, nullptr, Position, true);
					}))
				);

				MenuBuilder.AddMenuEntry(
					FText::FromString("Set " + Parameter->Name.ToString()),
					FText::FromString("Create Setter for local variable '" + Parameter->Name.ToString() + "'\n(Alt-drag to automatically create a setter)"),
					FSlateIcon(),
					FUIAction(MakeLambdaDelegate([Guid = ParameterGuid, Position = GraphPosition, WeakGraph = MakeWeakObjectPtr(&EdGraph)]
					{
						UEdGraph* PinnedEdGraph = WeakGraph.Get();
						if (!PinnedEdGraph)
						{
							return;
						}

						FVoxelGraphSchemaAction_NewParameterUsage Action;
						Action.Guid = Guid;
						Action.ParameterType = EVoxelGraphParameterType::LocalVariable;
						Action.bLocalVariable_IsDeclaration = true;
						Action.PerformAction(PinnedEdGraph, nullptr, Position, true);
					}))
				);
			}
			MenuBuilder.EndSection();

			FSlateApplication::Get().PushMenu(
				Panel,
				FWidgetPath(),
				MenuBuilder.MakeWidget(),
				ScreenPosition,
				FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu)
			);
		}

		return FReply::Handled();
	}

	FVoxelGraphSchemaAction_NewParameterUsage Action;
	Action.Guid = ParameterGuid;
	Action.ParameterType = Parameter->ParameterType;
	Action.PerformAction(&EdGraph, nullptr, GraphPosition, true);

	return FReply::Handled();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelMembersParameterDragDropAction::GetDefaultStatusSymbol(const FSlateBrush*& PrimaryBrushOut, FSlateColor& IconColorOut, FSlateBrush const*& SecondaryBrushOut, FSlateColor& SecondaryColorOut) const
{
	FGraphSchemaActionDragDropAction::GetDefaultStatusSymbol(PrimaryBrushOut, IconColorOut, SecondaryBrushOut, SecondaryColorOut);

	UVoxelGraph* Graph = WeakGraph.Get();
	if (!ensure(Graph))
	{
		return;
	}

	const FVoxelGraphParameter* Parameter = Graph->FindParameterByGuid(ParameterGuid);
	if (!ensure(Parameter))
	{
		return;
	}

	PrimaryBrushOut = FVoxelGraphVisuals::GetPinIcon(Parameter->Type).GetIcon();
	IconColorOut = FVoxelGraphVisuals::GetPinColor(Parameter->Type);
}