// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "DragDropActions/VoxelMembersBaseDragDropAction.h"

struct FVoxelGraphToolkit;
class UVoxelGraph;

class FVoxelGraphMacroDragDropAction : public FVoxelMembersBaseDragDropAction
{
public:
	DRAG_DROP_OPERATOR_TYPE(FMacroDragDropAction, FGraphSchemaActionDragDropAction)

	static TSharedRef<FVoxelGraphMacroDragDropAction> New(const TSharedPtr<FEdGraphSchemaAction>& InAction, const TWeakObjectPtr<UVoxelGraph> Macro)
	{
		const TSharedRef<FVoxelGraphMacroDragDropAction> Operation = MakeVoxelShared<FVoxelGraphMacroDragDropAction>();
		Operation->WeakMacroGraph = Macro;
		Operation->SourceAction = InAction;
		Operation->Construct();
		return Operation;
	}

	//~ Begin FGraphSchemaActionDragDropAction Interface
	virtual void HoverTargetChanged() override;
	virtual FReply DroppedOnPanel(const TSharedRef<SWidget>& Panel, FVector2D ScreenPosition, FVector2D GraphPosition, UEdGraph& EdGraph) override;
	//~ End FGraphSchemaActionDragDropAction Interface

private:
	TWeakObjectPtr<UVoxelGraph> WeakMacroGraph;
};