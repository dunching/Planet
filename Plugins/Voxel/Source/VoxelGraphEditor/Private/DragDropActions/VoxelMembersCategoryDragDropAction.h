// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "Widgets/SVoxelMembers.h"
#include "GraphEditorDragDropAction.h"

class FVoxelMembersCategoryDragDropAction : public FGraphEditorDragDropAction
{
public:
	DRAG_DROP_OPERATOR_TYPE(FCategoryDragDropAction, FGraphEditorDragDropAction)

	static TSharedRef<FVoxelMembersCategoryDragDropAction> New(const FString& InCategory, const TSharedPtr<SVoxelMembers>& MembersWidget, const int32 SectionId)
	{
		const TSharedRef<FVoxelMembersCategoryDragDropAction> Operation = MakeVoxelShared<FVoxelMembersCategoryDragDropAction>();
		Operation->DraggedCategory = InCategory;
		Operation->SectionId = SectionId;
		Operation->WeakMembersWidget = MembersWidget;
		Operation->Construct();
		return Operation;
	}

	//~ Begin FGraphSchemaActionDragDropAction Interface
	virtual void HoverTargetChanged() override;
	virtual FReply DroppedOnCategory(FText Category) override;
	//~ End FGraphSchemaActionDragDropAction Interface

private:
	int32 SectionId = 0;
	FString DraggedCategory;
	TWeakPtr<SVoxelMembers> WeakMembersWidget;

	void SetFeedbackMessageError(const FString& Message);
	void SetFeedbackMessageOK(const FString& Message);
};