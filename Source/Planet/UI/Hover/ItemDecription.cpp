
#include "ItemDecription.h"

#include "ItemProxy_Description.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetStringLibrary.h"

struct FUItemDecription : public TStructVariable<FUItemDecription>
{
	const FName Title = TEXT("Title");
	
	const FName Text = TEXT("Text");
};

inline void UItemDecription::BindData(
	const TSharedPtr<FBasicProxy>& InProxySPtr,
	const TSoftObjectPtr<UItemProxy_Description>& InItemProxy_Description
)
{
	ProxySPtr = InProxySPtr;
	ItemProxy_Description = InItemProxy_Description;
}

void UItemDecription::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (!ProxySPtr)
	{
		return;	
	}

	auto ItemProxy_DescriptionPtr = ItemProxy_Description.LoadSynchronous();
	if (ItemProxy_DescriptionPtr && !ItemProxy_DescriptionPtr->DecriptionText.IsEmpty())
	{
		FString Text = ItemProxy_DescriptionPtr->DecriptionText[0];
		for (const auto& Iter : ItemProxy_DescriptionPtr->Values)
		{
			if (Iter.Value.PerLevelValue.IsEmpty())
			{
				continue;
			}
			
			Text = Text.Replace(*Iter.Key, *UKismetStringLibrary::Conv_IntToString(Iter.Value.PerLevelValue[0]));
		}
	
		auto WidgetPtr = Cast<UTextBlock>(GetWidgetFromName(FUItemDecription::Get().Text));
		if (WidgetPtr)
		{
			WidgetPtr->SetText(FText::FromString(Text));
		}
	}
}
