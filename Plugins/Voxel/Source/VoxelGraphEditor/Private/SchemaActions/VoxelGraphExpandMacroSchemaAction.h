// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "VoxelGraphSchemaAction.h"
#include "Nodes/VoxelGraphMacroNode.h"
#include "VoxelGraphExpandMacroSchemaAction.generated.h"

class UVoxelGraph;
class UVoxelGraphMacroNode;

USTRUCT()
struct FVoxelGraphSchemaAction_ExpandMacro : public FVoxelGraphSchemaAction
{
	GENERATED_BODY();

public:
	using FVoxelGraphSchemaAction::FVoxelGraphSchemaAction;

	//~ Begin FVoxelGraphSchemaAction Interface
	virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
	//~ End FVoxelGraphSchemaAction Interface

private:
	void FindNearestSuitableLocation(const TSharedPtr<SGraphEditor>& GraphEditor, const UVoxelGraphMacroNode* MacroNode);
	void GroupNodesToCopy(UEdGraph* EdGraph);
	void ConnectNewNodes(const UVoxelGraphMacroNode* MacroNode);

private:
	void ExportNodes(FString& ExportText) const;
	void ImportNodes(const TSharedPtr<SGraphEditor>& GraphEditor, UVoxelGraph* Graph, const FString& ExportText);

private:
	struct FCopiedNode
	{
		TWeakObjectPtr<UEdGraphNode> OriginalNode;
		TWeakObjectPtr<UEdGraphNode> NewNode;
		TMap<FEdGraphPinReference, TSet<FGuid>> MappedOriginalPinsToParams;
	};

	FIntPoint SuitablePosition;
	FSlateRect PastedNodesBounds = FSlateRect(FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX);
	TMap<FGuid, TSharedPtr<FCopiedNode>> CopiedNodes;
};