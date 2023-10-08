// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "SVoxelCurveTemplateBar.h"
#include "CurveEditor.h"
#include "SVoxelCurveThumbnail.h"
#include "NiagaraEditorSettings.h"

void SVoxelCurveTemplateBar::Construct(const FArguments& InArgs, const TSharedRef<FCurveEditor>& InCurveEditor)
{
	CurveEditor = InCurveEditor;

	FToolBarBuilder ToolBarBuilder(CurveEditor->GetCommands(), FMultiBoxCustomization::None, nullptr, true);
	ToolBarBuilder.SetLabelVisibility(EVisibility::Collapsed);

	ToolBarBuilder.AddWidget(
		SNew(SBox)
		.VAlign(VAlign_Center)
		.Padding(0.f, 0.f, 5.f, 0.f)
		[
			SNew(STextBlock)
			.Text(INVTEXT("Templates"))
		]);

	for (const FNiagaraCurveTemplate& CurveTemplate : GetDefault<UNiagaraEditorSettings>()->GetCurveTemplates())
	{
		UCurveFloat* FloatCurveAsset = Cast<UCurveFloat>(CurveTemplate.CurveAsset.TryLoad());
		if (!FloatCurveAsset)
		{
			continue;
		}

		ToolBarBuilder.AddWidget(
			SNew(SButton)
			.ButtonStyle(FAppStyle::Get(), "SimpleButton")
			.OnClicked(this, &SVoxelCurveTemplateBar::CurveTemplateClicked, TWeakObjectPtr<UCurveFloat>(FloatCurveAsset))
			.ToolTipText(FText::FromString(FName::NameToDisplayString(FloatCurveAsset->GetName(), false) + "\nClick to apply this template to the selected curves."))
			.ContentPadding(3.f)
			.Content()
			[
				SNew(SVoxelCurveThumbnail, &FloatCurveAsset->FloatCurve)
			]);
	}

	ChildSlot
	[
		ToolBarBuilder.MakeWidget()
	];
}

FReply SVoxelCurveTemplateBar::CurveTemplateClicked(TWeakObjectPtr<UCurveFloat> WeakFloatCurveAsset) const
{
	const UCurveFloat* FloatCurveAsset = WeakFloatCurveAsset.Get();
	if (!ensure(FloatCurveAsset))
	{
		return FReply::Handled();
	}

	TArray<FCurveModelID> CurveModelIdsToSet;
	if (CurveEditor->GetRootTreeItems().Num() == 1)
	{
		const FCurveEditorTreeItem& TreeItem = CurveEditor->GetTreeItem(CurveEditor->GetRootTreeItems()[0]);
		for(const FCurveModelID& CurveModelId : TreeItem.GetCurves())
		{
			CurveModelIdsToSet.Add(CurveModelId);
		}
	}
	else
	{
		for (const TPair<FCurveEditorTreeItemID, ECurveEditorTreeSelectionState>& TreeItemSelectionState : CurveEditor->GetTreeSelection())
		{
			if (TreeItemSelectionState.Value != ECurveEditorTreeSelectionState::None)
			{
				const FCurveEditorTreeItem& TreeItem = CurveEditor->GetTreeItem(TreeItemSelectionState.Key);
				for (const FCurveModelID& CurveModelId : TreeItem.GetCurves())
				{
					CurveModelIdsToSet.Add(CurveModelId);
				}
			}
		}
	}

	if (CurveModelIdsToSet.Num() == 0)
	{
		return FReply::Handled();
	}

	for (const FCurveModelID& CurveModelId : CurveModelIdsToSet)
	{
		const TUniquePtr<FCurveModel>* CurveModelPtr = CurveEditor->GetCurves().Find(CurveModelId);
		if (!CurveModelPtr)
		{
			continue;
		}

		FCurveModel* CurveModel = CurveModelPtr->Get();
		if (!CurveModel)
		{
			continue;
		}

		TArray<FKeyHandle> KeyHandles;
		CurveModel->GetKeys(*CurveEditor.Get(), TNumericLimits<double>::Lowest(), TNumericLimits<double>::Max(), TNumericLimits<double>::Lowest(), TNumericLimits<double>::Max(), KeyHandles);
		CurveModel->RemoveKeys(KeyHandles);

		for (const FRichCurveKey& Key : FloatCurveAsset->FloatCurve.Keys)
		{
			FKeyPosition KeyPosition;
			KeyPosition.InputValue = Key.Time;
			KeyPosition.OutputValue = Key.Value;

			FKeyAttributes KeyAttributes;
			KeyAttributes.SetInterpMode(Key.InterpMode);
			KeyAttributes.SetTangentMode(Key.TangentMode);
			KeyAttributes.SetArriveTangent(Key.ArriveTangent);
			KeyAttributes.SetLeaveTangent(Key.LeaveTangent);

			CurveModel->AddKey(KeyPosition, KeyAttributes);
		}
	}
	CurveEditor->ZoomToFit();

	return FReply::Handled();
}