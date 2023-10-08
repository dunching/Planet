// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelGraphEditorCompiler.h"
#include "VoxelGraph.h"
#include "VoxelEdGraph.h"
#include "VoxelNodeLibrary.h"
#include "VoxelMacroLibrary.h"
#include "VoxelParameterNode.h"
#include "VoxelGraphCompileScope.h"
#include "VoxelLocalVariableNodes.h"
#include "Nodes/VoxelGraphKnotNode.h"
#include "Nodes/VoxelGraphMacroNode.h"
#include "Nodes/VoxelGraphStructNode.h"
#include "Nodes/VoxelGraphParameterNode.h"
#include "Nodes/VoxelGraphLocalVariableNode.h"
#include "Nodes/VoxelGraphMacroParameterNode.h"
#include "Subsystems/AssetEditorSubsystem.h"

VOXEL_RUN_ON_STARTUP_EDITOR_COOK(RegisterGraphEditorCompiler)
{
	check(!GVoxelGraphEditorCompiler);
	GVoxelGraphEditorCompiler = new FVoxelGraphEditorCompiler();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelGraphEditorCompiler::CompileAll()
{
	VOXEL_FUNCTION_COUNTER();

	FARFilter Filter;
	Filter.ClassPaths.Add(UVoxelGraph::StaticClass()->GetClassPathName());
	Filter.ClassPaths.Add(UVoxelMacroLibrary::StaticClass()->GetClassPathName());

	const IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();

	TArray<FAssetData> AssetDatas;
	AssetRegistry.GetAssets(Filter, AssetDatas);

	TSet<UVoxelGraph*> Graphs;
	for (const FAssetData& AssetData : AssetDatas)
	{
		UObject* Asset = AssetData.GetAsset();
		if (!ensure(Asset))
		{
			continue;
		}

		if (Asset->IsA<UVoxelMacroLibrary>())
		{
			Graphs.Add(CastChecked<UVoxelMacroLibrary>(Asset)->Graph);
		}
		else if (ensure(Asset->IsA<UVoxelGraph>()))
		{
			Graphs.Add(CastChecked<UVoxelGraph>(Asset));
		}
	}

	TArray<FSoftObjectPath> AssetsToOpen;
	for (UVoxelGraph* Graph : Graphs)
	{
		if (!ensure(Graph))
		{
			continue;
		}

		bool bShouldOpen = false;
		for (const UVoxelGraph* SubGraph : Graph->GetAllGraphs())
		{
			SubGraph->GetRuntimeGraph().ForceRecompile();

			if (SubGraph->GetRuntimeGraph().HasCompileMessages())
			{
				bShouldOpen = true;
			}
		}

		if (bShouldOpen)
		{
			AssetsToOpen.Add(Graph);
		}
	}
	GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorsForAssets(AssetsToOpen);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelGraphEditorCompiler::ReconstructAllNodes(const UVoxelGraph& Graph)
{
	VOXEL_FUNCTION_COUNTER();

	if (!Graph.MainEdGraph)
	{
		// New asset
		return;
	}

	UVoxelEdGraph* EdGraph = Cast<UVoxelEdGraph>(Graph.MainEdGraph);
	if (!ensure(EdGraph))
	{
		return;
	}

	EdGraph->MigrateAndReconstructAll();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelRuntimeGraphData FVoxelGraphEditorCompiler::Translate(const UVoxelGraph& Graph)
{
	VOXEL_FUNCTION_COUNTER();
	ensure(GVoxelGraphCompileScope);

	UVoxelEdGraph* EdGraph = Cast<UVoxelEdGraph>(Graph.MainEdGraph);
	if (!EdGraph)
	{
		return {};
	}

	EdGraph->MigrateIfNeeded();

	// Sanitize nodes, we've had cases where some nodes are null
	EdGraph->Nodes.RemoveAllSwap([](const UEdGraphNode* Node)
	{
		return !IsValid(Node);
	});

	TVoxelArray<const UVoxelGraphNode*> Nodes;
	for (const UEdGraphNode* EdGraphNode : EdGraph->Nodes)
	{
		const UVoxelGraphNode* Node = Cast<UVoxelGraphNode>(EdGraphNode);
		if (!Node ||
			Node->IsA<UVoxelGraphKnotNode>())
		{
			continue;
		}
		Nodes.Add(Node);

		for (const UEdGraphPin* Pin : Node->Pins)
		{
			if (!ensure(Pin))
			{
				VOXEL_MESSAGE(Error, "Invalid pin on {0}", Node);
				return {};
			}

			if (Pin->ParentPin)
			{
				if (!ensure(Pin->ParentPin->SubPins.Contains(Pin)))
				{
					VOXEL_MESSAGE(Error, "Invalid sub-pin: {0}", Pin);
					return {};
				}
			}

			for (const UEdGraphPin* SubPin : Pin->SubPins)
			{
				if (!ensure(SubPin->ParentPin == Pin))
				{
					VOXEL_MESSAGE(Error, "Invalid parent pin: {0}", Pin);
					return {};
				}
			}

			for (const UEdGraphPin* LinkedPin : Pin->LinkedTo)
			{
				if (!ensure(LinkedPin))
				{
					VOXEL_MESSAGE(Error, "Invalid pin linked to {0}", Pin);
					return {};
				}

				if (!ensure(LinkedPin->LinkedTo.Contains(Pin)))
				{
					VOXEL_MESSAGE(Error, "Link from {0} to {1} is invalid", Pin, LinkedPin);
					return {};
				}
			}
		}
	}

	FVoxelRuntimeGraphData Data;

	for (const UEdGraphNode* EdGraphNode : Nodes)
	{
		FVoxelRuntimeNode RuntimeNode;
		RuntimeNode.VoxelNode = INLINE_LAMBDA -> TVoxelInstancedStruct<FVoxelNode>
		{
			if (const UVoxelGraphStructNode* GraphNode = Cast<UVoxelGraphStructNode>(EdGraphNode))
			{
				return GraphNode->Struct;
			}

			if (const UVoxelGraphMacroNode* GraphNode = Cast<UVoxelGraphMacroNode>(EdGraphNode))
			{
				FVoxelNode_FunctionCall Node;
				Node.GraphInterface = GraphNode->GraphInterface;
				Node.Type = GraphNode->Type;
				Node.FixupPins();
				return Node;
			}

			if (const UVoxelGraphParameterNode* GraphNode = Cast<UVoxelGraphParameterNode>(EdGraphNode))
			{
				const FVoxelGraphParameter Parameter = GraphNode->GetParameterSafe();

				FVoxelNode_Parameter Node;
				Node.ParameterName = Parameter.Name;
				Node.ParameterGuid = Parameter.Guid;
				Node.GetPin(Node.ValuePin).SetType(Parameter.Type);
				return Node;
			}

			if (const UVoxelGraphMacroParameterInputNode* GraphNode = Cast<UVoxelGraphMacroParameterInputNode>(EdGraphNode))
			{
				const FVoxelGraphParameter Parameter = GraphNode->GetParameterSafe();

				if (GraphNode->bExposeDefaultPin)
				{
					FVoxelNode_FunctionCallInput_WithDefaultPin Node;
					Node.Name = Parameter.Name;
					Node.GetPin(Node.DefaultPin).SetType(Parameter.Type);
					Node.GetPin(Node.ValuePin).SetType(Parameter.Type);
					return Node;
				}
				else
				{
					FVoxelNode_FunctionCallInput_WithoutDefaultPin Node;
					Node.Name = Parameter.Name;
					Node.DefaultValue = Parameter.DefaultValue;
					Node.GetPin(Node.ValuePin).SetType(Parameter.Type);
					return Node;
				}
			}

			if (const UVoxelGraphMacroParameterOutputNode* GraphNode = Cast<UVoxelGraphMacroParameterOutputNode>(EdGraphNode))
			{
				const FVoxelGraphParameter Parameter = GraphNode->GetParameterSafe();

				FVoxelNode_FunctionCallOutput Node;
				Node.Name = Parameter.Name;
				Node.GetPin(Node.ValuePin).SetType(Parameter.Type);
				return Node;
			}

			if (const UVoxelGraphLocalVariableDeclarationNode* GraphNode = Cast<UVoxelGraphLocalVariableDeclarationNode>(EdGraphNode))
			{
				const FVoxelGraphParameter Parameter = GraphNode->GetParameterSafe();

				FVoxelLocalVariableDeclaration Node;
				Node.Guid = Parameter.Guid;
				Node.GetPin(Node.InputPinPin).SetType(Parameter.Type);
				Node.GetPin(Node.OutputPinPin).SetType(Parameter.Type);
				return Node;
			}

			if (const UVoxelGraphLocalVariableUsageNode* GraphNode = Cast<UVoxelGraphLocalVariableUsageNode>(EdGraphNode))
			{
				const FVoxelGraphParameter Parameter = GraphNode->GetParameterSafe();

				FVoxelLocalVariableUsage Node;
				Node.Guid = Parameter.Guid;
				Node.GetPin(Node.OutputPinPin).SetType(Parameter.Type);
				return Node;
			}

			if (EdGraphNode->IsA<UDEPRECATED_VoxelGraphMacroParameterNode>())
			{
				// RuntimeGraph will automatically migrated on failure
				return {};
			}

			ensure(false);
			return {};
		};

		if (RuntimeNode.VoxelNode)
		{
			RuntimeNode.StructName = RuntimeNode.VoxelNode.GetScriptStruct()->GetFName();
		}

		RuntimeNode.EdGraphNodeName = EdGraphNode->GetFName();
		RuntimeNode.EdGraphNodeTitle = *EdGraphNode->GetNodeTitle(ENodeTitleType::FullTitle).ToString();

		ensure(!Data.NodeNameToNode.Contains(RuntimeNode.EdGraphNodeName));
		Data.NodeNameToNode.Add(RuntimeNode.EdGraphNodeName, RuntimeNode);
	}

	TMap<const UEdGraphPin*, FVoxelRuntimePinRef> EdGraphPinToPinRef;
	for (const UVoxelGraphNode* EdGraphNode : Nodes)
	{
		FVoxelRuntimeNode& RuntimeNode = Data.NodeNameToNode[EdGraphNode->GetFName()];

		const auto MakePin = [&](const UEdGraphPin* EdGraphPin)
		{
			ensure(!EdGraphPin->bOrphanedPin);

			FVoxelRuntimePin RuntimePin;
			RuntimePin.Type = EdGraphPin->PinType;
			RuntimePin.PinName = EdGraphPin->PinName;

			if (const UEdGraphPin* ParentPin = EdGraphPin->ParentPin)
			{
				const FVoxelPinType Type(ParentPin->PinType);
				if (ParentPin->Direction == EGPD_Input)
				{
					Data.TypeToMakeNode.Add(Type);
				}
				else
				{
					check(ParentPin->Direction == EGPD_Output);
					Data.TypeToBreakNode.Add(Type);
				}

				RuntimePin.ParentPinName = ParentPin->PinName;
			}

			if (RuntimePin.Type.HasPinDefaultValue())
			{
				RuntimePin.DefaultValue = FVoxelPinValue::MakeFromPinDefaultValue(*EdGraphPin);
			}
			else
			{
				ensure(!EdGraphPin->DefaultObject);
				ensure(EdGraphPin->DefaultValue.IsEmpty());
				ensure(EdGraphPin->AutogeneratedDefaultValue.IsEmpty());
			}

			return RuntimePin;
		};

		for (const UEdGraphPin* Pin : EdGraphNode->GetInputPins())
		{
			if (Pin->bOrphanedPin)
			{
				VOXEL_MESSAGE(Warning, "Orphaned pin {0}", Pin);
				RuntimeNode.Errors.Add("Orphaned pin " + Pin->GetDisplayName().ToString());
				continue;
			}

			if (!ensure(!RuntimeNode.InputPins.Contains(Pin->PinName)))
			{
				continue;
			}

			RuntimeNode.InputPins.Add(Pin->PinName, MakePin(Pin));
			EdGraphPinToPinRef.Add(Pin, FVoxelRuntimePinRef{ EdGraphNode->GetFName(), Pin->PinName, true });
		}
		for (const UEdGraphPin* Pin : EdGraphNode->GetOutputPins())
		{
			if (Pin->bOrphanedPin)
			{
				VOXEL_MESSAGE(Warning, "Orphaned pin {0}", Pin);
				RuntimeNode.Errors.Add("Orphaned pin " + Pin->GetDisplayName().ToString());
				continue;
			}

			if (!ensure(!RuntimeNode.OutputPins.Contains(Pin->PinName)))
			{
				continue;
			}

			RuntimeNode.OutputPins.Add(Pin->PinName, MakePin(Pin));
			EdGraphPinToPinRef.Add(Pin, FVoxelRuntimePinRef{ EdGraphNode->GetFName(), Pin->PinName, false });
		}
	}

	for (auto& It : Data.TypeToMakeNode)
	{
		const FVoxelNode* Node = FVoxelNodeLibrary::FindMakeNode(It.Key);
		if (!ensure(Node))
		{
			continue;
		}

		It.Value = FVoxelInstancedStruct::Make(*Node);
	}
	for (auto& It : Data.TypeToBreakNode)
	{
		const FVoxelNode* Node = FVoxelNodeLibrary::FindBreakNode(It.Key);
		if (!ensure(Node))
		{
			continue;
		}

		It.Value = FVoxelInstancedStruct::Make(*Node);
	}

	// Link the pins once they're all allocated
	for (const UVoxelGraphNode* EdGraphNode : Nodes)
	{
		FVoxelRuntimeNode* RuntimeNode = Data.NodeNameToNode.Find(EdGraphNode->GetFName());
		if (!ensure(RuntimeNode))
		{
			continue;
		}

		const auto AddPin = [&](const UEdGraphPin* Pin)
		{
			if (!ensure(EdGraphPinToPinRef.Contains(Pin)))
			{
				return false;
			}

			FVoxelRuntimePin* RuntimePin = Data.FindPin(EdGraphPinToPinRef[Pin]);
			if (!ensure(RuntimePin))
			{
				return false;
			}

			for (const UEdGraphPin* LinkedTo : Pin->LinkedTo)
			{
				if (LinkedTo->bOrphanedPin)
				{
					if (Pin->Direction == EGPD_Input)
					{
						RuntimeNode->Errors.Add(Pin->GetDisplayName().ToString() + " is connected to an orphaned pin");
					}
					continue;
				}

				if (UVoxelGraphKnotNode* Knot = Cast<UVoxelGraphKnotNode>(LinkedTo->GetOwningNode()))
				{
					for (const UEdGraphPin* LinkedToKnot : Knot->GetLinkedPins(Pin->Direction))
					{
						if (LinkedToKnot->bOrphanedPin)
						{
							continue;
						}

						if (ensure(EdGraphPinToPinRef.Contains(LinkedToKnot)))
						{
							RuntimePin->LinkedTo.Add(EdGraphPinToPinRef[LinkedToKnot]);
						}
					}
					continue;
				}

				if (ensure(EdGraphPinToPinRef.Contains(LinkedTo)))
				{
					RuntimePin->LinkedTo.Add(EdGraphPinToPinRef[LinkedTo]);
				}
			}

			return true;
		};

		for (const UEdGraphPin* Pin : EdGraphNode->GetInputPins())
		{
			if (Pin->bOrphanedPin)
			{
				continue;
			}

			if (!ensure(AddPin(Pin)))
			{
				return {};
			}
		}
		for (const UEdGraphPin* Pin : EdGraphNode->GetOutputPins())
		{
			if (Pin->bOrphanedPin)
			{
				continue;
			}

			if (!ensure(AddPin(Pin)))
			{
				return {};
			}
		}
	}

	for (const UVoxelGraphNode* Node : Nodes)
	{
		if (!Node->bEnableDebug)
		{
			continue;
		}

		Data.DebuggedNodes.Add(Node->GetFName());
	}

	INLINE_LAMBDA
	{
		VOXEL_SCOPE_COUNTER("Setup preview");

		TVoxelArray<UEdGraphPin*> PreviewedPins;
		for (const UVoxelGraphNode* Node : Nodes)
		{
			if (!Node->bEnablePreview)
			{
				continue;
			}

			UEdGraphPin* PreviewedPin = Node->FindPin(Node->PreviewedPin);
			if (!PreviewedPin)
			{
				VOXEL_MESSAGE(Warning, "{0}: Invalid pin to preview", Node);
				continue;
			}

			PreviewedPins.Add(PreviewedPin);
		}

		if (PreviewedPins.Num() > 1)
		{
			VOXEL_MESSAGE(Warning, "Multiple pins being previewed, resetting");

			for (const UEdGraphPin* PreviewedPin : PreviewedPins)
			{
				CastChecked<UVoxelGraphNode>(PreviewedPin->GetOwningNode())->bEnablePreview = false;
			}
			return;
		}

		if (PreviewedPins.Num() == 0)
		{
			return;
		}

		const UEdGraphPin& PreviewedPin = *PreviewedPins[0];
		UVoxelGraphNode* PreviewedNode = CastChecked<UVoxelGraphNode>(PreviewedPin.GetOwningNode());

		FVoxelInstancedStruct PreviewHandler;
		for (const FVoxelPinPreviewSettings& PreviewSettings : PreviewedNode->PreviewSettings)
		{
			if (PreviewSettings.PinName == PreviewedPin.PinName)
			{
				ensure(!PreviewHandler.IsValid());
				PreviewHandler = PreviewSettings.PreviewHandler;
			}
		}
		if (!PreviewHandler.IsValid())
		{
			VOXEL_MESSAGE(Warning, "{0}: Invalid preview settings", PreviewedNode);
			return;
		}

		Data.PreviewedPin = FVoxelRuntimePinRef
		{
			PreviewedNode->GetFName(),
			PreviewedPin.PinName,
			PreviewedPin.Direction == EGPD_Input
		};

		Data.PreviewedPinType = PreviewedPin.PinType;
		Data.PreviewHandler = PreviewHandler;
	};

	return Data;
}