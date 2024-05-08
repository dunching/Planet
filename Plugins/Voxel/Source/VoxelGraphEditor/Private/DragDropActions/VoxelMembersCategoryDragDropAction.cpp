// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "DragDropActions/VoxelMembersCategoryDragDropAction.h"

void FVoxelMembersCategoryDragDropAction::HoverTargetChanged()
{
	if (!HoveredCategoryName.IsEmpty())
	{
		if (HoveredCategoryName.ToString() == DraggedCategory)
		{
			SetFeedbackMessageError("Cannot insert category '" + DraggedCategory + "' before itself");
			return;
		}

		SetFeedbackMessageOK("Move category '" + DraggedCategory + "' before '" + HoveredCategoryName.ToString() + "'");
		return;
	}

	if (HoveredAction.IsValid())
	{
		SetFeedbackMessageError("Can only insert before another category");
		return;
	}

	SetFeedbackMessageError("Moving category '" + DraggedCategory + "'");
}

FReply FVoxelMembersCategoryDragDropAction::DroppedOnCategory(FText OnCategory)
{
	const TSharedPtr<SVoxelMembers> MembersWidget = WeakMembersWidget.Pin();
	if (!ensure(MembersWidget))
	{
		return FReply::Handled();
	}

	const TSharedPtr<FVoxelToolkit> Toolkit = MembersWidget->GetToolkit();
	if (!ensure(Toolkit))
	{
		return FReply::Handled();
	}

	const FString CategoryDelim = "|";
	TArray<FString>& SortedCategories = MembersWidget->GetEditableCategories(SectionId);

	// Need to copy and rework categories list, since `OnCategory` is changed with GetCategoryDisplayString function
	TArray<FString> CopiedCategories;
	for (const FString& Category : SortedCategories)
	{
		CopiedCategories.Add(FEditorCategoryUtils::GetCategoryDisplayString(Category));
	}

	TArray<FString> OnCategoryChain;
	OnCategory.ToString().ParseIntoArray(OnCategoryChain, *CategoryDelim, true);
	TArray<FString> DraggedCategoryChain;
	FEditorCategoryUtils::GetCategoryDisplayString(DraggedCategory).ParseIntoArray(DraggedCategoryChain, *CategoryDelim, true);

	FString SearchStart;
	for (int32 Index = 0; Index < FMath::Min(OnCategoryChain.Num(), DraggedCategoryChain.Num()); Index++)
	{
		if (SearchStart.IsEmpty())
		{
			SearchStart = OnCategoryChain[Index];
		}
		else
		{
			SearchStart += "|" + OnCategoryChain[Index];
		}

		if (OnCategoryChain[Index] != DraggedCategoryChain[Index])
		{
			break;
		}
	}

	FString CategoryToMoveBefore;
	for (int32 Index = 0; Index < CopiedCategories.Num(); Index++)
	{
		if (CopiedCategories[Index].StartsWith(SearchStart))
		{
			CategoryToMoveBefore = CopiedCategories[Index];
			break;
		}
	}

	if (CategoryToMoveBefore.IsEmpty())
	{
		return FReply::Handled();
	}

	const FVoxelTransaction Transaction(Toolkit->GetAsset(), "Move member to category");

	int32 DraggedCategoryIndex = SortedCategories.IndexOfByKey(DraggedCategory);
	if (DraggedCategoryIndex == -1)
	{
		for (int32 Index = 0; Index < CopiedCategories.Num(); Index++)
		{
			if (CopiedCategories[Index].StartsWith(DraggedCategory))
			{
				DraggedCategoryIndex = Index;
				break;
			}
		}
		if (!ensure(DraggedCategoryIndex != -1))
		{
			return FReply::Handled();
		}
	}

	const FString OriginalCategory = SortedCategories[DraggedCategoryIndex];

	SortedCategories.RemoveAt(DraggedCategoryIndex);
	CopiedCategories.RemoveAt(DraggedCategoryIndex);

	const int32 OnCategoryIndex = CopiedCategories.IndexOfByKey(CategoryToMoveBefore);
	SortedCategories.Insert(OriginalCategory, OnCategoryIndex);

	return FReply::Handled();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


void FVoxelMembersCategoryDragDropAction::SetFeedbackMessageError(const FString& Message)
{
	const FSlateBrush* StatusSymbol = FAppStyle::GetBrush("Graph.ConnectorFeedback.Error");
	SetSimpleFeedbackMessage(StatusSymbol, FLinearColor::White, FText::FromString(Message));
}

void FVoxelMembersCategoryDragDropAction::SetFeedbackMessageOK(const FString& Message)
{
	const FSlateBrush* StatusSymbol = FAppStyle::GetBrush("Graph.ConnectorFeedback.OK");
	SetSimpleFeedbackMessage(StatusSymbol, FLinearColor::White, FText::FromString(Message));
}