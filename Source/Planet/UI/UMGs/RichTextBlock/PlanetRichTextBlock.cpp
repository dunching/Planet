#include "PlanetRichTextBlock.h"

#include "Blueprint/UserWidget.h"
#include "Components/Button.h"

#include "TemplateHelper.h"
#include "Components/TextBlock.h"

struct FURichTextBlock_Button_Helper : public TStructVariable<FURichTextBlock_Button_Helper>
{
	FName Btn = TEXT("Btn");

	FName Text = TEXT("Text");
};

class FRichInline_Button : public FRichTextDecorator
{
public:
	using RichTextBlockType = URichTextBlock_Button;

	FRichInline_Button(
		URichTextBlock_Button* InOwner,
		URichTextBlockButtonDecorator* InDecorator
	) :
	  FRichTextDecorator(InOwner)
	  , Decorator(InDecorator)
	{
		MyRichTextBlock = InOwner;
	}

	virtual bool Supports(
		const FTextRunParseResults& RunParseResult,
		const FString& Text
	) const override
	{
		if (
			RunParseResult.Name == TEXT("Button")
			&& RunParseResult.MetaData.Contains(TEXT("Display"))
			&& RunParseResult.MetaData.Contains(TEXT("Param"))
		)
		{
			const FTextRange& IdRange = RunParseResult.MetaData[TEXT("Display")];
			const FString TagId = Text.Mid(IdRange.BeginIndex, IdRange.EndIndex - IdRange.BeginIndex);

			return true;
		}

		return false;
	}

protected:
	virtual TSharedPtr<SWidget> CreateDecoratorWidget(
		const FTextRunInfo& RunInfo,
		const FTextBlockStyle& TextStyle
	) const override
	{
		auto WidgetPtr = CreateWidget<URichTextBlock_Button_Helper>(MyRichTextBlock, Decorator->UserWidgetClass);
		if (WidgetPtr)
		{
			const auto Display = RunInfo.MetaData[TEXT("Display")];
			const auto Param = RunInfo.MetaData[TEXT("Param")];

			WidgetPtr->SetText(Display);

			WidgetPtr->OnClickedDelegate.AddUObject(MyRichTextBlock, &RichTextBlockType::OnBtnClicked, Param);
			WidgetPtr->OnHoveredDelegate.AddUObject(MyRichTextBlock, &RichTextBlockType::OnBtnHovered, Param);

			return WidgetPtr->TakeWidget();
		}

		return TSharedPtr<SWidget>();
	}

private:
	URichTextBlockButtonDecorator* Decorator = nullptr;

	URichTextBlock_Button* MyRichTextBlock = nullptr;
};

/////////////////////////////////////////////////////
// URichTextBlockImageDecorator

void URichTextBlock_Button::OnBtnClicked(
	FString Text_
)
{
	OnClickedDelegate.Broadcast(Text_);
}

void URichTextBlock_Button::OnBtnHovered(
	FString Text_
)
{
	OnHoveredDelegate.Broadcast(Text_);
}

void URichTextBlock_Button_Helper::NativeConstruct()
{
	Super::NativeConstruct();
	
	auto UIPtr = Cast<UButton>(GetWidgetFromName(FURichTextBlock_Button_Helper::Get().Btn));
	if (UIPtr)
	{
		UIPtr->OnClicked.AddDynamic(this, &ThisClass::OnClicked);
		UIPtr->OnHovered.AddDynamic(this, &ThisClass::OnHovered);
	}
}

void URichTextBlock_Button_Helper::SetText(
	const FString& Text
)
{
	auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FURichTextBlock_Button_Helper::Get().Text));
	if (UIPtr)
	{
		UIPtr->SetText(FText::FromString(Text));
	}
}

void URichTextBlock_Button_Helper::OnClicked()
{
	OnClickedDelegate.Broadcast();
}

void URichTextBlock_Button_Helper::OnHovered()
{
	OnHoveredDelegate.Broadcast();
}

URichTextBlockButtonDecorator::URichTextBlockButtonDecorator(
	const FObjectInitializer& ObjectInitializer
) :
  Super(ObjectInitializer)
{
}

TSharedPtr<ITextDecorator> URichTextBlockButtonDecorator::CreateDecorator(
	URichTextBlock* InOwner
)
{
	return MakeShareable(new FRichInline_Button(Cast<URichTextBlock_Button>(InOwner), this));
}
