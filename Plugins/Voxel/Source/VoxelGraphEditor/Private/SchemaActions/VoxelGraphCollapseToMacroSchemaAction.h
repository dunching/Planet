// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "VoxelGraphSchemaAction.h"
#include "Nodes/VoxelGraphMacroParameterNode.h"
#include "VoxelGraphCollapseToMacroSchemaAction.generated.h"

class UVoxelGraphMacroNode;

USTRUCT()
struct FVoxelGraphSchemaAction_CollapseToMacro : public FVoxelGraphSchemaAction
{
	GENERATED_BODY();

public:
	using FVoxelGraphSchemaAction::FVoxelGraphSchemaAction;

	//~ Begin FVoxelGraphSchemaAction Interface
	virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
	//~ End FVoxelGraphSchemaAction Interface

private:
	void GroupSelectedNodes(const TSet<UObject*>& SelectedNodes);
	void UpdateParameters(UVoxelGraph* Graph);
	void AddParameterInputs(UVoxelGraph* Graph);
	void AddDeclarationOutputs(UVoxelGraph* Graph);
	void AddOuterParameters(UVoxelGraph* Graph);
	void FixupMainGraph(const UVoxelGraphMacroNode* MacroNode, UEdGraph* EdGraph);

private:
	void ExportNodes(FString& ExportText) const;
	void ImportNodes(UVoxelGraph* Graph, const FString& ExportText);
	FVector2D GetNodePosition(const UEdGraphNode* Node) const;

private:
	struct FCopiedNode
	{
		TWeakObjectPtr<UEdGraphNode> OriginalNode;
		TWeakObjectPtr<UEdGraphNode> NewNode;
		TMap<FEdGraphPinReference, TSet<FEdGraphPinReference>> OutsideConnectedPins;
		TMap<FGuid, TSet<FEdGraphPinReference>> MappedInputsOutputs;

		FCopiedNode(UEdGraphNode* Node, const TMap<FEdGraphPinReference, TSet<FEdGraphPinReference>>& OutsideConnectedPins)
			: OriginalNode(Node)
			, OutsideConnectedPins(OutsideConnectedPins)
		{
		}

		template<typename T>
		T* GetOriginalNode()
		{
			return Cast<T>(OriginalNode.Get());
		}

		template<typename T>
		T* GetNewNode()
		{
			return Cast<T>(NewNode.Get());
		}
	};

	struct FCopiedParameter
	{
		EVoxelGraphParameterType ParameterType;
		FGuid NewParameterId;
		FGuid OriginalParameterId;

		FGuid InputId;
		FGuid OutputId;

		TSharedPtr<FCopiedNode> DeclarationNode;
		TSet<TSharedPtr<FCopiedNode>> UsageNodes;

		explicit FCopiedParameter(const EVoxelGraphParameterType ParameterType, const FGuid OriginalParameterId)
			: ParameterType(ParameterType)
			, OriginalParameterId(OriginalParameterId)
		{}
	};

	TMap<FGuid, TSharedPtr<FCopiedNode>> CopiedNodes;
	TMap<FGuid, TSharedPtr<FCopiedParameter>> CopiedParameters;

	FVector2D AvgNodePosition;
	FVector2D InputDeclarationPosition;
	FVector2D OutputDeclarationPosition;
};