// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelGraphMacroDragDropAction.h"
#include "VoxelGraphSchema.h"
#include "VoxelGraphToolkit.h"
#include "VoxelMacroLibrary.h"

void FVoxelGraphMacroDragDropAction::HoverTargetChanged()
{
	if (const UEdGraph* EdGraph = GetHoveredGraph())
	{
		const UVoxelGraph* MacroGraph = WeakMacroGraph.Get();
		if (!ensure(MacroGraph))
		{
			return;
		}

		if (MacroGraph->MainEdGraph == EdGraph)
		{
			SetFeedbackMessageError("Macro cannot be used in itself");
			return;
		}

		if (const TSharedPtr<FVoxelGraphToolkit> MacroToolkit = UVoxelGraphSchema::GetToolkit(MacroGraph->MainEdGraph))
		{
			if (!MacroToolkit->bIsMacroLibrary)
			{
				if (const TSharedPtr<FVoxelGraphToolkit> TargetToolkit = UVoxelGraphSchema::GetToolkit(EdGraph))
				{
					if (MacroToolkit->Asset != TargetToolkit->Asset)
					{
						SetFeedbackMessageError("Voxel Graph macro cannot be used in other graph");
						return;
					}
				}
			}
		}
	}

	FVoxelMembersBaseDragDropAction::HoverTargetChanged();
}

FReply FVoxelGraphMacroDragDropAction::DroppedOnPanel(const TSharedRef<SWidget>& Panel, FVector2D ScreenPosition, FVector2D GraphPosition, UEdGraph& EdGraph)
{
	UVoxelGraph* Graph = WeakMacroGraph.Get();
	if (!ensure(Graph))
	{
		return FReply::Handled();
	}

	if (Graph->MainEdGraph == &EdGraph)
	{
		return FReply::Handled();
	}

	if (const TSharedPtr<FVoxelGraphToolkit> MacroToolkit = UVoxelGraphSchema::GetToolkit(Graph->MainEdGraph))
	{
		if (!MacroToolkit->bIsMacroLibrary)
		{
			if (const TSharedPtr<FVoxelGraphToolkit> TargetToolkit = UVoxelGraphSchema::GetToolkit(&EdGraph))
			{
				if (MacroToolkit->Asset != TargetToolkit->Asset)
				{
					return FReply::Handled();
				}
			}
		}
	}

	FVoxelGraphSchemaAction_NewMacroNode Action;
	Action.Graph = Graph;
	Action.PerformAction(&EdGraph, nullptr, GraphPosition, true);

	return FReply::Handled();
}