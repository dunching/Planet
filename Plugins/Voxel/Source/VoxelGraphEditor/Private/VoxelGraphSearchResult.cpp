// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelGraphSearchResult.h"
#include "VoxelGraph.h"
#include "VoxelGraphVisuals.h"
#include "VoxelGraphToolkit.h"
#include "Widgets/SVoxelGraphSearch.h"
#include "Nodes/VoxelGraphLocalVariableNode.h"
#include "Nodes/VoxelGraphParameterNodeBase.h"

TSharedRef<SWidget> FVoxelGraphSearchResult::GetIcon() const
{
	if (!Icon)
	{
		return SNullWidget::NullWidget;
	}

	return SNew(SImage)
		.Image(Icon)
		.ColorAndOpacity(Color);
}

void FVoxelGraphSearchResult::OnClick(const TWeakPtr<FVoxelGraphToolkit>& WeakToolkit)
{
	const TSharedPtr<FVoxelGraphSearchResult> Parent = WeakParent.Pin();
	if (!Parent)
	{
		return;
	}

	Parent->OnClick(WeakToolkit);
}

void FVoxelGraphSearchResult::AddChild(const TSharedPtr<FVoxelGraphSearchResult>& Child)
{
	Children.Add(Child);
	Child->WeakParent = SharedThis(this);
}

TSharedPtr<FVoxelGraphToolkit> FVoxelGraphSearchResult::GetToolkit(const TWeakPtr<FVoxelGraphToolkit>& WeakToolkit, const TWeakObjectPtr<UVoxelGraph>& WeakGraph) const
{
	if (TSharedPtr<FVoxelGraphToolkit> Toolkit = WeakToolkit.Pin())
	{
		return Toolkit;
	}

	UVoxelGraph* Graph = WeakGraph.Get();
	if (!ensure(Graph))
	{
		return nullptr;
	}

	FVoxelToolkit* Toolkit = FVoxelToolkit::OpenToolkit(Graph, FVoxelGraphToolkit::StaticStruct());
	if (!ensure(Toolkit))
	{
		return nullptr;
	}

	return StaticCastSharedRef<FVoxelGraphToolkit>(Toolkit->AsShared());
}

bool FVoxelGraphSearchResult::StringMatchesSearchTokens(const TArray<FString>& Tokens, FString String, const FVoxelGraphSearchSettings& Settings)
{
	if (Settings.bRemoveSpacesInLookup)
	{
		String = String.Replace(TEXT(" "), TEXT(""));
	}

	for (const FString& Token : Tokens)
	{
		if (!String.Contains(Token))
		{
			return false;
		}
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelGraphSearchGraphResult::FVoxelGraphSearchGraphResult(UVoxelGraph* Graph)
	: WeakGraph(Graph)
{
	if (!Cast<UVoxelGraph>(Graph->GetOuter()))
	{
		Name = Graph->GetClass()->GetDisplayNameText().ToString();
		ensure(Name.RemoveFromStart("Voxel "));

		Icon = FAppStyle::GetBrush(TEXT("GraphEditor.EventGraph_16x"));
	}
	else if (const UVoxelGraph* MacroGraph = Cast<UVoxelGraph>(Graph))
	{
		Name = MacroGraph->GetGraphName();

		Icon = FAppStyle::GetBrush(TEXT("GraphEditor.Macro_16x"));
		Color = MacroGraph->InstanceColor;
	}
	else
	{
		ensure(false);
	}
}

void FVoxelGraphSearchGraphResult::OnClick(const TWeakPtr<FVoxelGraphToolkit>& WeakToolkit)
{
	const UVoxelGraph* Graph = WeakGraph.Get();
	if (!ensure(Graph))
	{
		return;
	}

	if (const TSharedPtr<FVoxelGraphToolkit> Toolkit = GetToolkit(WeakToolkit, WeakGraph))
	{
		Toolkit->OpenGraphAndBringToFront(Graph->MainEdGraph, true);
	}
}

bool FVoxelGraphSearchGraphResult::MatchesTokens(const TArray<FString>& Tokens, const FVoxelGraphSearchSettings& Settings, TArray<TSharedPtr<FVoxelGraphSearchResult>>& AllItems)
{
	ON_SCOPE_EXIT
	{
		AllItems.Append(Children);
	};

	UVoxelGraph* Graph = WeakGraph.Get();
	if (!ensure(Graph) ||
		!ensure(Graph->MainEdGraph))
	{
		return false;
	}

	if (Settings.bParametersLookup)
	{
		for (const FVoxelGraphParameter& Parameter : Graph->Parameters)
		{
			TSharedPtr<FVoxelGraphSearchParameterResult> ParameterResult = MakeVoxelShared<FVoxelGraphSearchParameterResult>(Parameter);
			if (ParameterResult->MatchesTokens(Tokens, Settings, AllItems))
			{
				AddChild(ParameterResult);
			}
		}
	}

	for (UEdGraphNode* Node : Graph->MainEdGraph->Nodes)
	{
		if (Node->IsA<UVoxelGraphParameterNodeBase>())
		{
			if (!Settings.bParameterGettersAndSettersLookup)
			{
				continue;
			}
		}

		TSharedPtr<FVoxelGraphSearchNodeResult> NodeResult = MakeVoxelShared<FVoxelGraphSearchNodeResult>(Graph, Node);
		if (NodeResult->MatchesTokens(Tokens, Settings, AllItems))
		{
			AddChild(NodeResult);
		}
	}

	return
		Children.Num() > 0 ||
		StringMatchesSearchTokens(Tokens, Name, Settings);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelGraphSearchNodeResult::FVoxelGraphSearchNodeResult(UVoxelGraph* Graph, UEdGraphNode* Node)
	: WeakGraph(Graph)
	, WeakNode(Node)
{
	if (const UVoxelGraphParameterNodeBase* ParameterNode = Cast<UVoxelGraphParameterNodeBase>(Node))
	{
		const FVoxelGraphParameter& Parameter = ParameterNode->GetParameterSafe();
		if (ParameterNode->IsA<UVoxelGraphLocalVariableDeclarationNode>() ||
			Parameter.ParameterType == EVoxelGraphParameterType::Output)
		{
			Name = "Set ";
			Icon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "ClassIcon.K2Node_VariableSet").GetOptionalIcon();
		}
		else
		{
			Name = "Get ";
			Icon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "ClassIcon.K2Node_VariableGet").GetOptionalIcon();
		}

		Name += Parameter.Name.ToString();
		Color = FVoxelGraphVisuals::GetPinColor(Parameter.Type);
		Type = "(Type: " + Parameter.Type.ToString() + ")";
	}
	else
	{
		Name = Node->GetNodeTitle(ENodeTitleType::FullTitle).ToString();

		const FSlateIcon NodeIcon = Node->GetIconAndTint(Color);
		Icon = NodeIcon.GetOptionalIcon();
	}

	Name = Name.Replace(TEXT("\n"), TEXT(" "));

	Comment = Node->NodeComment;
}

FText FVoxelGraphSearchNodeResult::GetSubName() const
{
	return FText::FromString(Type);
}

FText FVoxelGraphSearchNodeResult::GetComment() const
{
	if (Comment.IsEmpty())
	{
		return {};
	}
	return FText::FromString("Node Comment: " + Comment);
}

void FVoxelGraphSearchNodeResult::OnClick(const TWeakPtr<FVoxelGraphToolkit>& WeakToolkit)
{
	UEdGraphNode* Node = WeakNode.Get();
	if (!ensure(Node))
	{
		return;
	}

	const TSharedPtr<FVoxelGraphToolkit> Toolkit = GetToolkit(WeakToolkit, WeakGraph);
	if (!ensure(Toolkit))
	{
		return;
	}

	const TSharedPtr<SGraphEditor> GraphEditor = Toolkit->OpenGraphAndBringToFront(Node->GetGraph(), true);
	if (!ensure(GraphEditor))
	{
		return;
	}

	GraphEditor->ClearSelectionSet();
	GraphEditor->SetNodeSelection(Node, true);
	GraphEditor->ZoomToFit(true);
}

bool FVoxelGraphSearchNodeResult::MatchesTokens(const TArray<FString>& Tokens, const FVoxelGraphSearchSettings& Settings, TArray<TSharedPtr<FVoxelGraphSearchResult>>& AllItems)
{
	ON_SCOPE_EXIT
	{
		AllItems.Append(Children);
	};

	UEdGraphNode* Node = WeakNode.Get();
	if (!ensure(Node))
	{
		return false;
	}

	FString TypeLookup;
	if (const UVoxelGraphParameterNodeBase* ParameterNode = Cast<UVoxelGraphParameterNodeBase>(Node))
	{
		const FVoxelGraphParameter& Parameter = ParameterNode->GetParameterSafe();

		if (Settings.bDescriptionLookup)
		{
			if (!Parameter.Description.IsEmpty() &&
				StringMatchesSearchTokens(Tokens, Parameter.Description, Settings))
			{
				AddChild(MakeVoxelShared<FVoxelGraphSearchTextResult>("Description: " + Parameter.Description));
			}
		}

		if (Settings.bDefaultValueLookup)
		{
			const FString DefaultValue = Parameter.DefaultValue.ExportToString();
			if (!DefaultValue.IsEmpty() &&
				StringMatchesSearchTokens(Tokens, DefaultValue, Settings))
			{
				AddChild(MakeVoxelShared<FVoxelGraphSearchTextResult>("Default Value: " + DefaultValue));
			}
		}

		if (Settings.bTypesLookup)
		{
			TypeLookup = " " + Parameter.Type.ToString();
		}

		const FString Guid = Parameter.Guid.ToString();
		for (const FString& Token : Tokens)
		{
			if (Guid == Token)
			{
				return true;
			}
		}
	}
	else
	{
		if (Settings.bNodesLookup &&
			Settings.bDescriptionLookup)
		{
			const FString ToolTip = Node->GetTooltipText().ToString();
			if (!ToolTip.IsEmpty() &&
				StringMatchesSearchTokens(Tokens, ToolTip, Settings))
			{
				AddChild(MakeVoxelShared<FVoxelGraphSearchTextResult>("ToolTip: " + ToolTip));
			}
		}

		for (UEdGraphPin* Pin : Node->Pins)
		{
			if (!ensure(Pin) ||
				!Settings.bPinsLookup)
			{
				continue;
			}

			TSharedPtr<FVoxelGraphSearchPinResult> PinResult = MakeVoxelShared<FVoxelGraphSearchPinResult>(Pin);
			if (PinResult->MatchesTokens(Tokens, Settings, AllItems))
			{
				AddChild(PinResult);
			}
		}
	}

	if (Children.Num() > 0)
	{
		return true;
	}

	if (Settings.bNodesLookup)
	{
		return StringMatchesSearchTokens(Tokens, Name + " " + Comment + TypeLookup, Settings);
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelGraphSearchPinResult::FVoxelGraphSearchPinResult(const UEdGraphPin* Pin)
	: WeakPin(Pin)
{
	Name = Pin->GetSchema()->GetPinDisplayName(Pin).ToString();

	const FVoxelPinType PinType(Pin->PinType);
	Type = "(Type: " + PinType.ToString() + ")";

	Icon = FAppStyle::GetBrush("GraphEditor.PinIcon");
	Color = FVoxelGraphVisuals::GetPinColor(PinType);
}

FText FVoxelGraphSearchPinResult::GetSubName() const
{
	return FText::FromString(Type);
}

bool FVoxelGraphSearchPinResult::MatchesTokens(const TArray<FString>& Tokens, const FVoxelGraphSearchSettings& Settings, TArray<TSharedPtr<FVoxelGraphSearchResult>>& AllItems)
{
	ON_SCOPE_EXIT
	{
		AllItems.Append(Children);
	};

	const UEdGraphPin* Pin = WeakPin.Get();
	if (!ensure(Pin))
	{
		return false;
	}

	if (Settings.bDescriptionLookup &&
		!Pin->PinToolTip.IsEmpty())
	{
		const FVoxelPinType PinType(Pin->PinType);
		FString ToolTip = Pin->PinToolTip;
		ToolTip.RemoveFromEnd("\n\nType: " + PinType.ToString());

		if (StringMatchesSearchTokens(Tokens, ToolTip, Settings))
		{
			AddChild(MakeVoxelShared<FVoxelGraphSearchTextResult>("ToolTip: " + ToolTip));
		}
	}

	if (Settings.bDefaultValueLookup)
	{
		const FString DefaultValue = FVoxelPinValue::MakeFromPinDefaultValue(*Pin).ExportToString();
		if (!DefaultValue.IsEmpty() &&
			StringMatchesSearchTokens(Tokens, DefaultValue, Settings))
		{
			AddChild(MakeVoxelShared<FVoxelGraphSearchTextResult>("Default Value: " + DefaultValue));
		}
	}

	if (Children.Num() > 0)
	{
		return true;
	}

	FString TypeLookup;
	if (Settings.bTypesLookup)
	{
		const FVoxelPinType PinType(Pin->PinType);
		TypeLookup = " " + PinType.ToString();
	}

	return StringMatchesSearchTokens(Tokens, Name + TypeLookup, Settings);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelGraphSearchParameterResult::FVoxelGraphSearchParameterResult(const FVoxelGraphParameter& Parameter)
	: Parameter(Parameter)
{
	Name = Parameter.Name.ToString();

	Type = "(Type: " + Parameter.Type.ToString() + ")";

	Icon = FVoxelGraphVisuals::GetPinIcon(Parameter.Type).GetOptionalIcon();
	Color = FVoxelGraphVisuals::GetPinColor(Parameter.Type);
}

FText FVoxelGraphSearchParameterResult::GetSubName() const
{
	return FText::FromString(Type);
}

bool FVoxelGraphSearchParameterResult::MatchesTokens(const TArray<FString>& Tokens, const FVoxelGraphSearchSettings& Settings, TArray<TSharedPtr<FVoxelGraphSearchResult>>& AllItems)
{
	ON_SCOPE_EXIT
	{
		AllItems.Append(Children);
	};

	if (Settings.bDescriptionLookup &&
		!Parameter.Description.IsEmpty())
	{
		if (StringMatchesSearchTokens(Tokens, Parameter.Description, Settings))
		{
			AddChild(MakeVoxelShared<FVoxelGraphSearchTextResult>("Description: " + Parameter.Description));
		}
	}

	if (Settings.bDefaultValueLookup)
	{
		const FString DefaultValue = Parameter.DefaultValue.ExportToString();
		if (!DefaultValue.IsEmpty() &&
			StringMatchesSearchTokens(Tokens, DefaultValue, Settings))
		{
			AddChild(MakeVoxelShared<FVoxelGraphSearchTextResult>("Default Value: " + DefaultValue));
		}
	}

	if (Children.Num() > 0)
	{
		return true;
	}

	for (const FString& Token : Tokens)
	{
		if (Parameter.Guid.ToString() == Token)
		{
			return true;
		}
	}

	FString TypeLookup;
	if (Settings.bTypesLookup)
	{
		TypeLookup = " " + Parameter.Type.ToString();
	}

	return StringMatchesSearchTokens(Tokens, Name + TypeLookup, Settings);
}