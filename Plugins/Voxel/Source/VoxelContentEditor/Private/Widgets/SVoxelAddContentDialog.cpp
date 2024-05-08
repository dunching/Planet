// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "SVoxelAddContentDialog.h"
#include "SVoxelAddContentWidget.h"

void SVoxelAddContentDialog::Construct(const FArguments& InArgs)
{
	const TSharedRef<SBox> Box =
		SNew(SBox)
		[
			SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(SThrobber)
			]
		];

	SWindow::Construct(SWindow::FArguments()
		.Title(INVTEXT("Add Voxel Plugin example content to the project"))
		.SizingRule(ESizingRule::UserSized)
		.ClientSize(FVector2D(900.f, 500.f))
		.SupportsMinimize(false)
		.SupportsMaximize(false)
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("Brushes.Panel"))
			.Padding(FMargin(10.f,0.f))
			[
				Box
			]
		]);

	FVoxelExampleContentManager::Get().OnExamplesReady(MakeWeakPtrDelegate(Box, [&Box = Box.Get()]
	{
		Box.SetContent(SNew(SVoxelAddContentWidget));
	}));
}