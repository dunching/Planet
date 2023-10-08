// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "SVoxelAddContentTile.h"
#include "Internationalization/BreakIterator.h"

void SVoxelAddContentTile::Construct(const FArguments& InArgs)
{
	Image = InArgs._Image;
	Text = InArgs._DisplayName;
	IsSelected = InArgs._IsSelected;

	ChildSlot
	[
		SNew(SBorder)
		.Padding(FMargin(0.f, 0.f, 5.f, 5.f))
		.BorderImage(FAppStyle::Get().GetBrush("ProjectBrowser.ProjectTile.DropShadow"))
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SScaleBox)
					.Stretch(EStretch::ScaleToFitX)
					[
						SNew(SImage)
						.Image(Image)
						.DesiredSizeOverride(InArgs._ImageSize)
					]
				]
				+ SVerticalBox::Slot()
				[
					SNew(SBorder)
					.Padding(InArgs._ThumbnailPadding)
					.VAlign(VAlign_Top)
					.Padding(FMargin(3.f, 3.f))
					.BorderImage(FAppStyle::Get().GetBrush("ProjectBrowser.ProjectTile.NameAreaBackground"))
					[
						SNew(STextBlock)
						.Font(FAppStyle::Get().GetFontStyle("ProjectBrowser.ProjectTile.Font"))
						.AutoWrapText(true)
						.LineBreakPolicy(FBreakIterator::CreateWordBreakIterator())
						.Text(InArgs._DisplayName)
						.ColorAndOpacity(FAppStyle::Get().GetSlateColor("Colors.Foreground"))
					]
				]
			]
			+ SOverlay::Slot()
			[
				SNew(SImage)
				.Visibility(EVisibility::HitTestInvisible)
				.Image_Lambda([this]
					{
						const bool bSelected = IsSelected.Get();
						const bool bHovered = IsHovered();

						if (bSelected && bHovered)
						{
							static const FName SelectedHover("ProjectBrowser.ProjectTile.SelectedHoverBorder");
							return FAppStyle::Get().GetBrush(SelectedHover);
						}
						else if (bSelected)
						{
							static const FName Selected("ProjectBrowser.ProjectTile.SelectedBorder");
							return FAppStyle::Get().GetBrush(Selected);
						}
						else if (bHovered)
						{
							static const FName Hovered("ProjectBrowser.ProjectTile.HoverBorder");
							return FAppStyle::Get().GetBrush(Hovered);
						}

						return FStyleDefaults::GetNoBrush();
					}
				)
			]
		]
	];
}