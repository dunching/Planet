// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "SVoxelCurveKeySelector.h"
#include "CurveEditor.h"
#include "CurveEditorTypes.h"
#include "EditorFontGlyphs.h"
#include "Tree/SCurveEditorTree.h"

void SVoxelCurveKeySelector::Construct(const FArguments& InArgs, const TSharedPtr<FCurveEditor>& InCurveEditor, const FCurveEditorTreeItemID& InCurveTreeItemId)
{
	CurveEditor = InCurveEditor;
	CurveTreeItemId = InCurveTreeItemId;

	ChildSlot
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(0.f, 0.f, 4.f, 0.f)
		[
			SNew(SButton)
			.ButtonStyle(FAppStyle::Get(), "SimpleButton")
			.OnClicked(this, &SVoxelCurveKeySelector::ZoomToFitClicked)
			.ToolTipText(INVTEXT("Zoom to fit all keys"))
			.Content()
			[
				SNew(STextBlock)
				.Font(FAppStyle::Get().GetFontStyle("FontAwesome.10"))
				.ColorAndOpacity(FSlateColor::UseForeground())
				.Text(FEditorFontGlyphs::Expand)
			]
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(0.f, 0.f, 1.f, 0.f)
		[
			SNew(SButton)
			.ButtonStyle(FAppStyle::Get(), "SimpleButton")
			.OnClicked(this, &SVoxelCurveKeySelector::PreviousKeyClicked)
			.ToolTipText(INVTEXT("Select the previous key for the selected curve."))
			.Content()
			[
				SNew(SImage)
				.Image(FAppStyle::Get().GetBrush("Icons.ArrowLeft"))
				.ColorAndOpacity(FSlateColor::UseForeground())
			]
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(0.f, 0.f, 4.f, 0.f)
		[
			SNew(SButton)
			.ButtonStyle(FAppStyle::Get(), "SimpleButton")
			.OnClicked(this, &SVoxelCurveKeySelector::NextKeyClicked)
			.ToolTipText(INVTEXT("Select the next key for the selected curve."))
			.Content()
			[
				SNew(SImage)
				.Image(FAppStyle::Get().GetBrush("Icons.ArrowRight"))
				.ColorAndOpacity(FSlateColor::UseForeground())
			]
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(0.f, 0.f, 1.f, 0.f)
		[
			SNew(SButton)
			.ButtonStyle(FAppStyle::Get(), "SimpleButton")
			.OnClicked(this, &SVoxelCurveKeySelector::AddKeyClicked)
			.ToolTipText(INVTEXT("Add a key to the selected curve."))
			.Content()
			[
				SNew(SImage)
				.Image(FAppStyle::Get().GetBrush("Icons.PlusCircle"))
				.ColorAndOpacity(FSlateColor::UseForeground())
			]
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.ButtonStyle(FAppStyle::Get(), "SimpleButton")
			.OnClicked(this, &SVoxelCurveKeySelector::DeleteKeyClicked)
			.ToolTipText(INVTEXT("Delete the currently selected keys."))
			.Content()
			[
				SNew(SImage)
				.Image(FAppStyle::Get().GetBrush("Icons.Delete"))
				.ColorAndOpacity(FSlateColor::UseForeground())
			]
		]
	];
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FReply SVoxelCurveKeySelector::ZoomToFitClicked() const
{
	TArray<FCurveModelID> ActiveCurveModelIds;
	GetOrderedActiveCurveModelIds(ActiveCurveModelIds);
	CurveEditor->ZoomToFitCurves(ActiveCurveModelIds);

	return FReply::Handled();
}

FReply SVoxelCurveKeySelector::PreviousKeyClicked()
{
	NavigateToAdjacentKey(false);
	return FReply::Handled();
}

FReply SVoxelCurveKeySelector::NextKeyClicked()
{
	NavigateToAdjacentKey(true);
	return FReply::Handled();
}

FReply SVoxelCurveKeySelector::AddKeyClicked()
{
	TOptional<FCurveModelID> CurveModelIdForAdd;
	TArray<FKeyHandle> SelectedKeyHandles;
	GetActiveCurveModelAndSelectedKeys(CurveModelIdForAdd, SelectedKeyHandles);

	if (!CurveModelIdForAdd.IsSet())
	{
		return FReply::Handled();
	}

	const TUniquePtr<FCurveModel>* CurveModelPtr = CurveEditor->GetCurves().Find(CurveModelIdForAdd.GetValue());
	if (!CurveModelPtr)
	{
		return FReply::Handled();
	}

	FCurveModel* CurveModel = CurveModelPtr->Get();
	if (!CurveModel)
	{
		return FReply::Handled();
	}

	FKeyPosition NewKeyPosition;
	FKeyAttributes NewKeyAttributes = CurveEditor->GetDefaultKeyAttributes().Get();
	NewKeyAttributes.SetInterpMode(RCIM_Cubic);
	NewKeyAttributes.SetTangentMode(RCTM_Auto);

	INLINE_LAMBDA
	{
		if (CurveModel->GetNumKeys() == 0)
		{
			// If there are no keys, add one at 0, 0.
			NewKeyPosition.InputValue = 0.0f;
			NewKeyPosition.OutputValue = 0.0f;
			return;
		}

		if (CurveModel->GetNumKeys() == 1)
		{
			// If there's a single key, add the new key at the same value, but time + 1.
			TArray<FKeyHandle> KeyHandles;
			CurveModel->GetKeys(*CurveEditor.Get(), TNumericLimits<double>::Lowest(), TNumericLimits<double>::Max(), TNumericLimits<double>::Lowest(), TNumericLimits<double>::Max(), KeyHandles);
			
			TArray<FKeyPosition> KeyPositions;
			KeyPositions.AddDefaulted();
			CurveModel->GetKeyPositions(KeyHandles, KeyPositions);
					
			NewKeyPosition.InputValue = KeyPositions[0].InputValue + 1;
			NewKeyPosition.OutputValue = KeyPositions[0].OutputValue;
			return;
		}

		TArray<FKeyHandlePositionPair> SortedKeyHandlePositionPairs;
		GetSortedKeyHandlessAndPositionsForModel(*CurveModel, SortedKeyHandlePositionPairs);

		int32 IndexToAddAfter = -1;
		if (SelectedKeyHandles.Num() > 0)
		{
			for (int32 Index = SortedKeyHandlePositionPairs.Num() - 1; Index >= 0; Index--)
			{
				if (SelectedKeyHandles.Contains(SortedKeyHandlePositionPairs[Index].Handle))
				{
					IndexToAddAfter = Index;
					break;
				}
			}
		}

		if (IndexToAddAfter == -1)
		{
			IndexToAddAfter = SortedKeyHandlePositionPairs.Num() - 1;
		}

		if (IndexToAddAfter == SortedKeyHandlePositionPairs.Num() - 1)
		{
			const FKeyPosition& TargetKeyPosition = SortedKeyHandlePositionPairs[IndexToAddAfter].Position;
			const FKeyPosition& PreviousKeyPosition = SortedKeyHandlePositionPairs[IndexToAddAfter - 1].Position;
			NewKeyPosition.InputValue = TargetKeyPosition.InputValue + (TargetKeyPosition.InputValue - PreviousKeyPosition.InputValue);
			NewKeyPosition.OutputValue = TargetKeyPosition.OutputValue + (TargetKeyPosition.OutputValue - PreviousKeyPosition.OutputValue);
			return;
		}

		const FKeyPosition& TargetKeyPosition = SortedKeyHandlePositionPairs[IndexToAddAfter].Position;
		const FKeyPosition& NextKeyPosition = SortedKeyHandlePositionPairs[IndexToAddAfter + 1].Position;
		NewKeyPosition.InputValue = TargetKeyPosition.InputValue + ((NextKeyPosition.InputValue - TargetKeyPosition.InputValue) / 2);
		NewKeyPosition.OutputValue = TargetKeyPosition.OutputValue + ((NextKeyPosition.OutputValue - TargetKeyPosition.OutputValue) / 2);
	};

	TOptional<FKeyHandle> NewKeyHandle = CurveModel->AddKey(NewKeyPosition, NewKeyAttributes);
	if (!NewKeyHandle.IsSet())
	{
		return FReply::Handled();
	}

	FCurveEditorSelection& CurveEditorSelection = CurveEditor->GetSelection();
	CurveEditorSelection.Clear();
	CurveEditorSelection.Add(CurveModelIdForAdd.GetValue(), ECurvePointType::Key, NewKeyHandle.GetValue());

	TArray<FCurveModelID> CurvesToFit;
	CurvesToFit.Add(CurveModelIdForAdd.GetValue());
	CurveEditor->ZoomToFitCurves(CurvesToFit);

	return FReply::Handled();
}

FReply SVoxelCurveKeySelector::DeleteKeyClicked() const
{
	CurveEditor->DeleteSelection();
	return FReply::Handled();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void SVoxelCurveKeySelector::NavigateToAdjacentKey(const bool bNext)
{
	TOptional<FCurveModelID> ActiveCurveModelId;
	TArray<FKeyHandle> SelectedKeyHandles;
	GetActiveCurveModelAndSelectedKeys(ActiveCurveModelId, SelectedKeyHandles);

	TOptional<FCurveModelID> CurveModelIdToSelect;
	TOptional<FKeyHandle> KeyHandleToSelect;
	INLINE_LAMBDA
	{
		if (!ActiveCurveModelId.IsSet())
		{
			return;
		}

		const TUniquePtr<FCurveModel>* CurveModelPtr = CurveEditor->GetCurves().Find(ActiveCurveModelId.GetValue());
		if (!CurveModelPtr)
		{
			return;
		}

		const FCurveModel* CurveModel = CurveModelPtr->Get();
		if (!CurveModel)
		{
			return;
		}

		TArray<FKeyHandlePositionPair> ActiveSortedKeyHandlePositionPairs;
		GetSortedKeyHandlessAndPositionsForModel(*CurveModel, ActiveSortedKeyHandlePositionPairs);

		if (SelectedKeyHandles.Num() > 0)
		{
			// If there's currently a selected key on the active curve then we want to use that as the target for navigating.
			int32 TargetSelectedKeyIndex = -1;
			const int32 StartIndex = bNext ? ActiveSortedKeyHandlePositionPairs.Num() - 1 : 0;
			const int32 EndIndex = bNext ? -1 : ActiveSortedKeyHandlePositionPairs.Num();
			const int32 IndexOffset = bNext ? -1 : 1;
			for (int32 Index = StartIndex; Index != EndIndex; Index += IndexOffset)
			{
				if (SelectedKeyHandles.Contains(ActiveSortedKeyHandlePositionPairs[Index].Handle))
				{
					TargetSelectedKeyIndex = Index;
					break;
				}
			}

			if (TargetSelectedKeyIndex == -1)
			{
				return;
			}

			if (!bNext &&
				TargetSelectedKeyIndex > 0)
			{
				// If we're navigating previous and we're not at the first key we can just select the previous key on this curve.
				CurveModelIdToSelect = ActiveCurveModelId;
				KeyHandleToSelect = ActiveSortedKeyHandlePositionPairs[TargetSelectedKeyIndex - 1].Handle;
				return;
			}
			if (bNext &&
				TargetSelectedKeyIndex < ActiveSortedKeyHandlePositionPairs.Num() - 1)
			{
				// If we're navigating next and we're not at the last key we can just select the next key on this curve.
				CurveModelIdToSelect = ActiveCurveModelId;
				KeyHandleToSelect = ActiveSortedKeyHandlePositionPairs[TargetSelectedKeyIndex + 1].Handle;
				return;
			}

			// Otherwise we're going to need to navigate to another curve since we're at the start or end of the current curve.
			TArray<FCurveModelID> OrderedActiveCurveModelIds;
			GetOrderedActiveCurveModelIds(OrderedActiveCurveModelIds);

			// Find the adjacent curve with keys so we can select a key there.
			FCurveModel* CurveModelToSelect = nullptr;
			int32 CurrentIndex = OrderedActiveCurveModelIds.IndexOfByKey(ActiveCurveModelId);
			const int32 CurrentIndexOffset = bNext ? 1 : -1;
			while (!CurveModelIdToSelect.IsSet())
			{
				CurrentIndex += CurrentIndexOffset;
				if (CurrentIndex < 0)
				{
					CurrentIndex = OrderedActiveCurveModelIds.Num() - 1;
				}
				else if (CurrentIndex > OrderedActiveCurveModelIds.Num() - 1)
				{
					CurrentIndex = 0;
				}

				const TUniquePtr<FCurveModel>* CurrentCurveModelPtr = CurveEditor->GetCurves().Find(OrderedActiveCurveModelIds[CurrentIndex]);
				if (!CurrentCurveModelPtr)
				{
					continue;
				}

				FCurveModel* CurrentCurveModel = CurrentCurveModelPtr->Get();
				if (!CurrentCurveModel)
				{
					continue;
				}

				if (CurrentCurveModel->GetNumKeys() > 0)
				{
					CurveModelIdToSelect = OrderedActiveCurveModelIds[CurrentIndex];
					CurveModelToSelect = CurrentCurveModel;
				}
			}

			if (CurveModelIdToSelect == ActiveCurveModelId)
			{
				// There were no other active curves with keys to select from so just wrap around the currently active curve.
				if (bNext)
				{
					KeyHandleToSelect = ActiveSortedKeyHandlePositionPairs[0].Handle;
				}
				else
				{
					KeyHandleToSelect = ActiveSortedKeyHandlePositionPairs.Last().Handle;
				}
			}
			else if (CurveModelToSelect)
			{
				// We're selecting a key on a different curve so we need to sort the positions and select the first or last based
				// on the navigation direction.
				TArray<FKeyHandlePositionPair> SortedKeyHandlePositionPairs;
				GetSortedKeyHandlessAndPositionsForModel(*CurveModelToSelect, SortedKeyHandlePositionPairs);

				if (bNext)
				{
					KeyHandleToSelect = SortedKeyHandlePositionPairs[0].Handle;
				}
				else
				{
					KeyHandleToSelect = SortedKeyHandlePositionPairs.Last().Handle;
				}
			}
			return;
		}

		if (ActiveSortedKeyHandlePositionPairs.Num() == 0)
		{
			return;
		}

		// There weren't any keys already selected so just select the first key on the active curve.
		CurveModelIdToSelect = ActiveCurveModelId;
		KeyHandleToSelect = ActiveSortedKeyHandlePositionPairs[0].Handle;
	};

	if (CurveModelIdToSelect.IsSet() &&
		KeyHandleToSelect.IsSet())
	{
		FCurveEditorSelection& CurveEditorSelection = CurveEditor->GetSelection();
		CurveEditorSelection.Clear();
		CurveEditorSelection.Add(CurveModelIdToSelect.GetValue(), ECurvePointType::Key, KeyHandleToSelect.GetValue());

		TArray<FCurveModelID> CurvesToFit;
		CurvesToFit.Add(CurveModelIdToSelect.GetValue());
		CurveEditor->ZoomToFitCurves(CurvesToFit);
	}
}

void SVoxelCurveKeySelector::GetActiveCurveModelAndSelectedKeys(TOptional<FCurveModelID>& OutActiveCurveModelId, TArray<FKeyHandle>& OutSelectedKeyHandles) const
{
	const TMap<FCurveEditorTreeItemID, ECurveEditorTreeSelectionState>& CurveTreeSelection = CurveEditor->GetTree()->GetSelection();
	if (CurveTreeSelection.Num() > 0)
	{
		const TArrayView<const FCurveModelID> CurveModelIds = CurveEditor->GetTreeItem(CurveTreeItemId).GetOrCreateCurves(CurveEditor.Get());
		if (CurveModelIds.Num() == 1)
		{
			OutActiveCurveModelId = CurveModelIds[0];

			if (const FKeyHandleSet* SelectedKeyHandleSet = CurveEditor->GetSelection().GetAll().Find(OutActiveCurveModelId.GetValue()))
			{
				OutSelectedKeyHandles = SelectedKeyHandleSet->AsArray();
			}
		}
		return;
	}

	// Otherwise check if there are keys selected and if so use first curve with selected keys.
	const FCurveEditorSelection& CurveEditorSelection = CurveEditor->GetSelection();
	if (!CurveEditorSelection.IsEmpty())
	{
		const TArrayView<const FCurveModelID> CurveModelIds = CurveEditor->GetTreeItem(CurveTreeItemId).GetCurves();
		if (CurveModelIds.Num() == 1)
		{
			if (const FKeyHandleSet* SelectedKeyHandleSet = CurveEditorSelection.GetAll().Find(CurveModelIds[0]))
			{
				OutActiveCurveModelId = CurveModelIds[0];
				OutSelectedKeyHandles = SelectedKeyHandleSet->AsArray();
			}
		}
		return;
	}

	// Otherwise just use the first pinned curve.
	const TSet<FCurveModelID>& PinnedCurveIds = CurveEditor->GetPinnedCurves();
	if (PinnedCurveIds.Num() > 0)
	{
		const TArrayView<const FCurveModelID> CurveModelIds = CurveEditor->GetTreeItem(CurveTreeItemId).GetOrCreateCurves(CurveEditor.Get());
		if (CurveModelIds.Num() == 1 &&
			PinnedCurveIds.Contains(CurveModelIds[0]))
		{
			OutActiveCurveModelId = CurveModelIds[0];
		}
	}
}

void SVoxelCurveKeySelector::GetSortedKeyHandlessAndPositionsForModel(const FCurveModel& InCurveModel, TArray<FKeyHandlePositionPair>& OutSortedKeyHandlesAndPositions)
{
	TArray<FKeyHandle> KeyHandles;
	InCurveModel.GetKeys(
		*CurveEditor.Get(),
		TNumericLimits<double>::Lowest(),
		TNumericLimits<double>::Max(),
		TNumericLimits<double>::Lowest(),
		TNumericLimits<double>::Max(),
		KeyHandles);

	TArray<FKeyPosition> KeyPositions;
	KeyPositions.AddDefaulted(KeyHandles.Num());
	InCurveModel.GetKeyPositions(KeyHandles, KeyPositions);

	for (int32 Index = 0; Index < KeyHandles.Num(); Index++)
	{
		FKeyHandlePositionPair& KeyHandlePositionPair = OutSortedKeyHandlesAndPositions.AddDefaulted_GetRef();
		KeyHandlePositionPair.Handle = KeyHandles[Index];
		KeyHandlePositionPair.Position = KeyPositions[Index];
	}

	OutSortedKeyHandlesAndPositions.Sort([](const FKeyHandlePositionPair& A, const FKeyHandlePositionPair& B)
	{
		return A.Position.InputValue < B.Position.InputValue;
	});
}

void SVoxelCurveKeySelector::GetOrderedActiveCurveModelIds(TArray<FCurveModelID>& OutOrderedActiveCurveModelIds) const
{
	if (CurveEditor->GetTreeSelection().Num() > 0)
	{
		const TArrayView<const FCurveModelID> CurveModelIds = CurveEditor->GetTreeItem(CurveTreeItemId).GetOrCreateCurves(CurveEditor.Get());
		if (CurveModelIds.Num() == 1)
		{
			OutOrderedActiveCurveModelIds.Add(CurveModelIds[0]);
		}
		return;
	}

	const TArrayView<const FCurveModelID> CurveModelIds = CurveEditor->GetTreeItem(CurveTreeItemId).GetCurves();
	if (CurveModelIds.Num() == 1 &&
		CurveEditor->IsCurvePinned(CurveModelIds[0]))
	{
		OutOrderedActiveCurveModelIds.Add(CurveModelIds[0]);
	}	
}