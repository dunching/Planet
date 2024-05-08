// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "GraphEditorDragDropAction.h"

class VOXELGRAPHEDITOR_API FVoxelMembersBaseDragDropAction : public FGraphSchemaActionDragDropAction
{
public:
	DRAG_DROP_OPERATOR_TYPE(FVoxelMembersBaseDragDropAction, FGraphSchemaActionDragDropAction)

	static TSharedRef<FVoxelMembersBaseDragDropAction> New(const TSharedPtr<FEdGraphSchemaAction>& Action)
	{
		TSharedRef<FVoxelMembersBaseDragDropAction> Operation = MakeVoxelShared<FVoxelMembersBaseDragDropAction>();
		Operation->SourceAction = Action;
		Operation->Construct();
		return Operation;
	}

	//~ Begin FGraphSchemaActionDragDropAction Interface
	virtual void HoverTargetChanged() override;
	virtual FReply DroppedOnCategory(const FText Category) override;
	virtual FReply DroppedOnAction(TSharedRef<FEdGraphSchemaAction> Action) override;
	//~ End FGraphSchemaActionDragDropAction Interface

protected:
	void SetFeedbackMessageError(const FString& Message);
	void SetFeedbackMessageOK(const FString& Message);
};