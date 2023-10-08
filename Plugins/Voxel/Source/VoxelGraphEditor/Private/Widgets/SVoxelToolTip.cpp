// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "SVoxelToolTip.h"

VOXEL_INITIALIZE_STYLE(ToolTip)
{
	const FTextBlockStyle NormalText = FAppStyle::GetWidgetStyle<FTextBlockStyle>("NormalText");

	const FTextBlockStyle TooltipText = FTextBlockStyle(NormalText)
		.SetFont(DEFAULT_FONT("Regular", 9))
		.SetColorAndOpacity(FLinearColor::Black);
	Set("Tooltip.NormalText", FTextBlockStyle(TooltipText));

	const FTextBlockStyle TooltipTextSubdued = FTextBlockStyle(NormalText)
		.SetFont(DEFAULT_FONT("Regular", 8))
		.SetColorAndOpacity(FLinearColor(0.1f, 0.1f, 0.1f));
	Set("Tooltip.SubduedText", FTextBlockStyle(TooltipTextSubdued));

	const FButtonStyle TooltipHyperlinkButton = FButtonStyle()
		.SetNormal(BORDER_BRUSH("Old/HyperlinkDotted", FMargin(0.f, 0.f, 0.f, 3.f / 16.f)))
		.SetPressed({})
		.SetHovered(BORDER_BRUSH("Old/HyperlinkUnderline", FMargin(0.f, 0.f, 0.f, 3.f / 16.0f)));

	const FTextBlockStyle TooltipHyperlinkText = FTextBlockStyle(NormalText)
		.SetFont(DEFAULT_FONT("Regular", 9))
		.SetColorAndOpacity(FLinearColor(0.1f, 0.1f, 0.5f));

	const FHyperlinkStyle TooltipHyperlink = FHyperlinkStyle()
		.SetUnderlineStyle(TooltipHyperlinkButton)
		.SetTextStyle(TooltipHyperlinkText)
		.SetPadding(0.0f);
	Set("Hyperlink", TooltipHyperlink);
}

void SVoxelToolTip::Construct(const FArguments& InArgs)
{
	PinType = InArgs._PinType;

	SToolTip::Construct(
		SToolTip::FArguments()
		.Text(InArgs._Text)
		.BorderImage(FCoreStyle::Get().GetBrush("ToolTip.BrightBackground"))
		.TextMargin(11.0f));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void SVoxelToolTip::OnOpening()
{
	SetContentWidget(CreateToolTipWidget());
}

bool SVoxelToolTip::IsInteractive() const
{
	const FModifierKeysState ModifierKeys = FSlateApplication::Get().GetModifierKeys();
	return ModifierKeys.IsAltDown() && ModifierKeys.IsControlDown();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedRef<SWidget> SVoxelToolTip::CreateToolTipWidget() const
{
	FString ToolTip = GetTextTooltip().ToString();
	ToolTip.ReplaceInline(TEXT("href=\"http"), TEXT("href=\"VOXEL_ID"));

	int32 LinksCount = 0;
	{
		int32 StartPosition = 0;
		TArray<FString> Links;
		while (StartPosition != -1)
		{
			StartPosition = ToolTip.Find("http", ESearchCase::IgnoreCase, ESearchDir::FromStart, StartPosition);
			if (StartPosition == -1)
			{
				break;
			}

			int32 SpacePosition = ToolTip.Find(TEXT(" "), ESearchCase::IgnoreCase, ESearchDir::FromStart, StartPosition);
			int32 NewLinePosition = ToolTip.Find(TEXT("\n"), ESearchCase::IgnoreCase, ESearchDir::FromStart, StartPosition);
			int32 NewLineMarkPosition = ToolTip.Find(TEXT("\\n"), ESearchCase::IgnoreCase, ESearchDir::FromStart, StartPosition);

			if (SpacePosition == -1)
			{
				SpacePosition = ToolTip.Len();
			}

			if (NewLinePosition == -1)
			{
				NewLinePosition = ToolTip.Len();
			}

			if (NewLineMarkPosition == -1)
			{
				NewLineMarkPosition = ToolTip.Len();
			}

			const int32 EndPosition = FMath::Min3(SpacePosition, NewLinePosition, NewLineMarkPosition);

			Links.Add(ToolTip.Mid(StartPosition, EndPosition - StartPosition));

			StartPosition = EndPosition;
		}

		for (const FString& Link : Links)
		{
			FString LinkName = Link;
			LinkName.RemoveFromStart("https://");
			LinkName.RemoveFromStart("http://");
			LinksCount += ToolTip.ReplaceInline(*Link, *("<a id=\"browser\" href=\"" + Link + "\">" + LinkName + "</>"));
		}
	}

	LinksCount += ToolTip.ReplaceInline(TEXT("<a href=\"VOXEL_ID"), TEXT("<a id=\"browser\" href=\"http"));

	TSharedRef<SVerticalBox> Box =
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SRichTextBlock)
			.Text(FText::FromString(ToolTip))
			.TextStyle(&FVoxelEditorStyle::GetWidgetStyle<FTextBlockStyle>("Tooltip.NormalText"))
			.WrapTextAt_Static(&SToolTip::GetToolTipWrapWidth)
			.DecoratorStyleSet(&FVoxelEditorStyle::Get())
			+ SRichTextBlock::HyperlinkDecorator("browser", FSlateHyperlinkRun::FOnClick::CreateLambda([](const FSlateHyperlinkRun::FMetadata& M)
			{
				if (const FString* UrlPtr = M.Find(TEXT("href")))
				{
					FPlatformProcess::LaunchURL(**UrlPtr, nullptr, nullptr);
				}
			}))
		];

	if (PinType.IsSet())
	{
		Box->AddSlot()
		.Padding(0.f, 5.f, 0.f, 0.f)
		.AutoHeight()
		[
			SNew(STextBlock)
			.TextStyle(&FVoxelEditorStyle::GetWidgetStyle<FTextBlockStyle>("Tooltip.SubduedText"))
			.Text(FText::FromString("Type: " + PinType.Get().ToString()))
		];
	}

	if (LinksCount > 0)
	{
		Box->AddSlot()
		.Padding(0.f, 10.f, 0.f, 0.f)
		.AutoHeight()
		.HAlign(HAlign_Center)
		[
			SNew(STextBlock)
			.TextStyle(&FVoxelEditorStyle::GetWidgetStyle<FTextBlockStyle>("Tooltip.SubduedText"))
			.Text(FText::FromString("Hold (Ctrl + Alt) to interact with links"))
		];
	}

	return Box;
}