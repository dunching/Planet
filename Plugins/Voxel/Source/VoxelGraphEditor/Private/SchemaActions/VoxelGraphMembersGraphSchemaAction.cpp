// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelGraphMembersGraphSchemaAction.h"
#include "VoxelGraph.h"
#include "VoxelGraphToolkit.h"
#include "Widgets/SVoxelGraphMembersGraphPaletteItem.h"

TSharedRef<SWidget> FVoxelGraphMembersGraphSchemaAction::CreatePaletteWidget(FCreateWidgetForActionData* const InCreateData) const
{
	return SNew(SVoxelGraphMembersGraphPaletteItem, InCreateData);
}

void FVoxelGraphMembersGraphSchemaAction::OnActionDoubleClick() const
{
	const TSharedPtr<FVoxelGraphToolkit> Toolkit = GetToolkit<FVoxelGraphToolkit>();
	const UEdGraph* EdGraph = GetEdGraph();
	if (!ensure(Toolkit) ||
		!EdGraph)
	{
		return;
	}

	Toolkit->OpenGraphAndBringToFront(EdGraph, true);
}

void FVoxelGraphMembersGraphSchemaAction::OnSelected() const
{
	const TSharedPtr<FVoxelGraphToolkit> Toolkit = GetToolkit<FVoxelGraphToolkit>();
	UEdGraph* EdGraph = GetEdGraph();
	if (!ensure(Toolkit) ||
		!EdGraph)
	{
		return;
	}

	Toolkit->UpdateDetailsView(EdGraph);
	Toolkit->SelectMember(EdGraph, false, false);
}

bool FVoxelGraphMembersGraphSchemaAction::CanRequestRename() const
{
	return false;
}

FString FVoxelGraphMembersGraphSchemaAction::GetName() const
{
	const TSharedPtr<FVoxelGraphToolkit> Toolkit = GetToolkit<FVoxelGraphToolkit>();
	UEdGraph* EdGraph = GetEdGraph();
	if (!ensure(Toolkit) ||
		!EdGraph)
	{
		return "";
	}

	return Toolkit->GetGraphName(EdGraph);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FEdGraphSchemaActionDefiningObject FVoxelGraphMembersGraphSchemaAction::GetPersistentItemDefiningObject() const
{
	return FEdGraphSchemaActionDefiningObject(WeakGraph.Get());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

UEdGraph* FVoxelGraphMembersGraphSchemaAction::GetEdGraph() const
{
	const UVoxelGraph* Graph = WeakGraph.Get();
	if (!ensure(Graph))
	{
		return nullptr;
	}

	UEdGraph* EdGraph = Graph->MainEdGraph;
	if (!ensure(EdGraph))
	{
		return nullptr;
	}

	return EdGraph;
}