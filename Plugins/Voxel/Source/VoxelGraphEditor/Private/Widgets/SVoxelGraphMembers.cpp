// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "SVoxelGraphMembers.h"

#include "VoxelEdGraph.h"
#include "VoxelGraphSchema.h"
#include "VoxelGraphToolkit.h"
#include "SchemaActions/VoxelGraphMembersGraphSchemaAction.h"
#include "SchemaActions/VoxelGraphMembersMacroSchemaAction.h"
#include "SchemaActions/VoxelGraphMembersVariableSchemaAction.h"
#include "SchemaActions/VoxelGraphMembersMacroLibrarySchemaAction.h"

void SVoxelGraphMembers::Construct(const FArguments& Args)
{
	SVoxelMembers::Construct(
		SVoxelMembers::FArguments()
		.Object(Args._Graph)
		.Toolkit(Args._Toolkit));

	const TSharedPtr<FVoxelGraphToolkit> Toolkit = GetToolkit<FVoxelGraphToolkit>();
	if (!ensure(Toolkit))
	{
		return;
	}

	const UVoxelGraph* MainGraph = Toolkit->Asset;
	if (!ensure(MainGraph))
	{
		return;
	}

	MainGraph->OnParametersChanged.AddSP(this, &SVoxelGraphMembers::OnParametersChanged);

	UpdateActiveGraph(GetObject<UVoxelGraph>());
}

void SVoxelGraphMembers::UpdateActiveGraph(const TWeakObjectPtr<UVoxelGraph>& NewActiveGraph)
{
	const TSharedPtr<FVoxelGraphToolkit> Toolkit = GetToolkit<FVoxelGraphToolkit>();
	if (!ensure(Toolkit))
	{
		return;
	}

	const UVoxelGraph* MainGraph = Toolkit->Asset;
	if (!ensure(MainGraph))
	{
		return;
	}

	if (const UVoxelGraph* ActiveGraph = GetObject<UVoxelGraph>())
	{
		if (OnMembersChangedHandle.IsValid())
		{
			ActiveGraph->OnParametersChanged.Remove(OnMembersChangedHandle);
		}
	}

	SetObject(NewActiveGraph);

	const UVoxelGraph* ActiveGraph = GetObject<UVoxelGraph>();
	if (!ActiveGraph ||
		ActiveGraph == MainGraph)
	{
		return;
	}

	OnMembersChangedHandle = ActiveGraph->OnParametersChanged.AddSP(this, &SVoxelGraphMembers::OnParametersChanged);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void SVoxelGraphMembers::CollectStaticSections(TArray<int32>& StaticSectionIds)
{
	const TSharedPtr<FVoxelGraphToolkit> Toolkit = GetToolkit<FVoxelGraphToolkit>();
	if (Toolkit->bIsMacroLibrary)
	{
		StaticSectionIds.Add(GetSectionId(ESection::MacroLibraries));
	}
	else
	{
		StaticSectionIds.Add(GetSectionId(ESection::Graph));
		StaticSectionIds.Add(GetSectionId(ESection::InlineMacros));
	}

	const UVoxelGraph* Graph = GetObject<UVoxelGraph>();
	if (!Graph)
	{
		return;
	}

	StaticSectionIds.Add(GetSectionId(ESection::MacroInputs));
	StaticSectionIds.Add(GetSectionId(ESection::MacroOutputs));

	if (!Toolkit->bIsMacroLibrary)
	{
		StaticSectionIds.Add(GetSectionId(ESection::Parameters));
	}

	StaticSectionIds.Add(GetSectionId(ESection::LocalVariables));
}

FText SVoxelGraphMembers::OnGetSectionTitle(const int32 SectionId)
{
	static const TArray<FText> NodeSectionNames
	{
		INVTEXT(""),
		INVTEXT("Graph"),
		INVTEXT("Macros"),
		INVTEXT("Macros"),
		INVTEXT("Parameters"),
		INVTEXT("Inputs"),
		INVTEXT("Outputs"),
		INVTEXT("Local Variables"),
	};

	if (!ensure(NodeSectionNames.IsValidIndex(SectionId)))
	{
		return {};
	}

	return NodeSectionNames[SectionId];
}

TSharedRef<SWidget> SVoxelGraphMembers::OnGetMenuSectionWidget(TSharedRef<SWidget> RowWidget, int32 SectionId)
{
	switch (GetSection(SectionId))
	{
	default: check(false);
	case ESection::Graph: return SNullWidget::NullWidget;
	case ESection::InlineMacros: return CreateAddButton(SectionId, INVTEXT("Macro"), "AddNewMacro");
	case ESection::MacroLibraries: return CreateAddButton(SectionId, INVTEXT("Macro"), "AddNewMacro");
	case ESection::Parameters: return CreateAddButton(SectionId, INVTEXT("Parameter"), "AddNewParameter");
	case ESection::MacroInputs: return CreateAddButton(SectionId, INVTEXT("Input"), "AddNewInput");
	case ESection::MacroOutputs: return CreateAddButton(SectionId, INVTEXT("Output"), "AddNewOutput");
	case ESection::LocalVariables: return CreateAddButton(SectionId, INVTEXT("Local Variable"), "AddNewLocalVariable");
	}
}

void SVoxelGraphMembers::CollectSortedActions(FVoxelMembersActionsSortHelper& OutActionsList)
{
	const TSharedPtr<FVoxelGraphToolkit> Toolkit = GetToolkit<FVoxelGraphToolkit>();
	if (!ensure(Toolkit))
	{
		return;
	}

	UVoxelGraph& MainGraph = *Toolkit->Asset;

	OutActionsList.AddCategoriesSortList(GetSectionId(ESection::None), {});

	if (Toolkit->bIsMacroLibrary)
	{
		OutActionsList.AddCategoriesSortList(GetSectionId(ESection::MacroLibraries), MainGraph.InlineMacroCategories.Categories);
	}
	else
	{
		OutActionsList.AddCategoriesSortList(GetSectionId(ESection::Graph), {});
		OutActionsList.AddCategoriesSortList(GetSectionId(ESection::InlineMacros), MainGraph.InlineMacroCategories.Categories);
	}

	INLINE_LAMBDA
	{
		if (Toolkit->bIsMacroLibrary)
		{
			for (UVoxelGraph* Macro : MainGraph.InlineMacros)
			{
				TSharedRef<FVoxelGraphMembersMacroLibrarySchemaAction> NewMacroAction = MakeVoxelShared<FVoxelGraphMembersMacroLibrarySchemaAction>(
					FText::FromString(Macro->Category),
					FText::FromString(Macro->GetGraphName()),
					FText::FromString(Macro->Description),
					0,
					INVTEXT("macro"),
					GetSectionId(ESection::MacroLibraries));

				NewMacroAction->WeakToolkit = Toolkit;
				NewMacroAction->WeakMembersWidget = SharedThis(this);
				NewMacroAction->WeakMainGraph = &MainGraph;
				NewMacroAction->WeakMacroGraph = Macro;

				OutActionsList.AddAction(NewMacroAction, Macro->Category);
			}
			return;
		}

		{
			const TSharedRef<FVoxelGraphMembersGraphSchemaAction> NewGraphAction = MakeVoxelShared<FVoxelGraphMembersGraphSchemaAction>(
				INVTEXT(""),
				FText::FromString(Toolkit->GetGraphName(MainGraph.MainEdGraph)),
				INVTEXT(""),
				2,
				INVTEXT(""),
				GetSectionId(ESection::Graph));

			NewGraphAction->WeakToolkit = Toolkit;
			NewGraphAction->WeakMembersWidget = SharedThis(this);
			NewGraphAction->WeakGraph = &MainGraph;

			OutActionsList.AddAction(NewGraphAction, "");
		}

		for (UVoxelGraph* InlineMacro : MainGraph.InlineMacros)
		{
			TSharedRef<FVoxelGraphMembersMacroSchemaAction> NewMacroAction = MakeVoxelShared<FVoxelGraphMembersMacroSchemaAction>(
				FText::FromString(InlineMacro->Category),
				FText::FromString(InlineMacro->GetGraphName()),
				FText::FromString(InlineMacro->Description),
				2,
				INVTEXT("macro"),
				GetSectionId(ESection::InlineMacros));

			NewMacroAction->WeakToolkit = Toolkit;
			NewMacroAction->WeakMembersWidget = SharedThis(this);
			NewMacroAction->WeakMainGraph = &MainGraph;
			NewMacroAction->WeakMacroGraph = InlineMacro;

			OutActionsList.AddAction(NewMacroAction, InlineMacro->Category);
		}
	};

	UVoxelGraph* ActiveGraph = GetObject<UVoxelGraph>();
	if (!ActiveGraph)
	{
		return;
	}

	OutActionsList.AddCategoriesSortList(GetSectionId(ESection::MacroInputs), ActiveGraph->GetCategories(EVoxelGraphParameterType::Input));
	OutActionsList.AddCategoriesSortList(GetSectionId(ESection::MacroOutputs), ActiveGraph->GetCategories(EVoxelGraphParameterType::Output));

	if (!Toolkit->bIsMacroLibrary)
	{
		OutActionsList.AddCategoriesSortList(GetSectionId(ESection::Parameters), ActiveGraph->GetCategories(EVoxelGraphParameterType::Parameter));
	}

	OutActionsList.AddCategoriesSortList(GetSectionId(ESection::LocalVariables), ActiveGraph->GetCategories(EVoxelGraphParameterType::LocalVariable));

	for (const FVoxelGraphParameter& Parameter : ActiveGraph->Parameters)
	{
		TSharedRef<FVoxelGraphMembersVariableSchemaAction> NewParameterAction = MakeVoxelShared<FVoxelGraphMembersVariableSchemaAction>(
			FText::FromString(Parameter.Category),
			FText::FromName(Parameter.Name),
			FText::FromString(Parameter.Description),
			1,
			FText::FromString(Parameter.Type.ToString()),
			GetSectionId(Parameter.ParameterType));

		NewParameterAction->WeakToolkit = Toolkit;
		NewParameterAction->WeakMembersWidget = SharedThis(this);
		NewParameterAction->WeakOwnerGraph = ActiveGraph;
		NewParameterAction->ParameterGuid = Parameter.Guid;

		OutActionsList.AddAction(NewParameterAction, Parameter.Category);
	}
}

void SVoxelGraphMembers::SelectBaseObject()
{
	if (const TSharedPtr<FVoxelGraphToolkit> Toolkit = GetToolkit<FVoxelGraphToolkit>())
	{
		Toolkit->UpdateDetailsView(Toolkit->GetActiveEdGraph());
		Toolkit->SelectMember(nullptr, false, false);
	}
}

void SVoxelGraphMembers::GetContextMenuAddOptions(FMenuBuilder& MenuBuilder)
{
	const TSharedPtr<FVoxelGraphToolkit> Toolkit = GetToolkit<FVoxelGraphToolkit>();
	if (!ensure(Toolkit))
	{
		return;
	}

	MenuBuilder.AddMenuEntry(
		INVTEXT("Add new Macro"),
		FText(),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "BlueprintEditor.AddNewMacroDeclaration"),
		FUIAction{
			FExecuteAction::CreateSP(this, &SVoxelGraphMembers::OnAddNewMember, GetSectionId(Toolkit->bIsMacroLibrary ? ESection::MacroLibraries : ESection::InlineMacros))
		});

	if (!GetObject<UVoxelGraph>())
	{
		return;
	}

	MenuBuilder.AddMenuEntry(
		INVTEXT("Add new Input"),
		FText(),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "BlueprintEditor.AddNewVariable"),
		FUIAction{
			FExecuteAction::CreateSP(this, &SVoxelGraphMembers::OnAddNewMember, GetSectionId(ESection::MacroInputs))
		});
	MenuBuilder.AddMenuEntry(
		INVTEXT("Add new Output"),
		FText(),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "BlueprintEditor.AddNewVariable"),
		FUIAction{
			FExecuteAction::CreateSP(this, &SVoxelGraphMembers::OnAddNewMember, GetSectionId(ESection::MacroOutputs))
		});

	MenuBuilder.AddMenuEntry(
		INVTEXT("Add new Parameter"),
		FText(),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "BlueprintEditor.AddNewVariable"),
		FUIAction{
			FExecuteAction::CreateSP(this, &SVoxelGraphMembers::OnAddNewMember, GetSectionId(ESection::Parameters))
		});

	MenuBuilder.AddMenuEntry(
		INVTEXT("Add new Local Variable"),
		FText(),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "BlueprintEditor.AddNewLocalVariable"),
		FUIAction{
			FExecuteAction::CreateSP(this, &SVoxelGraphMembers::OnAddNewMember, GetSectionId(ESection::LocalVariables))
		});
}

void SVoxelGraphMembers::OnPasteItem(const FString& ImportText, int32 SectionId)
{
	UVoxelGraph* Graph = GetObject<UVoxelGraph>();
	const TSharedPtr<FVoxelGraphToolkit> Toolkit = GetToolkit<FVoxelGraphToolkit>();
	if (!Graph ||
		!ensure(Toolkit))
	{
		return;
	}

	FStringOutputDevice Errors;
	const ESection Section = GetSection(SectionId);

	if (Section == ESection::InlineMacros ||
		Section == ESection::MacroLibraries)
	{
		const UVoxelGraph* MacroGraph = FindObject<UVoxelGraph>(nullptr, *ImportText);
		if (!MacroGraph)
		{
			return;
		}

		FVoxelGraphMembersMacroSchemaAction::OnPaste(Toolkit, MacroGraph);
	}
	else if (
		Section == ESection::Parameters ||
		Section == ESection::MacroInputs ||
		Section == ESection::MacroOutputs ||
		Section == ESection::LocalVariables)
	{
		FVoxelGraphParameter NewParameter;
		FVoxelGraphParameter::StaticStruct()->ImportText(
			*ImportText,
			&NewParameter,
			nullptr,
			0,
			&Errors,
			FVoxelGraphParameter::StaticStruct()->GetName());

		if (!Errors.IsEmpty())
		{
			return;
		}

		NewParameter.Guid = FGuid::NewGuid();
		NewParameter.Category = GetPasteCategory();

		{
			const FVoxelTransaction Transaction(Graph, "Paste " + OnGetSectionTitle(SectionId).ToString());
			Graph->Parameters.Add(NewParameter);
		}

		Toolkit->SelectParameter(Graph, NewParameter.Guid, true, true);
	}
}

bool SVoxelGraphMembers::CanPasteItem(const FString& ImportText, int32 SectionId)
{
	FStringOutputDevice Errors;
	const ESection Section = GetSection(SectionId);

	if (Section == ESection::InlineMacros ||
		Section == ESection::MacroLibraries)
	{
		return FindObject<UVoxelGraph>(nullptr, *ImportText) != nullptr;
	}
	else if (
		Section == ESection::Parameters ||
		Section == ESection::MacroInputs ||
		Section == ESection::MacroOutputs ||
		Section == ESection::LocalVariables)
	{
		FVoxelGraphParameter Parameter;
		FVoxelGraphParameter::StaticStruct()->ImportText(
			*ImportText,
			&Parameter,
			nullptr,
			0,
			&Errors,
			FVoxelGraphParameter::StaticStruct()->GetName());
	}
	else
	{
		return false;
	}

	return Errors.IsEmpty();
}

void SVoxelGraphMembers::OnAddNewMember(int32 SectionId)
{
	const TSharedPtr<FVoxelGraphToolkit> Toolkit = GetToolkit<FVoxelGraphToolkit>();
	if (!ensure(Toolkit))
	{
		return;
	}

	const ESection Section = GetSection(SectionId);
	if (!ensure(Section != ESection::None) ||
		!ensure(Section != ESection::Graph))
	{
		return;
	}

	if (Section == ESection::InlineMacros ||
		Section == ESection::MacroLibraries)
	{
		FVoxelGraphSchemaAction_NewInlineMacro Action;
		Action.TargetCategory = GetPasteCategory();
		Action.PerformAction(Toolkit->Asset->MainEdGraph, nullptr, Toolkit->FindLocationInGraph());
		return;
	}

	UEdGraph* EdGraph = Toolkit->GetActiveEdGraph();
	if (!ensure(EdGraph))
	{
		return;
	}

	FVoxelGraphSchemaAction_NewParameter Action;
	Action.TargetCategory = GetPasteCategory();
	Action.ParameterType = GetParameterType(Section);
	Action.PerformAction(EdGraph, nullptr, Toolkit->FindLocationInGraph());
}

const TArray<FString>& SVoxelGraphMembers::GetCopyPrefixes() const
{
	static const TArray<FString> CopyPrefixes
	{
		"INVALID:",
		"INVALID:",
		"GraphInlineMacro:",
		"GraphMacroLibrary:",
		"GraphParameter:",
		"GraphMacroInput:",
		"GraphMacroOutput:",
		"GraphLocalVariable:",
	};

	return CopyPrefixes;
}

TArray<FString>& SVoxelGraphMembers::GetEditableCategories(const int32 SectionId)
{
	const ESection Section = GetSection(SectionId);
	if (Section == ESection::InlineMacros ||
		Section == ESection::MacroLibraries)
	{
		return GetToolkit<FVoxelGraphToolkit>()->Asset->InlineMacroCategories.Categories;
	}

	UVoxelGraph* Graph = GetObject<UVoxelGraph>();
	if (!ensure(Graph))
	{
		static TArray<FString> CategoriesList;
		return CategoriesList;
	}

	return Graph->GetCategories(GetParameterType(Section));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void SVoxelGraphMembers::OnParametersChanged(const UVoxelGraph::EParameterChangeType ChangeType)
{
	switch (ChangeType)
	{
	default: check(false);
	case UVoxelGraph::EParameterChangeType::Unknown: RequestRefresh(); break;
	case UVoxelGraph::EParameterChangeType::DefaultValue: break;
	}
}