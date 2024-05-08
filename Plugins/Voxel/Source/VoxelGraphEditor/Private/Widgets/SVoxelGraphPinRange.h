// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "VoxelPinValue.h"

template<typename PinType, typename InnerType>
class VOXELGRAPHEDITOR_API SVoxelGraphPinRange : public SGraphPin
{
public:
	VOXEL_SLATE_ARGS()
	{
	};

	void Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
	{
		SGraphPin::Construct(SGraphPin::FArguments(), InGraphPinObj);
	}

protected:
	//~ Begin SGraphPin Interface
	virtual TSharedRef<SWidget>	GetDefaultValueWidget() override
	{
		return
			SNew(SVerticalBox)
			.Visibility(this, &SGraphPin::GetDefaultValueVisibility)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.f)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(2.f)
				.HAlign(HAlign_Fill)
				[
					SNew(SNumericEntryBox<InnerType>)
					.LabelVAlign(VAlign_Center)
					.Label()
					[
						SNew(STextBlock)
						.Font(FAppStyle::GetFontStyle("Graph.VectorEditableTextBox"))
						.ColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 0.4f))
						.Text(INVTEXT("Min"))
					]
					.Value(this, &SVoxelGraphPinRange::GetValue, true)
					.OnValueCommitted(this, &SVoxelGraphPinRange::SetValue, true)
					.Font(FAppStyle::GetFontStyle("Graph.VectorEditableTextBox"))
					.UndeterminedString(INVTEXT("Multiple Values"))
					.ToolTipText(INVTEXT("Min value"))
					.EditableTextBoxStyle(&FAppStyle::GetWidgetStyle<FEditableTextBoxStyle>("Graph.VectorEditableTextBox"))
					.BorderForegroundColor(FLinearColor::White)
					.BorderBackgroundColor(FLinearColor::White)
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(2.f)
				.HAlign(HAlign_Fill)
				[
					SNew(SNumericEntryBox<InnerType>)
					.LabelVAlign(VAlign_Center)
					.Label()
					[
						SNew(STextBlock)
						.Font(FAppStyle::GetFontStyle("Graph.VectorEditableTextBox"))
						.ColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 0.4f))
						.Text(INVTEXT("Max"))
					]
					.Value(this, &SVoxelGraphPinRange::GetValue, false)
					.OnValueCommitted(this, &SVoxelGraphPinRange::SetValue, false)
					.Font(FAppStyle::GetFontStyle("Graph.VectorEditableTextBox"))
					.UndeterminedString(INVTEXT("Multiple Values"))
					.ToolTipText(INVTEXT("Max value"))
					.EditableTextBoxStyle(&FAppStyle::GetWidgetStyle<FEditableTextBoxStyle>("Graph.VectorEditableTextBox"))
					.BorderForegroundColor(FLinearColor::White)
					.BorderBackgroundColor(FLinearColor::White)
				]
			];
	}
	//~ End SGraphPin Interface

	TOptional<InnerType> GetValue(const bool bMin) const
	{
		FVoxelPinValue PinValue = GetValue();
		if (!PinValue.IsValid())
		{
			return {};
		}

		return bMin ? PinValue.Get<PinType>().Min : PinValue.Get<PinType>().Max;
	}

	void SetValue(InnerType NewValue, ETextCommit::Type, const bool bMin) const
	{
		if (GraphPinObj->IsPendingKill())
		{
			return;
		}

		FVoxelPinValue PinValue = GetValue();
		PinType& Value = PinValue.Get<PinType>();

		if (bMin)
		{
			if (NewValue == Value.Min)
			{
				return;
			}

			Value.Min = NewValue;
			Value.Max = FMath::Max(Value.Max, NewValue);
		}
		else
		{
			if (NewValue == Value.Max)
			{
				return;
			}

			Value.Max = NewValue;
			Value.Min = FMath::Min(Value.Min, NewValue);
		}

		{
			const FVoxelTransaction Transaction(GraphPinObj);

			GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, PinValue.ExportToString());
		}
	}

private:
	FVoxelPinValue GetValue() const
	{
		if (GraphPinObj->IsPendingKill())
		{
			return {};
		}

		FVoxelPinValue PinValue = FVoxelPinValue::MakeFromPinDefaultValue(*GraphPinObj);
		if (!ensure(PinValue.Is<PinType>()))
		{
			return {};
		}

		return PinValue;
	}
};

using SVoxelGraphPinFloatRange = SVoxelGraphPinRange<FVoxelFloatRange, float>;
using SVoxelGraphPinInt32Range = SVoxelGraphPinRange<FVoxelInt32Range, int32>;