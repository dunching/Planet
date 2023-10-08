// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "DragDropActions/VoxelMembersBaseDragDropAction.h"

class UVoxelGraph;

class VOXELGRAPHEDITOR_API FVoxelMembersParameterDragDropAction : public FVoxelMembersBaseDragDropAction
{
public:
	DRAG_DROP_OPERATOR_TYPE(FVoxelMembersParameterDragDropAction, FGraphSchemaActionDragDropAction)

	const TWeakObjectPtr<UVoxelGraph> WeakGraph;
	const FGuid ParameterGuid;
	const bool bAltDrag;
	const bool bControlDrag;

	static TSharedRef<FVoxelMembersParameterDragDropAction> New(
		const TSharedPtr<FEdGraphSchemaAction>& Action,
		const TWeakObjectPtr<UVoxelGraph>& WeakGraph,
		const FGuid& ParameterGuid,
		const FPointerEvent& MouseEvent)
	{
		const TSharedRef<FVoxelMembersParameterDragDropAction> Operation = MakeVoxelShareable(new (GVoxelMemory) FVoxelMembersParameterDragDropAction(
			WeakGraph,
			ParameterGuid,
			MouseEvent.IsAltDown(),
			MouseEvent.IsControlDown()));
		Operation->SourceAction = Action;
		Operation->Construct();
		return Operation;
	}

	//~ Begin FGraphSchemaActionDragDropAction Interface
	virtual void HoverTargetChanged() override;

	virtual FReply DroppedOnPin(FVector2D ScreenPosition, FVector2D GraphPosition) override;
	virtual FReply DroppedOnNode(FVector2D ScreenPosition, FVector2D GraphPosition) override;
	virtual FReply DroppedOnPanel(const TSharedRef<SWidget>& Panel, FVector2D ScreenPosition, FVector2D GraphPosition, UEdGraph& EdGraph) override;

	virtual void GetDefaultStatusSymbol(const FSlateBrush*& PrimaryBrushOut, FSlateColor& IconColorOut, FSlateBrush const*& SecondaryBrushOut, FSlateColor& SecondaryColorOut) const override;
	//~ End FGraphSchemaActionDragDropAction Interface

private:
	FVoxelMembersParameterDragDropAction(
		const TWeakObjectPtr<UVoxelGraph>& WeakGraph,
		const FGuid& ParameterGuid,
		const bool bAltDrag,
		const bool bControlDrag)
		: WeakGraph(WeakGraph)
		, ParameterGuid(ParameterGuid)
		, bAltDrag(bAltDrag)
		, bControlDrag(bControlDrag)
	{
	}
};