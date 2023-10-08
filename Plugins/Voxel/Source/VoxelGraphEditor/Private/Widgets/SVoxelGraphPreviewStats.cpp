// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "SVoxelGraphPreviewStats.h"

void SVoxelGraphPreviewStats::Construct(const FArguments& Args)
{
	const TSharedRef<SScrollBar> ScrollBar = SNew(SScrollBar);

	ChildSlot
	[
		SNew(SBorder)
		.BorderImage(FAppStyle::Get().GetBrush("Brushes.Recessed"))
		.Padding(15.0f)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			.Padding(0.f)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.FillWidth(1)
				[
					SNew(SScrollBox)
					.Orientation(Orient_Horizontal)
					+ SScrollBox::Slot()
					[
						SAssignNew(RowsView, SListView<TSharedPtr<FRow>>)
						.ListItemsSource(&Rows)
						.OnGenerateRow(this, &SVoxelGraphPreviewStats::CreateRow)
						.ExternalScrollbar(ScrollBar)
						.ItemHeight(24.0f)
						.ConsumeMouseWheel(EConsumeMouseWheel::Always)

					]
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SBox)
					.WidthOverride(FOptionalSize(16))
					[
						ScrollBar
					]
				]
			]
		]
	];
}

TSharedRef<ITableRow> SVoxelGraphPreviewStats::CreateRow(TSharedPtr<FRow> StatsRow, const TSharedRef<STableViewBase>& OwnerTable) const
{
	return
		SNew(STableRow<TSharedPtr<FRow>>, OwnerTable)
		[
			SNew(SVerticalBox)
			.ToolTipText(StatsRow->Tooltip)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.Padding(2.f)
				[
					SNew(SBox)
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					.WidthOverride(16.f)
					.HeightOverride(16.f)
					[
						SNew(SImage)
						.Image(FAppStyle::GetBrush("Icons.BulletPoint"))
					]
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString(StatsRow->Header.ToString() + ": "))
					.ColorAndOpacity(FSlateColor::UseForeground())
					.TextStyle(FAppStyle::Get(), "MessageLog")
				]
				+ SHorizontalBox::Slot()
				.FillWidth(1.f)
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Center)
				.Padding(0.f, 0.f, 5.f, 0.f)
				[
					SNew(STextBlock)
					.Text(StatsRow->Value)
					.ColorAndOpacity(FSlateColor::UseForeground())
					.TextStyle(FAppStyle::Get(), "MessageLog")
				]
			]
		];
}