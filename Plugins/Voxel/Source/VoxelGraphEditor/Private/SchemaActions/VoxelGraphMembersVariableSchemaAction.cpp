// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelGraphMembersVariableSchemaAction.h"
#include "VoxelExposedSeed.h"
#include "VoxelGraphToolkit.h"
#include "Widgets/SVoxelGraphMembers.h"
#include "Widgets/SVoxelGraphMembersVariablePaletteItem.h"
#include "Nodes/VoxelGraphParameterNodeBase.h"
#include "DragDropActions/VoxelMembersParameterDragDropAction.h"

TSharedRef<SWidget> FVoxelGraphMembersVariableSchemaAction::CreatePaletteWidget(FCreateWidgetForActionData* const InCreateData) const
{
	return
		SNew(SVoxelGraphMembersVariablePaletteItem, InCreateData)
		.MembersWidget(GetMembersWidget());
}

FReply FVoxelGraphMembersVariableSchemaAction::OnDragged(UObject* Object, const TSharedPtr<FVoxelMembersBaseSchemaAction>& Action, const FPointerEvent& MouseEvent) const
{
	UVoxelGraph* Graph = Cast<UVoxelGraph>(Object);
	if (!ensure(Graph))
	{
		return FReply::Handled();
	}

	return FReply::Handled().BeginDragDrop(FVoxelMembersParameterDragDropAction::New(
		Action,
		Graph,
		ParameterGuid,
		MouseEvent));
}

void FVoxelGraphMembersVariableSchemaAction::OnActionDoubleClick() const
{
	const TSharedPtr<FVoxelGraphToolkit> Toolkit = GetToolkit<FVoxelGraphToolkit>();
	if (!ensure(Toolkit))
	{
		return;
	}

	const TSharedPtr<SGraphEditor> GraphEditor = Toolkit->GetActiveGraphEditor();
	if (!ensure(GraphEditor))
	{
		return;
	}

	GraphEditor->ClearSelectionSet();

	TArray<UVoxelGraphParameterNodeBase*> Nodes;
	GraphEditor->GetCurrentGraph()->GetNodesOfClassEx<UVoxelGraphParameterNodeBase>(Nodes);

	for (UVoxelGraphParameterNodeBase* Node : Nodes)
	{
		if (Node->Guid != ParameterGuid)
		{
			continue;
		}

		GraphEditor->SetNodeSelection(Node, true);
	}

	if (GraphEditor->GetSelectedNodes().Num() > 0)
	{
		GraphEditor->ZoomToFit(true);
	}
}

void FVoxelGraphMembersVariableSchemaAction::OnSelected() const
{
	const TSharedPtr<FVoxelGraphToolkit> Toolkit = GetToolkit<FVoxelGraphToolkit>();
	UVoxelGraph* Graph = WeakOwnerGraph.Get();
	if (!ensure(Toolkit) ||
		!ensure(Graph))
	{
		return;
	}

	Toolkit->SelectParameter(Graph, ParameterGuid, false, false);
}

void FVoxelGraphMembersVariableSchemaAction::GetContextMenuActions(FMenuBuilder& MenuBuilder) const
{
	MenuBuilder.BeginSection("BasicOperations");
	{
		MenuBuilder.AddMenuEntry(FGenericCommands::Get().Rename, NAME_None, INVTEXT("Rename"), INVTEXT("Renames this " + GetVariableTypeName()) );
		MenuBuilder.AddMenuEntry(FGraphEditorCommands::Get().FindReferences);
		MenuBuilder.AddMenuEntry(FGenericCommands::Get().Cut);
		MenuBuilder.AddMenuEntry(FGenericCommands::Get().Copy);
		MenuBuilder.AddMenuEntry(FGenericCommands::Get().Duplicate);
		MenuBuilder.AddMenuEntry(FGenericCommands::Get().Delete);
	}
	MenuBuilder.EndSection();
}

void FVoxelGraphMembersVariableSchemaAction::OnDelete() const
{
	UVoxelGraph* Graph = WeakOwnerGraph.Get();
	if (!ensure(Graph))
	{
		return;
	}

	TArray<FVoxelGraphParameter>& Parameters = Graph->Parameters;

	const int32 Index = Parameters.IndexOfByKey(ParameterGuid);
	if (Index == -1)
	{
		return;
	}

	const FVoxelGraphParameter& Parameter = Parameters[Index];

	const FString& VariableTypeName = GetVariableTypeName();

	bool bDeleteNodes = false;
	if (SVoxelGraphMembers::GetSection(SectionID) != SVoxelGraphMembers::ESection::MacroInputs &&
		SVoxelGraphMembers::GetSection(SectionID) != SVoxelGraphMembers::ESection::MacroOutputs &&
		IsParameterUsed())
	{
		if (!CreateDeletePopups(bDeleteNodes, "Delete " + VariableTypeName, Parameter.Name.ToString()))
		{
			return;
		}
	}

	{
		const FVoxelTransaction Transaction(Graph, "Delete " + VariableTypeName);

		if (bDeleteNodes)
		{
			DeleteParameterNodes();
		}

		Parameters.RemoveAt(Index);
	}

	if (const TSharedPtr<FVoxelGraphToolkit> Toolkit = GetToolkit<FVoxelGraphToolkit>())
	{
		Toolkit->UpdateDetailsView(Toolkit->GetActiveEdGraph());
		Toolkit->SelectMember(nullptr, false, false);
	}
}

void FVoxelGraphMembersVariableSchemaAction::OnDuplicate() const
{
	UVoxelGraph* Graph = WeakOwnerGraph.Get();
	if (!ensure(Graph))
	{
		return;
	}

	TArray<FVoxelGraphParameter>& Parameters = Graph->Parameters;

	const FVoxelGraphParameter* Parameter = Parameters.FindByKey(ParameterGuid);
	if (!Parameter)
	{
		return;
	}

	const FGuid NewGuid = FGuid::NewGuid();

	{
		const FVoxelTransaction Transaction(Graph, "Duplicate " + GetVariableTypeName());

		FVoxelGraphParameter NewParameter = *Parameter;
		NewParameter.Guid = NewGuid;

		Parameters.Add(NewParameter);
	}

	if (const TSharedPtr<FVoxelGraphToolkit> Toolkit = GetToolkit<FVoxelGraphToolkit>())
	{
		Toolkit->SelectParameter(Graph, NewGuid, true, true);
	}
}

bool FVoxelGraphMembersVariableSchemaAction::OnCopy(FString& OutExportText) const
{
	const FVoxelGraphParameter* Parameter = GetParameter();
	if (!ensure(Parameter))
	{
		return false;
	}

	FVoxelGraphParameter::StaticStruct()->ExportText(OutExportText, Parameter, Parameter, nullptr, 0, nullptr);
	return true;
}

FString FVoxelGraphMembersVariableSchemaAction::GetName() const
{
	const FVoxelGraphParameter* Parameter = GetParameter();
	if (!Parameter)
	{
		return {};
	}

	return Parameter->Name.ToString();
}

void FVoxelGraphMembersVariableSchemaAction::SetName(const FString& Name) const
{
	FVoxelGraphParameter* Parameter = GetParameter();
	UVoxelGraph* Graph = WeakOwnerGraph.Get();
	if (!ensure(Parameter) ||
		!ensure(Graph))
	{
		return;
	}

	{
		const FVoxelTransaction Transaction(WeakOwnerGraph, "Rename " + GetVariableTypeName());

		Parameter->Name = *Name;
	}

	if (const TSharedPtr<FVoxelGraphToolkit> Toolkit = GetToolkit<FVoxelGraphToolkit>())
	{
		Toolkit->SelectParameter(Graph, ParameterGuid, false, true);
	}
}

void FVoxelGraphMembersVariableSchemaAction::SetCategory(const FString& NewCategory) const
{
	FVoxelGraphParameter* Parameter = GetParameter();
	if (!ensure(Parameter))
	{
		return;
	}

	Parameter->Category = NewCategory;
}

FString FVoxelGraphMembersVariableSchemaAction::GetSearchString() const
{
	const FVoxelGraphParameter* Parameter = GetParameter();
	if (!Parameter)
	{
		return {};
	}

	return Parameter->Guid.ToString();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelGraphMembersVariableSchemaAction::MovePersistentItemToCategory(const FText& NewCategoryName)
{
	FVoxelGraphParameter* Parameter = GetParameter();
	UVoxelGraph* Graph = WeakOwnerGraph.Get();
	if (!ensure(Parameter) ||
		!ensure(Graph))
	{
		return;
	}

	FString TargetCategory;

	// Need to lookup for original category, since NewCategoryName is returned as display string
	const TArray<FString>& OriginalCategories = Graph->GetCategories(Parameter->ParameterType);
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
		const FVoxelTransaction Transaction(Graph, "Move " + GetVariableTypeName() + " to category");

		Parameter->Category = TargetCategory;
	}

	if (const TSharedPtr<FVoxelGraphToolkit> Toolkit = GetToolkit<FVoxelGraphToolkit>())
	{
		Toolkit->SelectParameter(Graph, ParameterGuid, false, true);
	}
}

int32 FVoxelGraphMembersVariableSchemaAction::GetReorderIndexInContainer() const
{
	const UVoxelGraph* Graph = WeakOwnerGraph.Get();
	if (!ensure(Graph))
	{
		return -1;
	}

	return Graph->Parameters.IndexOfByKey(ParameterGuid);
}

bool FVoxelGraphMembersVariableSchemaAction::ReorderToBeforeAction(const TSharedRef<FEdGraphSchemaAction> OtherAction)
{
	if (OtherAction->SectionID != SectionID)
	{
		return false;
	}

	const TSharedPtr<FVoxelGraphMembersVariableSchemaAction> TargetAction = StaticCastSharedRef<FVoxelGraphMembersVariableSchemaAction>(OtherAction);
	if (!ensure(TargetAction))
	{
		return false;
	}

	UVoxelGraph* OwnerGraph = WeakOwnerGraph.Get();
	if (!ensure(OwnerGraph))
	{
		return false;
	}

	if (TargetAction->ParameterGuid == ParameterGuid ||
		TargetAction->WeakOwnerGraph != WeakOwnerGraph)
	{
		return false;
	}

	const FVoxelGraphParameter* ParameterToMove = GetParameter();
	if (!ParameterToMove)
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

	FVoxelGraphParameter CopierParameter = *ParameterToMove;

	const FVoxelTransaction Transaction(OwnerGraph, "Reorder " + GetVariableTypeName());

	CopierParameter.Category = TargetAction->GetParameter()->Category;

	OwnerGraph->Parameters.RemoveAt(CurrentIndex);
	OwnerGraph->Parameters.Insert(CopierParameter, TargetIndex);

	return true;
}

FEdGraphSchemaActionDefiningObject FVoxelGraphMembersVariableSchemaAction::GetPersistentItemDefiningObject() const
{
	return FEdGraphSchemaActionDefiningObject(WeakOwnerGraph.Get());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelPinType FVoxelGraphMembersVariableSchemaAction::GetPinType() const
{
	const FVoxelGraphParameter* Parameter = GetParameter();
	if (!ensure(Parameter))
	{
		return FVoxelPinType::Make<float>();
	}

	return Parameter->Type;
}

void FVoxelGraphMembersVariableSchemaAction::SetPinType(const FVoxelPinType& NewPinType) const
{
	FVoxelGraphParameter* Parameter = GetParameter();
	UVoxelGraph* Graph = WeakOwnerGraph.Get();
	if (!ensure(Parameter) ||
		!ensure(Graph))
	{
		return;
	}

	{
		const FVoxelTransaction Transaction(Graph, "Change " + GetVariableTypeName() + " type");

		Parameter->Type = NewPinType;
		Parameter->Fixup(nullptr);

		if (Parameter->DefaultValue.Is<FVoxelExposedSeed>())
		{
			Parameter->DefaultValue.Get<FVoxelExposedSeed>().Randomize();
		}
	}

	if (const TSharedPtr<FVoxelGraphToolkit> Toolkit = GetToolkit<FVoxelGraphToolkit>())
	{
		Toolkit->SelectParameter(Graph, ParameterGuid, false, true);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelGraphParameter* FVoxelGraphMembersVariableSchemaAction::GetParameter() const
{
	ensure(
		SectionID == SVoxelGraphMembers::GetSectionId(SVoxelGraphMembers::ESection::Parameters) ||
		SectionID == SVoxelGraphMembers::GetSectionId(SVoxelGraphMembers::ESection::MacroInputs) ||
		SectionID == SVoxelGraphMembers::GetSectionId(SVoxelGraphMembers::ESection::MacroOutputs) ||
		SectionID == SVoxelGraphMembers::GetSectionId(SVoxelGraphMembers::ESection::LocalVariables));

	UVoxelGraph* Graph = WeakOwnerGraph.Get();
	if (!ensure(Graph))
	{
		return nullptr;
	}

	return Graph->Parameters.FindByKey(ParameterGuid);
}

bool FVoxelGraphMembersVariableSchemaAction::IsParameterUsed() const
{
	const TSharedPtr<FVoxelGraphToolkit> Toolkit = GetToolkit<FVoxelGraphToolkit>();
	if (!ensure(Toolkit))
	{
		return false;
	}

	const UEdGraph* EdGraph = Toolkit->GetActiveEdGraph();
	if (!ensure(EdGraph))
	{
		return false;
	}

	TArray<UVoxelGraphParameterNodeBase*> Nodes;
	EdGraph->GetNodesOfClass<UVoxelGraphParameterNodeBase>(Nodes);

	return Nodes.ContainsByPredicate([this](const UVoxelGraphParameterNodeBase* Node)
	{
		return Node->Guid == ParameterGuid;
	});
}

void FVoxelGraphMembersVariableSchemaAction::DeleteParameterNodes() const
{
	const TSharedPtr<FVoxelGraphToolkit> Toolkit = GetToolkit<FVoxelGraphToolkit>();
	if (!ensure(Toolkit))
	{
		return;
	}

	UEdGraph* EdGraph = Toolkit->GetActiveEdGraph();
	if (!ensure(EdGraph))
	{
		return;
	}

	TArray<UVoxelGraphParameterNodeBase*> Nodes;
	EdGraph->GetNodesOfClass<UVoxelGraphParameterNodeBase>(Nodes);

	FVoxelTransaction Transaction(EdGraph);

	for (UVoxelGraphParameterNodeBase* Node : Nodes)
	{
		if (Node->Guid == ParameterGuid)
		{
			EdGraph->RemoveNode(Node);
		}
	}
}

FString FVoxelGraphMembersVariableSchemaAction::GetVariableTypeName() const
{
	UVoxelGraph* Graph = WeakOwnerGraph.Get();
	if (!ensure(Graph))
	{
		return "";
	}

	TArray<FVoxelGraphParameter>& Parameters = Graph->Parameters;

	const int32 Index = Parameters.IndexOfByKey(ParameterGuid);
	if (Index == -1)
	{
		return "";
	}

	return UEnum::GetDisplayValueAsText(Parameters[Index].ParameterType).ToString();
}