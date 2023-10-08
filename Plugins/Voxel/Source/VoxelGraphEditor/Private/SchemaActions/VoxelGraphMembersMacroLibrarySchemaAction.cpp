// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelGraphMembersMacroLibrarySchemaAction.h"

#include "VoxelGraph.h"
#include "VoxelGraphSearchManager.h"
#include "Widgets/SVoxelGraphMembersMacroLibraryPaletteItem.h"

TSharedRef<SWidget> FVoxelGraphMembersMacroLibrarySchemaAction::CreatePaletteWidget(FCreateWidgetForActionData* const InCreateData) const
{
	return SNew(SVoxelGraphMembersMacroLibraryPaletteItem, InCreateData);
}

void FVoxelGraphMembersMacroLibrarySchemaAction::OnDelete() const
{
	const TSharedPtr<FVoxelGraphToolkit> Toolkit = GetToolkit<FVoxelGraphToolkit>();
	UVoxelGraph* MacroGraph = WeakMacroGraph.Get();
	if (!ensure(Toolkit) ||
		!ensure(MacroGraph))
	{
		return;
	}

	TMap<UObject*, UVoxelGraph*> Assets = FVoxelGraphSearchManager::Get().GetAllLookupGraphs();

	TMap<UObject*, UVoxelGraph*> AffectedAssets;
	for (const auto& It : Assets)
	{
		if (!ensure(It.Value))
		{
			continue;
		}

		if (IsMacroUsed(It.Value, MacroGraph))
		{
			AffectedAssets.Add(It.Key, It.Value);
		}
	}

	bool bDeleteNodes = false;
	if (AffectedAssets.Num() > 0)
	{
		if (!CreateDeletePopups(bDeleteNodes, "Delete Macro", MacroGraph->GetGraphName()))
		{
			return;
		}
	}

	Toolkit->CloseGraph(MacroGraph->MainEdGraph);

	{
		const FVoxelTransaction Transaction(Toolkit->Asset, "Delete macro");
		Toolkit->Asset->InlineMacros.Remove(MacroGraph);
		MacroGraph->MarkAsGarbage();
	}

	for (const auto& It : AffectedAssets)
	{
		const FVoxelTransaction Transaction(It.Key, "Delete macro usages");
		InvalidateMacroNodes(It.Value, MacroGraph, bDeleteNodes);
	}

	UEdGraph* EdGraph = Toolkit->GetActiveEdGraph();
	Toolkit->UpdateDetailsView(EdGraph);
	Toolkit->SelectMember(EdGraph, false, true);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool FVoxelGraphMembersMacroLibrarySchemaAction::IsMacroVisible() const
{
	const UVoxelGraph* MacroGraph = WeakMacroGraph.Get();
	if (!ensure(MacroGraph))
	{
		return false;
	}

	return MacroGraph->bExposeToLibrary;
}

void FVoxelGraphMembersMacroLibrarySchemaAction::ToggleMacroVisibility() const
{
	UVoxelGraph* MacroGraph = WeakMacroGraph.Get();
	if (!ensure(MacroGraph))
	{
		return;
	}

	const FVoxelTransaction Transaction(MacroGraph, "Toggle macro visibility");

	MacroGraph->bExposeToLibrary = !MacroGraph->bExposeToLibrary;
}