// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelGraphSchema.h"
#include "VoxelGraph.h"
#include "VoxelNode.h"
#include "VoxelEdGraph.h"
#include "VoxelNodeLibrary.h"
#include "VoxelGraphVisuals.h"
#include "VoxelGraphToolkit.h"
#include "VoxelMacroLibrary.h"
#include "VoxelRuntimeGraph.h"
#include "VoxelGraphInstance.h"
#include "VoxelMakeValueNode.h"
#include "Templates/VoxelOperatorNodes.h"
#include "Nodes/VoxelGraphKnotNode.h"
#include "Nodes/VoxelGraphMacroNode.h"
#include "Nodes/VoxelGraphStructNode.h"
#include "Nodes/VoxelGraphParameterNode.h"
#include "Nodes/VoxelGraphLocalVariableNode.h"
#include "Nodes/VoxelGraphMacroParameterNode.h"
#include "Widgets/SVoxelGraphPinTypeSelector.h"
#include "SchemaActions/VoxelGraphExpandMacroSchemaAction.h"
#include "SchemaActions/VoxelGraphCollapseToMacroSchemaAction.h"

#include "ToolMenu.h"

UEdGraphNode* FVoxelGraphSchemaAction_NewMacroNode::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	const FVoxelTransaction Transaction(ParentGraph, "New graph node");

	FGraphNodeCreator<UVoxelGraphMacroNode> NodeCreator(*ParentGraph);
	UVoxelGraphMacroNode* Node = NodeCreator.CreateUserInvokedNode(bSelectNewNode);
	Node->GraphInterface = Graph;
	Node->NodePosX = Location.X;
	Node->NodePosY = Location.Y;
	NodeCreator.Finalize();

	if (FromPin)
	{
		Node->AutowireNewNode(FromPin);
	}

	return Node;
}

void FVoxelGraphSchemaAction_NewMacroNode::GetIcon(FSlateIcon& Icon, FLinearColor& Color)
{
	static const FSlateIcon MacroIcon("EditorStyle", "GraphEditor.Macro_16x");
	Icon = MacroIcon;
}

UEdGraphNode* FVoxelGraphSchemaAction_NewParameterUsage::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	check(Guid.IsValid());
	const FVoxelTransaction Transaction(ParentGraph, "Create new variable usage");

	UEdGraphNode* NewNode;
	if (ParameterType == EVoxelGraphParameterType::Parameter)
	{
		FGraphNodeCreator<UVoxelGraphParameterNode> NodeCreator(*ParentGraph);
		UVoxelGraphParameterNode* Node = NodeCreator.CreateUserInvokedNode(bSelectNewNode);
		Node->Guid = Guid;
		Node->NodePosX = Location.X;
		Node->NodePosY = Location.Y;
		NodeCreator.Finalize();

		NewNode = Node;
	}
	else if (ParameterType == EVoxelGraphParameterType::Input)
	{
		FGraphNodeCreator<UVoxelGraphMacroParameterInputNode> NodeCreator(*ParentGraph);
		UVoxelGraphMacroParameterInputNode* Node = NodeCreator.CreateUserInvokedNode(bSelectNewNode);
		Node->bExposeDefaultPin = bInput_ExposeDefaultAsPin;
		Node->Guid = Guid;
		Node->NodePosX = Location.X;
		Node->NodePosY = Location.Y;
		NodeCreator.Finalize();

		NewNode = Node;
	}
	else if (ParameterType == EVoxelGraphParameterType::Output)
	{
		FGraphNodeCreator<UVoxelGraphMacroParameterOutputNode> NodeCreator(*ParentGraph);
		UVoxelGraphMacroParameterOutputNode* Node = NodeCreator.CreateUserInvokedNode(bSelectNewNode);
		Node->Guid = Guid;
		Node->NodePosX = Location.X;
		Node->NodePosY = Location.Y;
		NodeCreator.Finalize();

		NewNode = Node;
	}
	else if (ParameterType == EVoxelGraphParameterType::LocalVariable)
	{
		if (bLocalVariable_IsDeclaration)
		{
			FGraphNodeCreator<UVoxelGraphLocalVariableDeclarationNode> NodeCreator(*ParentGraph);
			UVoxelGraphLocalVariableDeclarationNode* Node = NodeCreator.CreateUserInvokedNode(bSelectNewNode);
			Node->Guid = Guid;
			Node->NodePosX = Location.X;
			Node->NodePosY = Location.Y;
			NodeCreator.Finalize();

			NewNode = Node;
		}
		else
		{
			FGraphNodeCreator<UVoxelGraphLocalVariableUsageNode> NodeCreator(*ParentGraph);
			UVoxelGraphLocalVariableUsageNode* Node = NodeCreator.CreateUserInvokedNode(bSelectNewNode);
			Node->Guid = Guid;
			Node->NodePosX = Location.X;
			Node->NodePosY = Location.Y;
			NodeCreator.Finalize();

			NewNode = Node;
		}
	}
	else
	{
		ensure(false);
		return nullptr;
	}

	if (FromPin)
	{
		NewNode->AutowireNewNode(FromPin);
	}

	return NewNode;
}

void FVoxelGraphSchemaAction_NewParameterUsage::GetIcon(FSlateIcon& Icon, FLinearColor& Color)
{
	Icon = FVoxelGraphVisuals::GetPinIcon(PinType);
	Color = FVoxelGraphVisuals::GetPinColor(PinType);
}

UEdGraphNode* FVoxelGraphSchemaAction_NewParameter::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	const FVoxelTransaction Transaction(ParentGraph, "Create new " + UEnum::GetDisplayValueAsText(ParameterType).ToString());

	const TSharedPtr<FVoxelGraphToolkit> Toolkit = UVoxelGraphSchema::GetToolkit(ParentGraph);
	if (!ensure(Toolkit))
	{
		return nullptr;
	}

	UVoxelGraph* Graph = Toolkit->Asset->FindGraph(ParentGraph);
	if (!ensure(Graph))
	{
		return nullptr;
	}

	FVoxelGraphParameter NewParameter;
	NewParameter.Guid = FGuid::NewGuid();
	NewParameter.ParameterType = ParameterType;
	NewParameter.Category = TargetCategory;

	if (FromPin)
	{
		const FString PinName = FromPin->GetDisplayName().ToString();
		if (!PinName.TrimStartAndEnd().IsEmpty())
		{
			NewParameter.Name = *PinName;
		}
		NewParameter.Type = FromPin->PinType;

		if (NewParameter.Type.HasPinDefaultValue())
		{
			NewParameter.DefaultValue = FVoxelPinValue::MakeFromPinDefaultValue(*FromPin);
		}

		if (ParameterType == EVoxelGraphParameterType::Parameter &&
			!NewParameter.Type.IsBufferArray())
		{
			NewParameter.Type = NewParameter.Type.GetInnerType();
		}
	}
	else
	{
		NewParameter.Type = FVoxelPinType::Make<float>();
		NewParameter.Fixup(nullptr);
	}

	if (!ParameterName.IsNone())
	{
		NewParameter.Name = ParameterName;
	}

	UVoxelGraphNode* ResultNode;
	{
		const FVoxelTransaction GraphTransaction(Graph);

		Graph->Parameters.Add(NewParameter);

		if (ParameterType == EVoxelGraphParameterType::Parameter)
		{
			FGraphNodeCreator<UVoxelGraphParameterNode> NodeCreator(*ParentGraph);
			UVoxelGraphParameterNode* Node = NodeCreator.CreateUserInvokedNode(bSelectNewNode);
			Node->Guid = NewParameter.Guid;
			Node->NodePosX = Location.X;
			Node->NodePosY = Location.Y;
			NodeCreator.Finalize();

			ResultNode = Node;
		}
		else if (ParameterType == EVoxelGraphParameterType::Input)
		{
			FGraphNodeCreator<UVoxelGraphMacroParameterInputNode> NodeCreator(*ParentGraph);
			UVoxelGraphMacroParameterInputNode* Node = NodeCreator.CreateUserInvokedNode(bSelectNewNode);
			Node->Guid = NewParameter.Guid;
			Node->NodePosX = Location.X;
			Node->NodePosY = Location.Y;
			NodeCreator.Finalize();

			ResultNode = Node;
		}
		else if (ParameterType == EVoxelGraphParameterType::Output)
		{
			FGraphNodeCreator<UVoxelGraphMacroParameterOutputNode> NodeCreator(*ParentGraph);
			UVoxelGraphMacroParameterOutputNode* Node = NodeCreator.CreateUserInvokedNode(bSelectNewNode);
			Node->Guid = NewParameter.Guid;
			Node->NodePosX = Location.X;
			Node->NodePosY = Location.Y;
			NodeCreator.Finalize();

			ResultNode = Node;
		}
		else if (ParameterType == EVoxelGraphParameterType::LocalVariable)
		{
			FGraphNodeCreator<UVoxelGraphLocalVariableDeclarationNode> NodeCreator(*ParentGraph);
			UVoxelGraphLocalVariableDeclarationNode* Node = NodeCreator.CreateUserInvokedNode(bSelectNewNode);
			Node->Guid = NewParameter.Guid;
			Node->NodePosX = Location.X;
			Node->NodePosY = Location.Y;
			NodeCreator.Finalize();

			// Local Variables don't use DefaultValue, just apply it to the pin
			if (NewParameter.Type.HasPinDefaultValue())
			{
				UEdGraphPin& InputPin = *Node->GetInputPin(0);
				NewParameter.DefaultValue.ApplyToPinDefaultValue(InputPin);
				NewParameter.DefaultValue = {};
				NewParameter.Fixup(nullptr);
			}

			ResultNode = Node;
		}
		else
		{
			ensure(false);
			return nullptr;
		}

		if (FromPin)
		{
			if (FromPin->GetOwningNode()->GetGraph() == ParentGraph)
			{
				ResultNode->AutowireNewNode(FromPin);
			}

			switch (ParameterType)
			{
			default: check(false);
			case EVoxelGraphParameterType::Parameter: break;
			case EVoxelGraphParameterType::Output: break;
			case EVoxelGraphParameterType::Input: break;
			case EVoxelGraphParameterType::LocalVariable:
			{
				if (ResultNode->GetInputPins().Num() > 0)
				{
					UEdGraphPin* InputPin = ResultNode->GetInputPin(0);
					InputPin->DefaultObject = FromPin->DefaultObject;
					InputPin->DefaultValue = FromPin->DefaultValue;
					InputPin->DefaultTextValue = FromPin->DefaultTextValue;
				}
			}
			break;
			}
		}
	}

	Toolkit->SelectParameter(Graph, NewParameter.Guid, true, true);

	return ResultNode;
}

UEdGraphNode* FVoxelGraphSchemaAction_NewInlineMacro::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	const TSharedPtr<FVoxelGraphToolkit> Toolkit = UVoxelGraphSchema::GetToolkit(ParentGraph);
	if (!ensure(Toolkit))
	{
		return nullptr;
	}

	FName NewMacroName = "NewMacro";

	TSet<FName> UsedNames;
	for (const UVoxelGraph* InlineMacro : Toolkit->Asset->InlineMacros)
	{
		UsedNames.Add(*InlineMacro->GetGraphName());
	}

	while (UsedNames.Contains(NewMacroName))
	{
		NewMacroName.SetNumber(NewMacroName.GetNumber() + 1);
	}

	{
		const FVoxelTransaction Transaction(ParentGraph, "Create new inline macro");

		EObjectFlags Flags = RF_Transactional;
		if (Toolkit->bIsMacroLibrary)
		{
			Flags |= RF_Public;
		}

		NewMacro = NewObject<UVoxelGraph>(Toolkit->Asset, NAME_None, Flags);
		NewMacro->SetGraphName(NewMacroName.ToString());
		NewMacro->Category = TargetCategory;

		Toolkit->FixupGraph(NewMacro);
		Toolkit->Asset->InlineMacros.Add(NewMacro);
	}

	Toolkit->UpdateDetailsView(NewMacro);
	if (bOpenNewGraph)
	{
		Toolkit->OpenGraphAndBringToFront(NewMacro->MainEdGraph, false);
	}
	Toolkit->SelectMember(NewMacro, true, true);

	return nullptr;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

UEdGraphNode* FVoxelGraphSchemaAction_NewStructNode::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	const FVoxelTransaction Transaction(ParentGraph, "New graph node");

	FGraphNodeCreator<UVoxelGraphStructNode> NodeCreator(*ParentGraph);
	UVoxelGraphStructNode* StructNode = NodeCreator.CreateUserInvokedNode(bSelectNewNode);
	StructNode->Struct = *Node;
	StructNode->NodePosX = Location.X;
	StructNode->NodePosY = Location.Y;
	NodeCreator.Finalize();

	if (!FromPin)
	{
		return StructNode;
	}

	const FVoxelPinType PinType(FromPin->PinType);
	StructNode->AutowireNewNode(FromPin);

	UEdGraphPin* LinkedPin = nullptr;
	for (UEdGraphPin* Pin : FromPin->LinkedTo)
	{
		if (Pin->GetOwningNode() == StructNode)
		{
			LinkedPin = Pin;
			break;
		}
	}

	if (!LinkedPin)
	{
		return StructNode;
	}

	FVoxelPinTypeSet PromotionTypes;
	if (!ensure(LinkedPin) ||
		!StructNode->CanPromotePin(*LinkedPin, PromotionTypes))
	{
		return StructNode;
	}

	if (!PromotionTypes.Contains(PinType))
	{
		return StructNode;
	}

	StructNode->PromotePin(*LinkedPin, PinType);

	return StructNode;
}

void FVoxelGraphSchemaAction_NewStructNode::GetIcon(FSlateIcon& Icon, FLinearColor& Color)
{
	if (!ensure(Node))
	{
		return;
	}

	static FSlateIcon StructIcon("EditorStyle", "Kismet.AllClasses.FunctionIcon");

	const FString NodeIcon = Node->GetMetadataContainer().GetMetaData(STATIC_FNAME("NodeIcon"));
	const FString NodeIconColor = Node->GetMetadataContainer().GetMetaData(STATIC_FNAME("NodeIconColor"));

	if (!NodeIcon.IsEmpty())
	{
		Icon = FVoxelGraphVisuals::GetNodeIcon(NodeIcon);
	}
	else
	{
		Icon = StructIcon;
	}

	if (!NodeIconColor.IsEmpty())
	{
		Color = FVoxelGraphVisuals::GetNodeColor(NodeIconColor);
	}
	else
	{
		Color = GetDefault<UGraphEditorSettings>()->FunctionCallNodeTitleColor;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

UEdGraphNode* FVoxelGraphSchemaAction_NewPromotableStructNode::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	const FVoxelTransaction Transaction(ParentGraph, "New graph node");

	FGraphNodeCreator<UVoxelGraphStructNode> NodeCreator(*ParentGraph);
	UVoxelGraphStructNode* StructNode = NodeCreator.CreateUserInvokedNode(bSelectNewNode);
	StructNode->Struct = *Node;
	StructNode->NodePosX = Location.X;
	StructNode->NodePosY = Location.Y;
	NodeCreator.Finalize();

	if (FromPin)
	{
		for (UEdGraphPin* InputPin : StructNode->GetInputPins())
		{
			if (PinTypes.Num() == 0)
			{
				break;
			}

			StructNode->PromotePin(*InputPin, PinTypes.Pop());
		}

		StructNode->AutowireNewNode(FromPin);
	}

	return StructNode;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

UEdGraphNode* FVoxelGraphSchemaAction_NewKnotNode::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	const FVoxelTransaction Transaction(ParentGraph, "New reroute node");

	FGraphNodeCreator<UVoxelGraphKnotNode> NodeCreator(*ParentGraph);
	UVoxelGraphKnotNode* Node = NodeCreator.CreateUserInvokedNode(bSelectNewNode);
	Node->NodePosX = Location.X;
	Node->NodePosY = Location.Y;
	NodeCreator.Finalize();

	if (FromPin)
	{
		Node->AutowireNewNode(FromPin);
	}

	Node->PropagatePinType();

	return Node;
}

void FVoxelGraphSchemaAction_NewKnotNode::GetIcon(FSlateIcon& Icon, FLinearColor& Color)
{
	static const FSlateIcon KnotIcon = FSlateIcon("EditorStyle", "GraphEditor.Default_16x");
	Icon = KnotIcon;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedPtr<FEdGraphSchemaAction> UVoxelGraphSchema::FindCastAction(const FEdGraphPinType& From, const FEdGraphPinType& To) const
{
	const FVoxelNode* CastNode = FVoxelNodeLibrary::FindCastNode(From, To);
	if (!CastNode)
	{
		return nullptr;
	}

	const TSharedRef<FVoxelGraphSchemaAction_NewStructNode> Action = MakeVoxelShared<FVoxelGraphSchemaAction_NewStructNode>();
	Action->Node = CastNode;
	return Action;
}

TOptional<FPinConnectionResponse> UVoxelGraphSchema::GetCanCreateConnectionOverride(const UEdGraphPin* PinA, const UEdGraphPin* PinB) const
{
	const UEdGraphPin* InputPin = nullptr;
	const UEdGraphPin* OutputPin = nullptr;
	if (!ensure(CategorizePinsByDirection(PinA, PinB, InputPin, OutputPin)))
	{
		return {};
	}

	if (FVoxelPinType(OutputPin->PinType).CanBeCastedTo_Schema(InputPin->PinType))
	{
		// Can connect directly
		return {};
	}

	const auto TryConnect = [this](const UEdGraphPin& PinToPromote, const UEdGraphPin& OtherPin)
	{
		FString AdditionalText;
		FVoxelPinType Type;
		if (!TryGetPromotionType(PinToPromote, OtherPin.PinType, Type, AdditionalText))
		{
			return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, "");
		}

		if (!AdditionalText.IsEmpty())
		{
			AdditionalText = "\n" + AdditionalText;
		}

		return FPinConnectionResponse(CONNECT_RESPONSE_MAKE_WITH_PROMOTION,
			FString::Printf(TEXT("Convert pin %s from %s to %s%s"),
				*PinToPromote.GetDisplayName().ToString(),
				*FVoxelPinType(PinToPromote.PinType).ToString(),
				*Type.ToString(),
				*AdditionalText));
	};

	{
		const FPinConnectionResponse Response = TryConnect(*PinB, *PinA);
		if (Response.Response != CONNECT_RESPONSE_DISALLOW)
		{
			return Response;
		}
	}

	if (CanCreateAutomaticConversionNode(InputPin, OutputPin))
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_MAKE_WITH_CONVERSION_NODE, FString::Printf(TEXT("Convert %s to %s"),
			*FVoxelPinType(OutputPin->PinType).ToString(),
			*FVoxelPinType(InputPin->PinType).ToString()));
	}

	{
		const FPinConnectionResponse Response = TryConnect(*PinA, *PinB);
		if (Response.Response != CONNECT_RESPONSE_DISALLOW)
		{
			return Response;
		}
	}

	return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, FString::Printf(TEXT("Cannot convert %s to %s"),
		*FVoxelPinType(OutputPin->PinType).ToString(),
		*FVoxelPinType(InputPin->PinType).ToString()));
}

bool UVoxelGraphSchema::CreatePromotedConnectionSafe(UEdGraphPin*& PinA, UEdGraphPin*& PinB) const
{
	const auto TryPromote = [&](UEdGraphPin& PinToPromote, const UEdGraphPin& OtherPin)
	{
		FString AdditionalText;
		FVoxelPinType Type;
		if (!TryGetPromotionType(PinToPromote, OtherPin.PinType, Type, AdditionalText))
		{
			return false;
		}

		UVoxelGraphNode* Node = CastChecked<UVoxelGraphNode>(PinToPromote.GetOwningNode());

		const FName PinAName = PinA->GetFName();
		const FName PinBName = PinB->GetFName();
		const UVoxelGraphNode* NodeA = CastChecked<UVoxelGraphNode>(PinA->GetOwningNode());
		const UVoxelGraphNode* NodeB = CastChecked<UVoxelGraphNode>(PinB->GetOwningNode());
		{
			// Tricky: PromotePin might reconstruct the node, invalidating pin pointers
			Node->PromotePin(PinToPromote, Type);
		}
		PinA = NodeA->FindPin(PinAName);
		PinB = NodeB->FindPin(PinBName);

		return true;
	};

	if (!TryPromote(*PinB, *PinA) &&
		!TryPromote(*PinA, *PinB))
	{
		return false;
	}

	if (!ensure(PinA) ||
		!ensure(PinB) ||
		!ensure(CanCreateConnection(PinA, PinB).Response != CONNECT_RESPONSE_MAKE_WITH_PROMOTION) ||
		!ensure(TryCreateConnection(PinA, PinB)))
	{
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void UVoxelGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	VOXEL_FUNCTION_COUNTER();

	constexpr int32 SectionId_Macros = 0;
	constexpr int32 SectionId_Operators = 0;
	constexpr int32 SectionId_StructNodes = 0;
	constexpr int32 SectionId_ShortList = 1;
	constexpr int32 SectionId_InlineMacros = 2;
	constexpr int32 SectionId_Parameters = 2;

	const TSharedPtr<FVoxelGraphToolkit> Toolkit = GetToolkit(ContextMenuBuilder.CurrentGraph);
	if (!ensure(Toolkit))
	{
		return;
	}

	Super::GetGraphContextActions(ContextMenuBuilder);

	ContextMenuBuilder.AddAction(MakeVoxelShared<FVoxelGraphSchemaAction_NewKnotNode>(
		FText(),
		INVTEXT("Add reroute node"),
		INVTEXT("Create a reroute node"),
		0));

	FARFilter Filter;
	Filter.ClassPaths.Add(UVoxelGraph::StaticClass()->GetClassPathName());
	Filter.ClassPaths.Add(UVoxelGraphInstance::StaticClass()->GetClassPathName());
	Filter.ClassPaths.Add(UVoxelMacroLibrary::StaticClass()->GetClassPathName());

	const IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();

	TArray<FAssetData> AssetDatas;
	AssetRegistry.GetAssets(Filter, AssetDatas);

	TArray<UVoxelGraphInterface*> GraphInterfaces;
	TArray<UVoxelMacroLibrary*> MacroLibraries;
	for (const FAssetData& AssetData : AssetDatas)
	{
		UObject* Asset = AssetData.GetAsset();
		if (!ensure(Asset))
		{
			continue;
		}

		if (Asset->IsA<UVoxelMacroLibrary>())
		{
			MacroLibraries.Add(CastChecked<UVoxelMacroLibrary>(Asset));
		}
		else if (ensure(Asset->IsA<UVoxelGraphInterface>()))
		{
			GraphInterfaces.Add(CastChecked<UVoxelGraphInterface>(Asset));
		}
	}

	const auto AddMacro = [&](UVoxelGraphInterface* GraphInterface, const FString& Category, int32 SectionId)
	{
		const UVoxelGraph* Graph = GraphInterface->GetGraph();
		if (!Graph)
		{
			return;
		}

		if (ContextMenuBuilder.FromPin)
		{
			bool bHasMatch = false;

			for (const FVoxelGraphParameter& Parameter : Graph->Parameters)
			{
				if (ContextMenuBuilder.FromPin->PinType == Parameter.Type &&
					(ContextMenuBuilder.FromPin->Direction == EGPD_Output) == (Parameter.ParameterType == EVoxelGraphParameterType::Input))
				{
					bHasMatch = true;
					break;
				}
			}

			if (!bHasMatch)
			{
				return;
			}
		}

		const TSharedRef<FVoxelGraphSchemaAction_NewMacroNode> Action = MakeVoxelShared<FVoxelGraphSchemaAction_NewMacroNode>(
			FText::FromString(Category),
			FText::FromString(GraphInterface->GetGraphName()),
			FText::FromString(Graph->Tooltip),
			SectionId);

		Action->Graph = GraphInterface;

		ContextMenuBuilder.AddAction(Action);
	};

	for (UVoxelMacroLibrary* MacroLibrary : MacroLibraries)
	{
		for (UVoxelGraph* Graph : MacroLibrary->Graph->InlineMacros)
		{
			if (!Graph->bExposeToLibrary)
			{
				continue;
			}

			AddMacro(Graph, Graph->Category, SectionId_Macros);
		}
	}

	for (UVoxelGraphInterface* GraphInterface : GraphInterfaces)
	{
		const UVoxelGraph* Graph = GraphInterface->GetGraph();
		if (!Graph)
		{
			continue;
		}

		AddMacro(GraphInterface, Graph->Category, SectionId_Macros);
	}

	for (UVoxelGraph* Graph : Toolkit->Asset->InlineMacros)
	{
		AddMacro(Graph, "Macros", SectionId_InlineMacros);
	}

	{
		const TSharedRef<FVoxelGraphSchemaAction_NewMacroNode> Action = MakeVoxelShared<FVoxelGraphSchemaAction_NewMacroNode>(
			INVTEXT("Misc"),
			INVTEXT("Call Graph"),
			INVTEXT(""),
			SectionId_Macros);

		ContextMenuBuilder.AddAction(Action);
	}

	TArray<TSharedPtr<FEdGraphSchemaAction>> ShortListActions_Forced;
	TArray<TSharedPtr<FEdGraphSchemaAction>> ShortListActions_ExactMatches;
	TArray<TSharedPtr<FEdGraphSchemaAction>> ShortListActions_Parameters;
	ON_SCOPE_EXIT
	{
		for (const TSharedPtr<FEdGraphSchemaAction>& Action : ShortListActions_Forced)
		{
			ContextMenuBuilder.AddAction(Action);
		}

		if (ShortListActions_ExactMatches.Num() < 10)
		{
			for (const TSharedPtr<FEdGraphSchemaAction>& Action : ShortListActions_ExactMatches)
			{
				ContextMenuBuilder.AddAction(Action);
			}
		}

		for (const TSharedPtr<FEdGraphSchemaAction>& Action : ShortListActions_Parameters)
		{
			ContextMenuBuilder.AddAction(Action);
		}
	};

	for (const FVoxelNode* Node : FVoxelNodeLibrary::GetNodes())
	{
		UObject* Outer = Toolkit->Asset->GetOuter();
		if (!ensure(Outer))
		{
			continue;
		}

		bool bIsExactMatch = true;
		FVoxelPinTypeSet PromotionTypes;

		const bool bHasPinMatch = !ContextMenuBuilder.FromPin || INLINE_LAMBDA
		{
			const UEdGraphPin& FromPin = *ContextMenuBuilder.FromPin;

			FVoxelPinTypeSet FromPinPromotionTypes;
			if (const UVoxelGraphNode* ThisNode = Cast<UVoxelGraphNode>(FromPin.GetOwningNode()))
			{
				if (!ThisNode->CanPromotePin(FromPin, FromPinPromotionTypes))
				{
					FromPinPromotionTypes.Add(FromPin.PinType);
				}
			}

			for (const FVoxelPin& ToPin : Node->GetPins())
			{
				if (FromPin.Direction == (ToPin.bIsInput ? EGPD_Input : EGPD_Output))
				{
					continue;
				}

				FVoxelPinTypeSet ToPinPromotionTypes;
				if (ToPin.IsPromotable())
				{
					ToPinPromotionTypes = Node->GetPromotionTypes(ToPin);
				}
				else
				{
					ToPinPromotionTypes.Add(ToPin.GetType());
				}

				if (ToPinPromotionTypes.GetSetType() == EVoxelPinTypeSetType::All)
				{
					PromotionTypes = ToPinPromotionTypes;
					bIsExactMatch = false;
					return true;
				}

				for (const FVoxelPinType& Type : ToPinPromotionTypes.GetTypes())
				{
					for (const FVoxelPinType& FromType : FromPinPromotionTypes.GetTypes())
					{
						if (FromPin.Direction == EGPD_Input && Type.CanBeCastedTo_Schema(FromType))
						{
							PromotionTypes = ToPinPromotionTypes;
							return true;
						}
						if (FromPin.Direction == EGPD_Output && FromType.CanBeCastedTo_Schema(Type))
						{
							PromotionTypes = ToPinPromotionTypes;
							return true;
						}
					}
				}
			}

			return false;
		};

		if (!bHasPinMatch)
		{
			continue;
		}

		FString Keywords;
		Node->GetMetadataContainer().GetStringMetaDataHierarchical(STATIC_FNAME("Keywords"), &Keywords);
		Keywords += Node->GetMetadataContainer().GetMetaData(STATIC_FNAME("CompactNodeTitle"));

		if (Node->GetMetadataContainer().HasMetaDataHierarchical(STATIC_FNAME("NativeMakeFunc")))
		{
			Keywords += "construct build";
		}
		if (Node->GetMetadataContainer().HasMetaDataHierarchical(STATIC_FNAME("Autocast")))
		{
			Keywords += "cast convert";
		}

		FString DisplayName = Node->GetDisplayName();
		Keywords += DisplayName;
		DisplayName.ReplaceInline(TEXT("\n"), TEXT(" "));

		if (ContextMenuBuilder.FromPin &&
			Node->GetMetadataContainer().HasMetaData(STATIC_FNAME("Operator")))
		{
			const FString Operator = Node->GetMetadataContainer().GetMetaData(STATIC_FNAME("Operator"));

			TMap<FVoxelPinType, TSet<FVoxelPinType>> Permutations = CollectOperatorPermutations(*Node, *ContextMenuBuilder.FromPin, PromotionTypes);
			for (const auto& PermutationIt : Permutations)
			{
				FVoxelPinType InnerType = PermutationIt.Key.GetInnerType();
				for (const FVoxelPinType& SecondType : PermutationIt.Value)
				{
					FVoxelPinType SecondInnerType = SecondType.GetInnerType();

					const TSharedRef<FVoxelGraphSchemaAction_NewPromotableStructNode> Action = MakeVoxelShared<FVoxelGraphSchemaAction_NewPromotableStructNode>(
						FText::FromString(Node->GetCategory()),
						FText::FromString(InnerType.ToString() + " " + Operator + " " + SecondInnerType.ToString()),
						FText::FromString(Node->GetTooltip()),
						SectionId_Operators,
						FText::FromString(Keywords + " " + InnerType.ToString() + " " + SecondInnerType.ToString()));

					Action->PinTypes = { SecondType, PermutationIt.Key };
					Action->Node = Node;

					ContextMenuBuilder.AddAction(Action);
				}
			}
		}
		else
		{
			const TSharedRef<FVoxelGraphSchemaAction_NewStructNode> Action = MakeVoxelShared<FVoxelGraphSchemaAction_NewStructNode>(
				FText::FromString(Node->GetCategory()),
				FText::FromString(DisplayName),
				FText::FromString(Node->GetTooltip()),
				SectionId_StructNodes,
				FText::FromString(Keywords));

			Action->Node = Node;

			ContextMenuBuilder.AddAction(Action);

			const TSharedRef<FVoxelGraphSchemaAction_NewStructNode> ShortListAction = MakeVoxelShared<FVoxelGraphSchemaAction_NewStructNode>(
				FText(),
				FText::FromString(DisplayName),
				FText::FromString(Node->GetTooltip()),
				SectionId_ShortList,
				FText::FromString(Keywords));

			ShortListAction->Node = Node;

			if (Node->GetMetadataContainer().HasMetaDataHierarchical(STATIC_FNAME("ShowInRootShortList")) ||
				(ContextMenuBuilder.FromPin && Node->GetMetadataContainer().HasMetaDataHierarchical(STATIC_FNAME("ShowInShortList"))))
			{
				ShortListActions_Forced.Add(ShortListAction);
			}
			else if (ContextMenuBuilder.FromPin && bIsExactMatch)
			{
				ShortListActions_ExactMatches.Add(ShortListAction);
			}
		}
	}

	const auto CreateNewParameterUsageAction = [&](const FVoxelGraphParameter& Parameter, const bool bDeclaration)
	{
		const FString Prefix = bDeclaration ? "Set" : "Get";

		const FString TypeName = UEnum::GetDisplayValueAsText(Parameter.ParameterType).ToString();

		const FText Tooltip = FText::FromString(Prefix + " " + Parameter.Name.ToString() + " (" + TypeName + ")");

		const TSharedRef<FVoxelGraphSchemaAction_NewParameterUsage> Action = MakeVoxelShared<FVoxelGraphSchemaAction_NewParameterUsage>(
			FText::FromString(TypeName),
			FText::FromString(Prefix + " " + Parameter.Name.ToString()),
			Tooltip,
			SectionId_Parameters);

		Action->Guid = Parameter.Guid;
		Action->ParameterType = Parameter.ParameterType;
		Action->bLocalVariable_IsDeclaration = bDeclaration;
		Action->PinType = Parameter.Type.GetEdGraphPinType();

		if (ContextMenuBuilder.FromPin)
		{
			const TSharedRef<FVoxelGraphSchemaAction_NewParameterUsage> ShortListAction = MakeVoxelShared<FVoxelGraphSchemaAction_NewParameterUsage>(
				FText(),
				FText::FromString(Prefix + " " + Parameter.Name.ToString()),
				Tooltip,
				SectionId_ShortList);

			ShortListAction->Guid = Parameter.Guid;
			ShortListAction->ParameterType = Parameter.ParameterType;
			ShortListAction->bLocalVariable_IsDeclaration = bDeclaration;
			ShortListAction->PinType = Parameter.Type.GetEdGraphPinType();

			ShortListActions_Parameters.Add(ShortListAction);
		}

		return Action;
	};

	const auto CheckUsage = [](const FGraphContextMenuBuilder& MenuBuilder, const FVoxelGraphParameter& Parameter, const EEdGraphPinDirection Direction)
	{
		if (!MenuBuilder.FromPin)
		{
			return true;
		}

		if (MenuBuilder.FromPin->Direction != Direction)
		{
			return false;
		}

		if (Parameter.Type == MenuBuilder.FromPin->PinType)
		{
			return true;
		}

		return
			Direction == EGPD_Input &&
			Parameter.Type.GetBufferType() == MenuBuilder.FromPin->PinType;
	};

	UVoxelGraph* VoxelGraph = Toolkit->Asset->FindGraph(ContextMenuBuilder.CurrentGraph);
	if (!VoxelGraph)
	{
		return;
	}

	for (const FVoxelGraphParameter& Parameter : VoxelGraph->Parameters)
	{
		if (Parameter.ParameterType == EVoxelGraphParameterType::LocalVariable)
		{
			if (CheckUsage(ContextMenuBuilder, Parameter, EGPD_Input))
			{
				ContextMenuBuilder.AddAction(CreateNewParameterUsageAction(Parameter, false));
			}

			if (CheckUsage(ContextMenuBuilder, Parameter, EGPD_Output))
			{
				ContextMenuBuilder.AddAction(CreateNewParameterUsageAction(Parameter, true));
			}
			continue;
		}

		if (Parameter.ParameterType == EVoxelGraphParameterType::Parameter)
		{
			if (CheckUsage(ContextMenuBuilder, Parameter, EGPD_Input))
			{
				ContextMenuBuilder.AddAction(CreateNewParameterUsageAction(Parameter, false));
			}
		}
	}

	const auto CreateNewParameterAction = [&](const EVoxelGraphParameterType Type, bool const bPromotion)
	{
		const FString Prefix = bPromotion ? "Promote to" : "Create new";

		const FString TypeName = UEnum::GetDisplayValueAsText(Type).ToString();

		const TSharedRef<FVoxelGraphSchemaAction_NewParameter> Action = MakeVoxelShared<FVoxelGraphSchemaAction_NewParameter>(
			INVTEXT(""),
			FText::FromString(Prefix + " " + TypeName),
			FText::FromString(Prefix + " " + TypeName),
			SectionId_Parameters);
		Action->ParameterType = Type;
		return Action;
	};

	const bool bIsPromotion = ContextMenuBuilder.FromPin ? true : false;
	if (!ContextMenuBuilder.FromPin ||
		ContextMenuBuilder.FromPin->Direction == EGPD_Input)
	{
		ContextMenuBuilder.AddAction(CreateNewParameterAction(EVoxelGraphParameterType::Parameter, bIsPromotion));
	}
	if (!ContextMenuBuilder.FromPin ||
		ContextMenuBuilder.FromPin->Direction == EGPD_Input)
	{
		ContextMenuBuilder.AddAction(CreateNewParameterAction(EVoxelGraphParameterType::Input, bIsPromotion));
	}
	if (!ContextMenuBuilder.FromPin ||
		ContextMenuBuilder.FromPin->Direction == EGPD_Output)
	{
		ContextMenuBuilder.AddAction(CreateNewParameterAction(EVoxelGraphParameterType::Output, bIsPromotion));
	}
	ContextMenuBuilder.AddAction(CreateNewParameterAction(EVoxelGraphParameterType::LocalVariable, bIsPromotion));
}

void UVoxelGraphSchema::GetContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	const TSharedPtr<FVoxelGraphToolkit> Toolkit = GetToolkit(Context->Graph);
	if (!ensure(Toolkit) ||
		Toolkit->bIsReadOnly)
	{
		return;
	}

	Super::GetContextMenuActions(Menu, Context);

	if (!Context->Pin &&
		Context->Graph)
	{
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("VoxelGraphNodeEdit");
			Section.AddMenuEntry(FGraphEditorCommands::Get().FindReferences);
		}

		const int32 NodesCount = GetNodeSelectionCount(Context->Graph);
		if (NodesCount > 0)
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("EdGraphSchemaOrganization");
			Section.AddMenuEntry(
				"CollapseToMacro",
				INVTEXT("Collapse to Macro"),
				INVTEXT("Collapse to Macro"),
				{},
				FUIAction(MakeLambdaDelegate([=]
				{
					FVoxelGraphSchemaAction_CollapseToMacro Action;
					if (const UVoxelGraphMacroNode* NewMacroNode = Cast<UVoxelGraphMacroNode>(Action.PerformAction(Cast<UEdGraph>(Context->Graph), nullptr, FVector2D::ZeroVector, true)))
					{
						Toolkit->SelectMember(NewMacroNode->GraphInterface, true, true);
					}
				}))
			);

			if (NodesCount == 1 &&
				Context->Node &&
				Context->Node->IsA<UVoxelGraphMacroNode>())
			{
				Section.AddMenuEntry(
					"ExpandMacro",
					INVTEXT("Expand Macro"),
					INVTEXT("Expand Macro"),
					{},
					FUIAction(MakeLambdaDelegate([=]
					{
						FVoxelGraphSchemaAction_ExpandMacro Action;
						Action.PerformAction(Cast<UEdGraph>(Context->Graph), nullptr, FVector2D::ZeroVector, true);
					}))
				);
			}
		}
	}

	if (UVoxelGraphNodeBase* Node = ConstCast(Cast<UVoxelGraphNodeBase>(Context->Node.Get())))
	{
		FToolMenuSection& DebugSection = Menu->FindOrAddSection("DebugSection");
		DebugSection.InitSection("DebugSection", INVTEXT("Debug"), FToolMenuInsert({}, EToolMenuInsertType::First));

		if (Node->bEnableDebug)
		{
			DebugSection.AddMenuEntry(
				"StopDebug",
				INVTEXT("Stop debugging"),
				INVTEXT("Stop debugging this node"),
				{},
				FUIAction(
					MakeLambdaDelegate([Node]
					{
						Node->bEnableDebug = false;
						OnGraphChanged(Node);
					})
				)
			);
		}
		else
		{
			DebugSection.AddMenuEntry(
				"StartDebug",
				INVTEXT("Debug node"),
				INVTEXT("Debug this node"),
				{},
				FUIAction(
					MakeLambdaDelegate([Node]
					{
						for (const TObjectPtr<UEdGraphNode>& OtherNode : Node->GetGraph()->Nodes)
						{
							if (UVoxelGraphNodeBase* OtherVoxelNode = Cast<UVoxelGraphNodeBase>(OtherNode.Get()))
							{
								if (OtherVoxelNode->bEnableDebug)
								{
									OtherVoxelNode->bEnableDebug = false;
								}
							}
						}

						Node->bEnableDebug = true;
						OnGraphChanged(Node);
					})
				)
			);
		}
	}

	if (UVoxelGraphNodeBase* Node = ConstCast(Cast<UVoxelGraphNodeBase>(Context->Node.Get())))
	{
		FToolMenuSection& PreviewSection = Menu->FindOrAddSection("PreviewSection");
		PreviewSection.InitSection("PreviewSection", INVTEXT("Preview"), FToolMenuInsert({}, EToolMenuInsertType::First));

		if (Node->bEnablePreview)
		{
			PreviewSection.AddMenuEntry(
				"StopPreview",
				INVTEXT("Stop previewing"),
				INVTEXT("Stop previewing this node"),
				{},
				FUIAction(
					MakeLambdaDelegate([Node]
					{
						const FVoxelTransaction Transaction(Node, "Stop previewing");
						Node->bEnablePreview = false;
					})
				)
			);
		}
		else
		{
			PreviewSection.AddMenuEntry(
				"StartPreview",
				INVTEXT("Preview node"),
				INVTEXT("Preview this node"),
				{},
				FUIAction(
					MakeLambdaDelegate([Node]
					{
						const FVoxelTransaction Transaction(Node, "Start previewing");

						for (const TObjectPtr<UEdGraphNode>& OtherNode : Node->GetGraph()->Nodes)
						{
							if (UVoxelGraphNodeBase* OtherVoxelNode = Cast<UVoxelGraphNodeBase>(OtherNode.Get()))
							{
								if (OtherVoxelNode->bEnablePreview)
								{
									const FVoxelTransaction OtherTransaction(OtherVoxelNode, "Stop previewing");
									OtherVoxelNode->bEnablePreview = false;
								}
							}
						}

						Node->bEnablePreview = true;
					})
				)
			);
		}
	}

	if (!Context->Pin)
	{
		FToolMenuSection& Section = Menu->FindOrAddSection("VoxelGraphNodeEdit");
		Section.AddMenuEntry(FGraphEditorCommands::Get().FindReferences);
	}

	const UEdGraphPin* Pin = Context->Pin;
	if (!Pin ||
		!ensure(Pin->GetOwningNode()) ||
		!ensure(Pin->GetOwningNode()->GetGraph()) ||
		Pin->bOrphanedPin ||
		Pin->bNotConnectable)
	{
		return;
	}

	FToolMenuSection& Section = Menu->FindOrAddSection("EdGraphSchemaPinActions");

	if (UVoxelGraphNode* Node = Cast<UVoxelGraphNode>(Pin->GetOwningNode()))
	{
		FVoxelPinTypeSet Types;
		if (Node->CanPromotePin(*Pin, Types))
		{
			const FVoxelPinType CurrentPinType = Pin->PinType;

			TArray<FVoxelPinType> TypesList = Types.GetTypes().Array();
			if (TypesList.Num() > 2 ||
				(TypesList.Num() == 2 && TypesList[0].GetInnerType() != TypesList[1].GetInnerType()))
			{
				Section.AddSubMenu(
					"PromotePin",
					INVTEXT("Convert pin"),
					INVTEXT("Convert this pin"),
					MakeLambdaDelegate([=](const FToolMenuContext&) -> TSharedRef<SWidget>
					{
						return
							SNew(SVoxelPinTypeSelector)
							.AllowedTypes(Types)
							.OnTypeChanged_Lambda([=](FVoxelPinType NewType)
							{
								const FVoxelTransaction Transaction(Pin, "Convert pin");
								if (CurrentPinType.IsBuffer())
								{
									NewType = NewType.GetBufferType();
								}
								Node->PromotePin(ConstCast(*Pin), NewType);
							})
							.OnCloseMenu_Lambda([=]
							{
								FSlateApplication::Get().ClearAllUserFocus();
							});
					})
				);
			}

			if (CurrentPinType.IsBuffer() &&
				Types.Contains(CurrentPinType.GetInnerType()))
			{
				Section.AddMenuEntry(
					"ConvertToUniform",
					INVTEXT("Convert pin to Uniform"),
					INVTEXT("Convert pin to Uniform"),
					{},
					FUIAction(MakeLambdaDelegate([=]
					{
						const FVoxelTransaction Transaction(Pin, "Convert pin to Uniform");
						Node->PromotePin(ConstCast(*Pin), CurrentPinType.GetInnerType());
					}))
				);
			}
			else if (
				!CurrentPinType.IsBuffer() &&
				Types.Contains(CurrentPinType.GetBufferType()))
			{
				Section.AddMenuEntry(
					"ConvertToBuffer",
					INVTEXT("Convert pin to Buffer"),
					INVTEXT("Convert pin to Buffer"),
					{},
					FUIAction(MakeLambdaDelegate([=]
					{
						const FVoxelTransaction Transaction(Pin, "Convert pin to Buffer");
						Node->PromotePin(ConstCast(*Pin), CurrentPinType.GetBufferType());
					}))
				);
			}
		}
	}

	if (!Pin->bNotConnectable)
	{
		if (Pin->Direction == EGPD_Input)
		{
			Section.AddMenuEntry(
				"PromoteToParameter",
				INVTEXT("Promote to Parameter"),
				INVTEXT("Promote to Parameter"),
				{},
				FUIAction(FExecuteAction::CreateUObject(this, &UVoxelGraphSchema::PromoteToVariable, ConstCast(Pin), EVoxelGraphParameterType::Parameter))
			);
		}

		if (Pin->Direction == EGPD_Input)
		{
			Section.AddMenuEntry(
				"PromoteToInput",
				INVTEXT("Promote to Input"),
				INVTEXT("Promote to Input"),
				{},
				FUIAction(FExecuteAction::CreateUObject(this, &UVoxelGraphSchema::PromoteToVariable, ConstCast(Pin), EVoxelGraphParameterType::Input))
			);
		}
		else
		{
			Section.AddMenuEntry(
				"PromoteToParameter",
				INVTEXT("Promote to Output"),
				INVTEXT("Promote to Output"),
				{},
				FUIAction(FExecuteAction::CreateUObject(this, &UVoxelGraphSchema::PromoteToVariable, ConstCast(Pin), EVoxelGraphParameterType::Output))
			);
		}

		Section.AddMenuEntry(
			"PromoteToLocalVariable",
			INVTEXT("Promote to Local Variable"),
			INVTEXT("Promote to Local Variable"),
			{},
			FUIAction(FExecuteAction::CreateUObject(this, &UVoxelGraphSchema::PromoteToVariable, ConstCast(Pin), EVoxelGraphParameterType::LocalVariable))
		);

		if (Pin->Direction == EGPD_Input)
		{
			Section.AddMenuEntry(
				"MakeValue",
				INVTEXT("Make Value"),
				INVTEXT("Make Value"),
				{},
				FUIAction(FExecuteAction::CreateUObject(this, &UVoxelGraphSchema::PromoteToMakeValue, ConstCast(Pin)))
			);
		}
	}
}

FLinearColor UVoxelGraphSchema::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	return FVoxelGraphVisuals::GetPinColor(PinType);
}

void UVoxelGraphSchema::OnPinConnectionDoubleCicked(UEdGraphPin* PinA, UEdGraphPin* PinB, const FVector2D& GraphPosition) const
{
	const FVoxelTransaction Transaction(PinA, "Create Reroute Node");

	const FVector2D NodeSpacerSize(42.0f, 24.0f);
	const FVector2D KnotTopLeft = GraphPosition - (NodeSpacerSize * 0.5f);

	UEdGraph* ParentGraph = PinA->GetOwningNode()->GetGraph();

	UVoxelGraphKnotNode* NewKnot = CastChecked<UVoxelGraphKnotNode>(FVoxelGraphSchemaAction_NewKnotNode().PerformAction(
		ParentGraph,
		nullptr,
		KnotTopLeft,
		true));

	// Move the connections across (only notifying the knot, as the other two didn't really change)
	PinA->BreakLinkTo(PinB);

	PinA->MakeLinkTo(NewKnot->GetPin(PinB->Direction));
	PinB->MakeLinkTo(NewKnot->GetPin(PinA->Direction));

	NewKnot->PropagatePinType();
}

void UVoxelGraphSchema::GetAssetsGraphHoverMessage(const TArray<FAssetData>& Assets, const UEdGraph* HoverGraph, FString& OutTooltipText, bool& OutOkIcon) const
{
	for (const FAssetData& Asset : Assets)
	{
		if (!Asset.GetClass()->IsChildOf<UVoxelGraphInterface>())
		{
			OutOkIcon = false;
			return;
		}
	}

	OutOkIcon = true;
}

void UVoxelGraphSchema::DroppedAssetsOnGraph(const TArray<FAssetData>& Assets, const FVector2D& GraphPosition, UEdGraph* Graph) const
{
	for (const FAssetData& Asset : Assets)
	{
		if (!Asset.GetClass()->IsChildOf<UVoxelGraphInterface>())
		{
			continue;
		}

		FVoxelGraphSchemaAction_NewMacroNode Action;
		Action.Graph = CastChecked<UVoxelGraphInterface>(Asset.GetAsset());
		Action.PerformAction(Graph, nullptr, GraphPosition, true);
	}
}

TSharedPtr<FVoxelGraphToolkit> UVoxelGraphSchema::GetToolkit(const UEdGraph* Graph)
{
	return StaticCastSharedPtr<FVoxelGraphToolkit>(Super::GetToolkit(Graph));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool UVoxelGraphSchema::TryGetPromotionType(const UEdGraphPin& Pin, const FVoxelPinType& TargetType, FVoxelPinType& OutType, FString& OutAdditionalText) const
{
	OutType = {};

	const UVoxelGraphNode* Node = Cast<UVoxelGraphNode>(Pin.GetOwningNode());
	if (!ensure(Node))
	{
		return false;
	}

	FVoxelPinTypeSet Types;
	if (!Node->CanPromotePin(Pin, Types))
	{
		return false;
	}

	OutAdditionalText = Node->GetPinPromotionWarning(Pin, TargetType);

	const FVoxelPinType CurrentType(Pin.PinType);

	const auto TryType = [&](const FVoxelPinType& NewType)
	{
		if (!OutType.IsValid() &&
			Types.Contains(NewType))
		{
			OutType = NewType;
		}
	};

	if (Pin.Direction == EGPD_Input)
	{
		// We're an input pin, we can implicitly promote to buffer

		if (CurrentType.IsBuffer())
		{
			// If we're currently a buffer, try to preserve that
			TryType(TargetType.GetBufferType());
			TryType(TargetType);
		}
		else
		{
			// Otherwise try the raw type first
			TryType(TargetType);
			TryType(TargetType.GetBufferType());
		}
	}
	else
	{
		// We're an output pin, we can never implicitly promote
		TryType(TargetType);
	}

	return OutType.IsValid();
}

void UVoxelGraphSchema::PromoteToVariable(UEdGraphPin* Pin, const EVoxelGraphParameterType ParameterType) const
{
	if (!ensure(Pin))
	{
		return;
	}

	FVoxelGraphSchemaAction_NewParameter Action;
	Action.ParameterType = ParameterType;

	const UEdGraphNode* OwningNode = Pin->GetOwningNode();
	FVector2D Position;
	Position.X = Pin->Direction == EGPD_Input ? OwningNode->NodePosX - 200 : OwningNode->NodePosX + 400;
	Position.Y = OwningNode->NodePosY + 75;

	Action.PerformAction(OwningNode->GetGraph(), Pin, Position, true);
}

void UVoxelGraphSchema::PromoteToMakeValue(UEdGraphPin* Pin) const
{
	if (!ensure(Pin))
	{
		return;
	}

	const FVoxelNode* Node = FVoxelNodeLibrary::GetNodeInstance<FVoxelNode_MakeValue>();
	if (!ensure(Node))
	{
		return;
	}

	FVoxelGraphSchemaAction_NewStructNode Action;
	Action.Node = Node;

	const UEdGraphNode* OwningNode = Pin->GetOwningNode();
	FVector2D Position;
	Position.X = Pin->Direction == EGPD_Input ? OwningNode->NodePosX - 200 : OwningNode->NodePosX + 400;
	Position.Y = OwningNode->NodePosY + 75;

	Action.PerformAction(OwningNode->GetGraph(), Pin, Position, true);
}

TMap<FVoxelPinType, TSet<FVoxelPinType>> UVoxelGraphSchema::CollectOperatorPermutations(const FVoxelNode& Node, const UEdGraphPin& FromPin, const FVoxelPinTypeSet& PromotionTypes)
{
	const FVoxelPinType FromPinType = FVoxelPinType(FromPin.PinType);
	const bool bIsBuffer = FromPinType.IsBuffer();

	if (!PromotionTypes.Contains(FromPinType))
	{
		return {};
	}

	const bool bIsCommutativeOperator = Node.IsA<FVoxelTemplateNode_CommutativeAssociativeOperator>();

	TMap<FVoxelPinType, TSet<FVoxelPinType>> Result;
	if (FromPin.Direction == EGPD_Output)
	{
		for (const FVoxelPinType& Type : PromotionTypes.GetTypes())
		{
			if (Type.IsBuffer() != bIsBuffer)
			{
				continue;
			}

			Result.FindOrAdd(FromPinType, {}).Add(Type);
			if (!bIsCommutativeOperator)
			{
				Result.FindOrAdd(Type, {}).Add(FromPinType);
			}
		}
	}
	else
	{
		const int32 FromDimension = FVoxelTemplateNodeUtilities::GetDimension(FromPinType);

		for (const FVoxelPinType& Type : PromotionTypes.GetTypes())
		{
			if (Type.IsBuffer() != bIsBuffer ||
				FromDimension < FVoxelTemplateNodeUtilities::GetDimension(Type))
			{
				continue;
			}

			if (bIsCommutativeOperator)
			{
				Result.FindOrAdd(FromPinType, {}).Add(Type);
				continue;
			}

			for (const FVoxelPinType& SecondType : PromotionTypes.GetTypes())
			{
				if (SecondType.IsBuffer() != bIsBuffer ||
					FromDimension < FVoxelTemplateNodeUtilities::GetDimension(SecondType))
				{
					continue;
				}

				if (Type != FromPinType &&
					SecondType != FromPinType)
				{
					continue;
				}

				Result.FindOrAdd(Type, {}).Add(SecondType);
			}
		}
	}

	return Result;
}