// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelGraphMembersMacroSchemaAction.h"
#include "VoxelGraph.h"
#include "VoxelEdGraph.h"
#include "VoxelGraphToolkit.h"
#include "VoxelRuntimeGraph.h"
#include "Widgets/SVoxelMembers.h"
#include "Nodes/VoxelGraphMacroNode.h"
#include "Widgets/SVoxelGraphMembersMacroPaletteItem.h"
#include "DragDropActions/VoxelGraphMacroDragDropAction.h"

TSharedRef<SWidget> FVoxelGraphMembersMacroSchemaAction::CreatePaletteWidget(FCreateWidgetForActionData* const InCreateData) const
{
	return SNew(SVoxelGraphMembersMacroPaletteItem, InCreateData);
}

FReply FVoxelGraphMembersMacroSchemaAction::OnDragged(UObject* Object, const TSharedPtr<FVoxelMembersBaseSchemaAction>& Action, const FPointerEvent& MouseEvent) const
{
	UVoxelGraph* MacroGraph = WeakMacroGraph.Get();
	if (!ensure(MacroGraph))
	{
		return FReply::Unhandled();
	}

	return FReply::Handled().BeginDragDrop(FVoxelGraphMacroDragDropAction::New(Action, MacroGraph));
}

void FVoxelGraphMembersMacroSchemaAction::OnActionDoubleClick() const
{
	const TSharedPtr<FVoxelGraphToolkit> Toolkit = GetToolkit<FVoxelGraphToolkit>();
	const UVoxelGraph* MacroGraph = WeakMacroGraph.Get();
	if (!ensure(Toolkit) ||
		!ensure(MacroGraph) ||
		!ensure(MacroGraph->MainEdGraph))
	{
		return;
	}

	Toolkit->OpenGraphAndBringToFront(MacroGraph->MainEdGraph, true);
}

void FVoxelGraphMembersMacroSchemaAction::OnSelected() const
{
	const TSharedPtr<FVoxelGraphToolkit> Toolkit = GetToolkit<FVoxelGraphToolkit>();
	UVoxelGraph* MacroGraph = WeakMacroGraph.Get();
	if (!ensure(Toolkit) ||
		!ensure(MacroGraph))
	{
		return;
	}

	Toolkit->UpdateDetailsView(MacroGraph);
	Toolkit->SelectMember(MacroGraph, false, false);
}

void FVoxelGraphMembersMacroSchemaAction::GetContextMenuActions(FMenuBuilder& MenuBuilder) const
{
	MenuBuilder.BeginSection("BasicOperations");
	{
		MenuBuilder.AddMenuEntry(FGenericCommands::Get().Rename, NAME_None, INVTEXT("Rename"), INVTEXT("Renames this macro") );
		MenuBuilder.AddMenuEntry(FGraphEditorCommands::Get().FindReferences);
		MenuBuilder.AddMenuEntry(FGenericCommands::Get().Cut);
		MenuBuilder.AddMenuEntry(FGenericCommands::Get().Copy);
		MenuBuilder.AddMenuEntry(FGenericCommands::Get().Duplicate);
		MenuBuilder.AddMenuEntry(FGenericCommands::Get().Delete);
	}
	MenuBuilder.EndSection();
}

void FVoxelGraphMembersMacroSchemaAction::OnDelete() const
{
	const TSharedPtr<FVoxelGraphToolkit> Toolkit = GetToolkit<FVoxelGraphToolkit>();
	UVoxelGraph* MacroGraph = WeakMacroGraph.Get();
	if (!ensure(Toolkit) ||
		!ensure(MacroGraph))
	{
		return;
	}

	bool bDeleteNodes = false;
	if (IsMacroUsed(Toolkit->Asset, MacroGraph))
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

		InvalidateMacroNodes(Toolkit->Asset, MacroGraph, bDeleteNodes);
	}

	UEdGraph* EdGraph = Toolkit->GetActiveEdGraph();
	Toolkit->UpdateDetailsView(EdGraph);
	Toolkit->SelectMember(EdGraph, false, true);
}

void FVoxelGraphMembersMacroSchemaAction::OnDuplicate() const
{
	OnPaste(GetToolkit<FVoxelGraphToolkit>(), WeakMacroGraph.Get());
}

bool FVoxelGraphMembersMacroSchemaAction::OnCopy(FString& OutExportText) const
{
	const UVoxelGraph* Graph = WeakMacroGraph.Get();
	if (!ensure(Graph))
	{
		return false;
	}

	OutExportText = Graph->GetPathName();
	return true;
}

FString FVoxelGraphMembersMacroSchemaAction::GetName() const
{
	const UVoxelGraph* MacroGraph = WeakMacroGraph.Get();
	if (!MacroGraph)
	{
		return "";
	}

	return MacroGraph->GetGraphName();
}

void FVoxelGraphMembersMacroSchemaAction::SetName(const FString& Name) const
{
	UVoxelGraph* MacroGraph = WeakMacroGraph.Get();
	if (!ensure(MacroGraph))
	{
		return;
	}

	{
		const FVoxelTransaction Transaction(MacroGraph, "Rename macro");
		MacroGraph->SetGraphName(Name);
	}

	if (const TSharedPtr<SVoxelMembers> MembersWidget = GetMembersWidget())
	{
		MembersWidget->SelectMember(FName(Name), SectionID, false, true);
	}
}

void FVoxelGraphMembersMacroSchemaAction::SetCategory(const FString& NewCategory) const
{
	UVoxelGraph* MacroGraph = WeakMacroGraph.Get();
	if (!ensure(MacroGraph))
	{
		return;
	}

	MacroGraph->Category = NewCategory;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelGraphMembersMacroSchemaAction::MovePersistentItemToCategory(const FText& NewCategoryName)
{
	UVoxelGraph* MacroGraph = WeakMacroGraph.Get();
	UVoxelGraph* MainGraph = WeakMainGraph.Get();
	if (!ensure(MacroGraph) ||
		!ensure(MainGraph))
	{
		return;
	}

	FString TargetCategory;

	// Need to lookup for original category, since NewCategoryName is returned as display string
	const TArray<FString>& OriginalCategories = MainGraph->InlineMacroCategories.Categories;
	for (int32 Index = 0; Index < OriginalCategories.Num(); Index++)
	{
		FString ReformattedCategory = FEditorCategoryUtils::GetCategoryDisplayString(OriginalCategories[Index]);
		if (ReformattedCategory == NewCategoryName.ToString())
		{
			TargetCategory = OriginalCategories[Index];
			break;
		}
	}

	if (TargetCategory.IsEmpty())
	{
		TargetCategory = NewCategoryName.ToString();
	}

	{
		const FVoxelTransaction Transaction(MainGraph, "Move macro to category");

		MacroGraph->Category = TargetCategory;
	}

	if (const TSharedPtr<SVoxelMembers> MembersWidget = GetMembersWidget())
	{
		MembersWidget->SelectMember(FName(MacroGraph->GetGraphName()), SectionID, false, true);
	}
}

int32 FVoxelGraphMembersMacroSchemaAction::GetReorderIndexInContainer() const
{
	const UVoxelGraph* MacroGraph = WeakMacroGraph.Get();
	const UVoxelGraph* MainGraph = WeakMainGraph.Get();
	if (!ensure(MacroGraph) ||
		!ensure(MainGraph))
	{
		return -1;
	}

	return MainGraph->InlineMacros.IndexOfByKey(MacroGraph);
}

bool FVoxelGraphMembersMacroSchemaAction::ReorderToBeforeAction(TSharedRef<FEdGraphSchemaAction> OtherAction)
{
	if (OtherAction->SectionID != SectionID)
	{
		return false;
	}

	UVoxelGraph* MacroGraph = WeakMacroGraph.Get();
	UVoxelGraph* MainGraph = WeakMainGraph.Get();
	if (!ensure(MacroGraph) ||
		!ensure(MainGraph))
	{
		return false;
	}

	const TSharedPtr<FVoxelGraphMembersMacroSchemaAction> TargetAction = StaticCastSharedRef<FVoxelGraphMembersMacroSchemaAction>(OtherAction);
	if (!ensure(TargetAction))
	{
		return false;
	}

	if (TargetAction->WeakMacroGraph == WeakMacroGraph ||
		TargetAction->WeakMainGraph != WeakMainGraph)
	{
		return false;
	}

	int32 TargetIndex = TargetAction->GetReorderIndexInContainer();
	if (TargetIndex == -1)
	{
		return false;
	}

	const int32 CurrentIndex = GetReorderIndexInContainer();
	if (TargetIndex > CurrentIndex)
	{
		TargetIndex--;
	}

	const FVoxelTransaction Transaction(MainGraph, "Reorder macro");

	MacroGraph->Category = TargetAction->WeakMacroGraph->Category;

	MainGraph->InlineMacros.RemoveAt(CurrentIndex);
	MainGraph->InlineMacros.Insert(MacroGraph, TargetIndex);

	return true;
}

FEdGraphSchemaActionDefiningObject FVoxelGraphMembersMacroSchemaAction::GetPersistentItemDefiningObject() const
{
	const TSharedPtr<FVoxelGraphToolkit> Toolkit = GetToolkit<FVoxelGraphToolkit>();
	if (!Toolkit)
	{
		return nullptr;
	}

	return FEdGraphSchemaActionDefiningObject(Toolkit->Asset);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelGraphMembersMacroSchemaAction::OnPaste(const TSharedPtr<FVoxelGraphToolkit>& Toolkit, const UVoxelGraph* GraphToCopy)
{
	if (!ensure(Toolkit) ||
		!ensure(GraphToCopy))
	{
		return;
	}

	UVoxelGraph* Owner = Toolkit->Asset;
	FName NewMacroName = *GraphToCopy->GetGraphName();

	TSet<FName> UsedNames;
	for (const UVoxelGraph* InlineMacro : Owner->InlineMacros)
	{
		UsedNames.Add(*InlineMacro->GetGraphName());
	}

	while (UsedNames.Contains(NewMacroName))
	{
		NewMacroName.SetNumber(NewMacroName.GetNumber() + 1);
	}

	UVoxelGraph* NewGraph;
	{
		const FVoxelTransaction Transaction(Owner, "Paste macro");

		NewGraph = DuplicateObject<UVoxelGraph>(GraphToCopy, Owner, {});
		if (!ensure(NewGraph))
		{
			return;
		}

		NewGraph->MainEdGraph = DuplicateObject<UVoxelEdGraph>(CastChecked<UVoxelEdGraph>(GraphToCopy->MainEdGraph), NewGraph);
		NewGraph->SetGraphName(NewMacroName.ToString());
		Toolkit->FixupGraph(NewGraph);

		Owner->InlineMacros.Add(NewGraph);
	}

	Toolkit->UpdateDetailsView(NewGraph);
	Toolkit->OpenGraphAndBringToFront(NewGraph->MainEdGraph, false);
	Toolkit->SelectMember(NewGraph, true, true);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool FVoxelGraphMembersMacroSchemaAction::IsMacroUsed(UVoxelGraph* LookupGraph, UVoxelGraph* MacroGraph) const
{
	const auto IsMacroUsedInEdGraph = [MacroGraph](const UEdGraph* EdGraph)
	{
		if (!ensure(EdGraph))
		{
			return false;
		}

		TArray<UVoxelGraphMacroNode*> MemberNodes;
		EdGraph->GetNodesOfClass<UVoxelGraphMacroNode>(MemberNodes);

		return MemberNodes.ContainsByPredicate([&](const UVoxelGraphMacroNode* Node)
		{
			if (!ensure(Node) ||
				!ensure(Node->GraphInterface))
			{
				return false;
			}

			return Node->GraphInterface == MacroGraph;
		});
	};

	// Macro library main graph is null
	if (LookupGraph->MainEdGraph)
	{
		if (IsMacroUsedInEdGraph(LookupGraph->MainEdGraph))
		{
			return true;
		}
	}

	for (const UVoxelGraph* Graph : LookupGraph->InlineMacros)
	{
		if (!ensure(Graph))
		{
			continue;
		}

		if (MacroGraph == Graph)
		{
			continue;
		}

		if (IsMacroUsedInEdGraph(Graph->MainEdGraph))
		{
			return true;
		}
	}

	return false;
}

void FVoxelGraphMembersMacroSchemaAction::InvalidateMacroNodes(UVoxelGraph* LookupGraph, UVoxelGraph* MacroGraph, bool bDelete) const
{
	const auto DeleteMacroUsages = [&](UEdGraph* EdGraph)
	{
		if (!ensure(EdGraph))
		{
			return;
		}

		TArray<UVoxelGraphMacroNode*> MacroNodes;
		EdGraph->GetNodesOfClass<UVoxelGraphMacroNode>(MacroNodes);

		for (UVoxelGraphMacroNode* MacroNode : MacroNodes)
		{
			if (!ensure(MacroNode) ||
				!ensure(MacroNode->GraphInterface))
			{
				continue;
			}

			if (MacroNode->GraphInterface != MacroGraph)
			{
				continue;
			}

			if (bDelete)
			{
				EdGraph->RemoveNode(MacroNode);
			}
			else
			{
				MacroNode->GraphInterface = nullptr;
				MacroNode->ReconstructNode();
			}
		}
	};

	DeleteMacroUsages(LookupGraph->MainEdGraph);

	for (const UVoxelGraph* Graph : LookupGraph->InlineMacros)
	{
		if (!ensure(Graph))
		{
			continue;
		}

		DeleteMacroUsages(Graph->MainEdGraph);
	}
}