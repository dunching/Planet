// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "CurveDataAbstraction.h"

class FCurveModel;
class FCurveEditor;
class SCurveEditorTree;
struct FCurveModelID;

class SVoxelCurveKeySelector : public SCompoundWidget
{
public:
	VOXEL_SLATE_ARGS()
	{
	};

	void Construct(const FArguments& InArgs, const TSharedPtr<FCurveEditor>& InCurveEditor, const FCurveEditorTreeItemID& InCurveTreeItemId);

private:
	FReply ZoomToFitClicked() const;
	FReply PreviousKeyClicked();
	FReply NextKeyClicked();
	FReply AddKeyClicked();
	FReply DeleteKeyClicked() const;

private:
	void NavigateToAdjacentKey(bool bNext);

	void GetActiveCurveModelAndSelectedKeys(TOptional<FCurveModelID>& OutActiveCurveModelId, TArray<FKeyHandle>& OutSelectedKeyHandles) const;

	struct FKeyHandlePositionPair
	{
		FKeyHandle Handle;
		FKeyPosition Position;
	};

	void GetSortedKeyHandlessAndPositionsForModel(const FCurveModel& InCurveModel, TArray<FKeyHandlePositionPair>& OutSortedKeyHandlesAndPositions);
	void GetOrderedActiveCurveModelIds(TArray<FCurveModelID>& OutOrderedActiveCurveModelIds) const;

private:
	TSharedPtr<FCurveEditor> CurveEditor;
	FCurveEditorTreeItemID CurveTreeItemId;
	TSharedPtr<SCurveEditorTree> CurveEditorTree;
};