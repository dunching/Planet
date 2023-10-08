#include "PromptStr.h"
#include "Components/TextBlock.h"

void UPromptStr::SetPromptStr(const FString& StrVal)
{
	auto TextPtr = Cast<UTextBlock>(GetWidgetFromName(TEXT("PromptStr")));
	if (TextPtr)
	{
		TextPtr->SetText(FText::FromString(StrVal));
	}
}
