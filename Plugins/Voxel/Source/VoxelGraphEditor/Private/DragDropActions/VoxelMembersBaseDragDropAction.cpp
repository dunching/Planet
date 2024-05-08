// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "DragDropActions/VoxelMembersBaseDragDropAction.h"
#include "VoxelEdGraph.h"

void FVoxelMembersBaseDragDropAction::HoverTargetChanged()
{
	if (!SourceAction)
	{
		FGraphSchemaActionDragDropAction::HoverTargetChanged();
		return;
	}

	if (!HoveredCategoryName.IsEmpty())
	{
		if (HoveredCategoryName.ToString() == SourceAction->GetCategory().ToString())
		{
			SetFeedbackMessageError("'" + SourceAction->GetMenuDescription().ToString() + "' is already in category '" + HoveredCategoryName.ToString() + "'");
		}
		else
		{
			SetFeedbackMessageOK("Move '" + SourceAction->GetMenuDescription().ToString() + "' to category '" + HoveredCategoryName.ToString() + "'");
		}
		return;
	}

	const TSharedPtr<FEdGraphSchemaAction> TargetAction = HoveredAction.Pin();
	if (!TargetAction)
	{
		FGraphSchemaActionDragDropAction::HoverTargetChanged();
		return;
	}

	if (TargetAction->SectionID == SourceAction->SectionID)
	{
		if (SourceAction->GetPersistentItemDefiningObject() == TargetAction->GetPersistentItemDefiningObject())
		{
			const int32 MovingItemIndex = SourceAction->GetReorderIndexInContainer();
			const int32 TargetVarIndex = TargetAction->GetReorderIndexInContainer();

			if (MovingItemIndex == INDEX_NONE)
			{
				SetFeedbackMessageError("Cannot reorder '" + SourceAction->GetMenuDescription().ToString() + "'.");
			}
			else if (TargetVarIndex == INDEX_NONE)
			{
				SetFeedbackMessageError("Cannot reorder '" + SourceAction->GetMenuDescription().ToString() + "' before '" + TargetAction->GetMenuDescription().ToString() + "'.");
			}
			else if (TargetAction == SourceAction)
			{
				SetFeedbackMessageError("Cannot reorder '" + SourceAction->GetMenuDescription().ToString() + "' before itself.");
			}
			else
			{
				SetFeedbackMessageOK("Reorder '" + SourceAction->GetMenuDescription().ToString() + "' before '" + TargetAction->GetMenuDescription().ToString() + "'");
			}
		}
		else
		{
			SetFeedbackMessageError("Cannot reorder '" + SourceAction->GetMenuDescription().ToString() + "' into a different scope.");
		}
	}
	else
	{
		SetFeedbackMessageError("Cannot reorder '" + SourceAction->GetMenuDescription().ToString() + "' into a different section.");
	}
}

FReply FVoxelMembersBaseDragDropAction::DroppedOnCategory(const FText Category)
{
	if (SourceAction)
	{
		SourceAction->MovePersistentItemToCategory(Category);
	}

	return FReply::Handled();
}

FReply FVoxelMembersBaseDragDropAction::DroppedOnAction(TSharedRef<FEdGraphSchemaAction> Action)
{
	if (!SourceAction.IsValid() ||
		SourceAction->GetTypeId() != Action->GetTypeId() ||
		SourceAction->GetPersistentItemDefiningObject() != Action->GetPersistentItemDefiningObject() ||
		SourceAction->SectionID != Action->SectionID)
	{
		return FReply::Unhandled();
	}

	SourceAction->ReorderToBeforeAction(Action);
	return FReply::Handled();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelMembersBaseDragDropAction::SetFeedbackMessageError(const FString& Message)
{
	const FSlateBrush* StatusSymbol = FAppStyle::GetBrush("Graph.ConnectorFeedback.Error");
	SetSimpleFeedbackMessage(StatusSymbol, FLinearColor::White, FText::FromString(Message));
}

void FVoxelMembersBaseDragDropAction::SetFeedbackMessageOK(const FString& Message)
{
	const FSlateBrush* StatusSymbol = FAppStyle::GetBrush("Graph.ConnectorFeedback.OK");
	SetSimpleFeedbackMessage(StatusSymbol, FLinearColor::White, FText::FromString(Message));
}