#include "ItemDecription.h"

#include "ItemProxy_Description.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetStringLibrary.h"

struct FUItemDecription : public TStructVariable<FUItemDecription>
{
	const FName Title = TEXT("Title");

	const FName Text = TEXT("Text");
};

UItemDecription::UItemDecription(
	const FObjectInitializer& ObjectInitializer
)
{
}

inline void UItemDecription::BindData(
	const TSharedPtr<FBasicProxy>& InProxySPtr,
	const TSoftObjectPtr<UItemProxy_Description>& InItemProxy_Description
)
{
	ProxySPtr = InProxySPtr;
	ItemProxy_Description = InItemProxy_Description;

	SetVisibility(ESlateVisibility::Hidden);
}

void UItemDecription::NativeConstruct()
{
	Super::NativeConstruct();

	UpdatePosstion();

	SetUIStyle();

	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UItemDecription::NativeTick(
	const FGeometry& MyGeometry,
	float InDeltaTime
)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdatePosstion();
}

void UItemDecription::UpdatePosstion()
{
	FVector2D ScreenSpacePosition;
	GetWorld()->GetGameViewport()->GetMousePosition(ScreenSpacePosition);
	SetPositionInViewport(ScreenSpacePosition);
}

void UItemDecription::SetUIStyle()
{
	if (!ProxySPtr)
	{
		return;
	}
	{
		auto WidgetPtr = Cast<UTextBlock>(GetWidgetFromName(FUItemDecription::Get().Title));
		if (WidgetPtr)
		{
			WidgetPtr->SetText(FText::FromString(ProxySPtr->GetProxyName()));
		}
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

		auto WidgetPtr = Cast<URichTextBlock>(GetWidgetFromName(FUItemDecription::Get().Text));
		if (WidgetPtr)
		{
			WidgetPtr->SetText(FText::FromString(Text));
		}
	}
}
