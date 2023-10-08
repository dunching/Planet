
#include "PromptBox.h"

#include "Components/TextBlock.h"
#include <Kismet/GameplayStatics.h>

#include "PromptStr.h"
#include "Components/VerticalBox.h"
#include <AssetRefrencePath.h>
#include "AssetRefMap.h"

void UPromptBox::SetPromptStr(const TArray<FString>& TextAry)
{
	auto VerticalBoxPtr = Cast<UVerticalBox>(GetWidgetFromName(TEXT("PromtStrBox")));
	if (VerticalBoxPtr)
	{
		VerticalBoxPtr->ClearChildren();

		auto WidgetClass = UAssetRefMap::GetInstance()->PromptStrClass;

		if (WidgetClass)
		{
			for (auto Iter : TextAry)
			{
				auto PromptStrPtr = CreateWidget<UPromptStr>(GetWorld(), WidgetClass);
				if (PromptStrPtr)
				{
					PromptStrPtr->SetPromptStr(Iter);
					VerticalBoxPtr->AddChild(PromptStrPtr);
				}
			}
		}
	}

}
