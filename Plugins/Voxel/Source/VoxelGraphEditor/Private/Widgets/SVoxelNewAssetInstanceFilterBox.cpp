// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "SVoxelNewAssetInstanceFilterBox.h"

TMap<EVoxelGraphScriptSource, bool> SVoxelNewAssetInstanceSourceFilterBox::SourceState
{
	{ EVoxelGraphScriptSource::Voxel, true },
	{ EVoxelGraphScriptSource::VoxelExamples, true },
	{ EVoxelGraphScriptSource::Game, true },
	{ EVoxelGraphScriptSource::Plugins, false },
	{ EVoxelGraphScriptSource::Developer, false },
};

void SVoxelNewAssetInstanceSourceFilterCheckBox::Construct(const FArguments& Args, EVoxelGraphScriptSource InSource)
{
	Source = InSource;
	OnSourceStateChanged = Args._OnSourceStateChanged;
	OnShiftClicked = Args._OnShiftClicked;

	const UEnum* ScriptSourceEnum = StaticEnum<EVoxelGraphScriptSource>();

	SCheckBox::Construct(
		SCheckBox::FArguments()
		.Style(FAppStyle::Get(), "ContentBrowser.FilterButton")
		.IsChecked(Args._IsChecked)
		.OnCheckStateChanged(FOnCheckStateChanged::CreateLambda([this](const ECheckBoxState NewState)
		{
			OnSourceStateChanged.ExecuteIfBound(Source, NewState == ECheckBoxState::Checked);
		})));

	SetToolTipText(FText::FromString("Display actions from source: " + ScriptSourceEnum->GetAuthoredNameStringByIndex(int64(Source)) + """.\nUse Shift+Click to exclusively select this filter."));

	SetContent(
		SNew(SBorder)
		.Padding(1.f)
		.BorderImage(FVoxelEditorStyle::GetBrush("Graph.NewAssetDialog.FilterCheckBox.Border"))
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.AutoWidth()
			[
				SNew(SImage)
				.Image(FAppStyle::Get().GetBrush("ContentBrowser.FilterImage"))
				.ColorAndOpacity(this, &SVoxelNewAssetInstanceSourceFilterCheckBox::GetScriptSourceColor)
			]
			+ SHorizontalBox::Slot()
			.Padding(MakeAttributeSP(this, &SVoxelNewAssetInstanceSourceFilterCheckBox::GetFilterNamePadding))
			.VAlign(VAlign_Center)
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(ScriptSourceEnum->GetDisplayNameTextByIndex(int64(Source)))
				.ColorAndOpacity(this, &SVoxelNewAssetInstanceSourceFilterCheckBox::GetTextColor)
				.TextStyle(FVoxelEditorStyle::Get(), "Graph.NewAssetDialog.ActionFilterTextBlock")
			]
		]
	);
}

FReply SVoxelNewAssetInstanceSourceFilterCheckBox::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FReply Reply = SCheckBox::OnMouseButtonUp(MyGeometry, MouseEvent);

	if (FSlateApplication::Get().GetModifierKeys().IsShiftDown() &&
		MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		OnShiftClicked.ExecuteIfBound(Source, !IsChecked());
		return FReply::Handled().ReleaseMouseCapture();
	}

	return Reply;
}

FSlateColor SVoxelNewAssetInstanceSourceFilterCheckBox::GetTextColor() const
{
	if (IsHovered())
	{
		constexpr float DimFactor = 0.75f;
		return FLinearColor(DimFactor, DimFactor, DimFactor, 1.f);
	}

	return IsChecked() ? FLinearColor::White : FLinearColor::Gray;
}

FSlateColor SVoxelNewAssetInstanceSourceFilterCheckBox::GetScriptSourceColor() const
{
	FLinearColor ScriptSourceColor;
	switch (Source)
	{
	case EVoxelGraphScriptSource::VoxelExamples: ScriptSourceColor = FLinearColor(0.494792f, 0.445313f, 0.445313f, 1.f); break;
	case EVoxelGraphScriptSource::Game: ScriptSourceColor = FLinearColor(0.510417f, 0.300158f, 0.051042f, 1.f); break;
	case EVoxelGraphScriptSource::Plugins: ScriptSourceColor = FLinearColor(0.048438f, 0.461547f, 0.484375f, 1.f); break;
	case EVoxelGraphScriptSource::Developer: ScriptSourceColor = FLinearColor(0.070312f, 0.46875f, 0.100547f, 1.f); break;
	default: ScriptSourceColor = FLinearColor(1.f, 1.f, 1.f, 0.3); break;
	}

	if (!IsChecked())
	{
		ScriptSourceColor.A = 0.1f;
	}

	return ScriptSourceColor;
}

FMargin SVoxelNewAssetInstanceSourceFilterCheckBox::GetFilterNamePadding() const
{
	return bIsPressed ? FMargin(2.f, 2.f, 1.f, 0.f) : FMargin(2.f, 1.f, 1.f, 1.f);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void SVoxelNewAssetInstanceSourceFilterBox::Construct(const FArguments& Args)
{
	OnFiltersChanged = Args._OnFiltersChanged;

	const TSharedRef<SWrapBox> SourceContainer = SNew(SWrapBox).UseAllottedSize(true);

	SourceContainer->AddSlot()
	.Padding(5.f)
	[
		SNew(SBorder)
        .BorderImage(FVoxelEditorStyle::GetBrush("Graph.NewAssetDialog.FilterCheckBox.Border"))
        .ToolTipText(INVTEXT("Show all"))
        .Padding(3.f)
		[
			SNew(SCheckBox)
            .Style(FVoxelEditorStyle::Get(), "Graph.NewAssetDialog.FilterCheckBox")
            .BorderBackgroundColor_Lambda([=]() -> FSlateColor
			{
			    for (int32 SourceIndex = 0; SourceIndex < int32(EVoxelGraphScriptSource::Unknown); SourceIndex++)
			    {
			    	if (!IsFilterActive(SourceIndex))
			    	{
			    		return FSlateColor::UseForeground();
			    	}
			    }

			    return FLinearColor::White;
			})
            .IsChecked_Lambda([=]() -> ECheckBoxState
			{
			    for (int32 SourceIndex = 0; SourceIndex < int32(EVoxelGraphScriptSource::Unknown); SourceIndex++)
			    {
			    	if (!IsFilterActive(SourceIndex))
			    	{
			    		return ECheckBoxState::Unchecked;
			    	}
			    }

			    return ECheckBoxState::Checked;
			})
            .OnCheckStateChanged(FOnCheckStateChanged::CreateLambda([=](ECheckBoxState NewState)
			{
			    bool bAnyChange = false;
			    for (int32 SourceIndex = 0; SourceIndex < int32(EVoxelGraphScriptSource::Unknown); SourceIndex++)
			    {
			        if (!IsFilterActive(SourceIndex))
			        {
			            bAnyChange = true;
			        	break;
			        }
			    }

			    if (bAnyChange)
			    {
			        for (int32 SourceIndex = 0; SourceIndex < int32(EVoxelGraphScriptSource::Unknown); SourceIndex++)
			        {
			            SourceState.Add(GetScriptSource(SourceIndex), true);
			        }

			        BroadcastFiltersChanged();
			    }
			}))
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.Padding(2.f)
				[
					SNew(STextBlock)
                    .Text(INVTEXT("Show all"))
                    .ShadowOffset(0.f)
                    .ColorAndOpacity_Lambda([=]() -> FSlateColor
					{
					    for (int32 SourceIndex = 0; SourceIndex < int32(EVoxelGraphScriptSource::Unknown); SourceIndex++)
					    {
					    	if (!IsFilterActive(SourceIndex))
					    	{
					    		return FLinearColor::Gray;
					    	}
					    }

					    return FLinearColor::Black;
					})
                    .TextStyle(FVoxelEditorStyle::Get(), "Graph.NewAssetDialog.ActionFilterTextBlock")
				]
			]
		]
	];

	for (int32 SourceIndex = 0; SourceIndex < int32(EVoxelGraphScriptSource::Unknown); SourceIndex++)
	{
		if (SourceIndex == int32(EVoxelGraphScriptSource::Voxel))
		{
			continue;
		}

		SourceContainer->AddSlot()
		.Padding(2.f)
		[
			SNew(SBorder)
    		.BorderImage(FAppStyle::GetBrush(TEXT("NoBorder")))
			.Padding(3.f)
			[
				SNew(SVoxelNewAssetInstanceSourceFilterCheckBox, SVoxelNewAssetInstanceSourceFilterBox::GetScriptSource(SourceIndex))
    			.IsChecked_Lambda([=]
				{
				    return IsFilterActive(SourceIndex) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
				})
    			.OnSourceStateChanged_Lambda([this](const EVoxelGraphScriptSource Source, const bool bState)
				{
				    SourceState.Add(Source, bState);
				    BroadcastFiltersChanged();
				})
    			.OnShiftClicked_Lambda([this](const EVoxelGraphScriptSource ChangedSource, bool bState)
				{
				    TArray<EVoxelGraphScriptSource> Keys;
				    SourceState.GenerateKeyArray(Keys);

				    for (EVoxelGraphScriptSource& Source : Keys)
				    {
				        SourceState.Add(Source, Source == ChangedSource);
				    }

				    BroadcastFiltersChanged();
				})
			]
		];
	}

	ChildSlot
	[
		SourceContainer
	];
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void SVoxelNewAssetInstanceFilterBox::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(3.f, 1.f)
		[
			SNew(STextBlock)
			.Text(INVTEXT("Source Filtering"))
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(3.f, 1.f)
		[
			SAssignNew(SourceFilterBox, SVoxelNewAssetInstanceSourceFilterBox)
			.OnFiltersChanged(InArgs._OnSourceFiltersChanged)
		]
	];
}

bool SVoxelNewAssetInstanceFilterBox::IsSourceFilterActive(const FAssetData& Item) const
{
	return
		SourceFilterBox.IsValid() &&
		SourceFilterBox->IsFilterActive(GetScriptSource(Item));
}

EVoxelGraphScriptSource SVoxelNewAssetInstanceFilterBox::GetScriptSource(const FAssetData& ScriptAssetData)
{
	TArray<FString> AssetPathParts;
	ScriptAssetData.GetSoftObjectPath().ToString().ParseIntoArray(AssetPathParts, TEXT("/"));

	if (!ensure(AssetPathParts.Num() != 0))
	{
		return EVoxelGraphScriptSource::Unknown;
	}

	if (AssetPathParts[0] == "Voxel")
	{
		return EVoxelGraphScriptSource::Voxel;
	}
	else if (AssetPathParts[0] == "Game")
	{
		if (AssetPathParts.Num() > 1)
		{
			if (AssetPathParts[1] == "VoxelExamples")
			{
				return EVoxelGraphScriptSource::VoxelExamples;
			}
			else if (AssetPathParts[1] == "Developers")
			{
				return EVoxelGraphScriptSource::Developer;
			}
			else
			{
				return EVoxelGraphScriptSource::Game;
			}
		}
	}

	return EVoxelGraphScriptSource::Plugins;
}