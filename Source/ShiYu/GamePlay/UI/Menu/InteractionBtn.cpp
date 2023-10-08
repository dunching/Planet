
#include "InteractionBtn.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

UInteractionBtn::UInteractionBtn(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{

}

void UInteractionBtn::SetEvent(const std::function<void()>& NewClickEvent)
{
	ClickEvent = NewClickEvent;
}

void UInteractionBtn::SetTitle(const FString& Title)
{
	auto TextPtr = Cast<UTextBlock>(GetWidgetFromName(TEXT("Text")));
	if (TextPtr)
	{
		TextPtr->SetText(FText::FromString(Title));
	}
}

void UInteractionBtn::NativeConstruct()
{
	Super::NativeConstruct();

	auto BtnPtr = Cast<UButton>(GetWidgetFromName(TEXT("Btn")));
	if (BtnPtr)
	{
		BtnPtr->OnClicked.AddDynamic(this, &UInteractionBtn::BtnClick);
	}
}

void UInteractionBtn::BtnClick()
{
	if (ClickEvent)
	{
		ClickEvent();
	}
}

