// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "SVoxelGraphPreviewDepthSlider.h"

void SVoxelGraphPreviewDepthSlider::Construct(const FArguments& InArgs)
{
	ValueText = InArgs._ValueText;
	Value = InArgs._Value;
	OnValueChanged = InArgs._OnValueChanged;
	MinValue = InArgs._MinValue;
	MaxValue = InArgs._MaxValue;

	TypeInterface = MakeVoxelShared<TNumericUnitTypeInterface<float>>(EUnit::Meters);
	TypeInterface->SetMinFractionalDigits(2);
	TypeInterface->SetMaxFractionalDigits(2);

	Options.SetMinimumFractionalDigits(2);
	Options.SetMaximumFractionalDigits(2);

	CreateMinMaxValue(true);
	CreateMinMaxValue(false);

	ChildSlot
	[
		SNew(SBox)
		.Padding(6.f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				[
					SNew(SVoxelDetailText)
					.Text(INVTEXT("Min"))
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SBox)
					.HAlign(HAlign_Fill)
					.MinDesiredWidth(70.f)
					[
						MinValueBox.ToSharedRef()
					]
				]
			]
			+ SHorizontalBox::Slot()
			.FillWidth(0.9f)
			[
				SNew(SBox)
				.MinDesiredWidth(200.f)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Bottom)
					.AutoHeight()
					[
						SNew(SVoxelDetailText)
						.Text(this, &SVoxelGraphPreviewDepthSlider::GetValueText)
					]
					+ SVerticalBox::Slot()
					.FillHeight(1.f)
					[
						SAssignNew(Slider, SSlider)
						.ToolTip(GetToolTip())
						.Orientation(Orient_Horizontal)
						.MinValue(MinValue)
						.MaxValue(MaxValue)
						.StepSize(1.f)
						.Value(Value)
						.OnValueChanged_Lambda([this](float NewValue)
						{
							OnValueChanged.ExecuteIfBound(NewValue);
							Value = NewValue;
						})
					]
				]
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				[
					SNew(SVoxelDetailText)
					.Text(INVTEXT("Max"))
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SBox)
					.HAlign(HAlign_Fill)
					.MinDesiredWidth(70.f)
					[
						MaxValueBox.ToSharedRef()
					]
				]
			]
		]
	];
}

void SVoxelGraphPreviewDepthSlider::ResetValue(float NewValue, bool bKeepRange)
{
	if (NewValue < 100.f &&
		NewValue > -100.f)
	{
		Value = NewValue;
		if (bKeepRange)
		{
			const float HalfRange = (MaxValue - MinValue) / 2.f;
			MinValue = NewValue - HalfRange;
			MaxValue = NewValue + HalfRange;
		}
		else
		{
			MinValue = -100.f;
			MaxValue = 100.f;
		}
	}
	else
	{
		Value = NewValue;
		if (bKeepRange)
		{
			const float HalfRange = (MaxValue - MinValue) / 2.f;
			MinValue = NewValue - HalfRange;
			MaxValue = NewValue + HalfRange;
		}
		else
		{
			const int32 Power = FMath::Max(2, FMath::FloorToInt(FMath::LogX(10.f, FMath::Abs(Value))));
			MinValue = NewValue - FMath::Pow(10.f, Power);
			MaxValue = NewValue + FMath::Pow(10.f, Power);
		}
	}

	Slider->SetValue(Value);
	Slider->SetMinAndMaxValues(MinValue, MaxValue);
}

FText SVoxelGraphPreviewDepthSlider::GetValueText() const
{
	return FText::FromString(ValueText.ToString() + ": " + FText::AsNumber(Value / 100.f, &Options).ToString() + " m");
}

void SVoxelGraphPreviewDepthSlider::CreateMinMaxValue(bool bMin)
{
	SAssignNew((bMin ? MinValueBox : MaxValueBox), SNumericEntryBox<float>)
      	.Justification(ETextJustify::Center)
      	.ToolTip(GetToolTip())
      	.Value(this, &SVoxelGraphPreviewDepthSlider::GetMinMaxValue, bMin)
		.TypeInterface(TypeInterface)
      	.OnValueCommitted(this, &SVoxelGraphPreviewDepthSlider::UpdateRange, bMin);
}

TOptional<float> SVoxelGraphPreviewDepthSlider::GetMinMaxValue(const bool bMin) const
{
	return (bMin ? MinValue : MaxValue) / 100.f;
}

void SVoxelGraphPreviewDepthSlider::UpdateRange(float NewMinMaxValue, ETextCommit::Type, const bool bMin)
{
	NewMinMaxValue *= 100.f;

	float& TargetValue = bMin ? MinValue : MaxValue;
	float& OpposingValue = bMin ? MaxValue : MinValue;

	TargetValue = NewMinMaxValue;
	if (MaxValue < MinValue)
	{
		OpposingValue = TargetValue;
	}

	Slider->SetMinAndMaxValues(MinValue, MaxValue);

	const float NewValue = FMath::Clamp(Value, MinValue, MaxValue);
	if (NewValue != Value)
	{
		Value = NewValue;
		Slider->SetValue(Value);
		OnValueChanged.ExecuteIfBound(NewValue);
	}
}