#include "ItemDecriptionBaseWidget.h"

#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetStringLibrary.h"

#include "ItemProxy_Description.h"
#include "TemplateHelper.h"
#include "ItemProxy.h"

struct FItemDecriptionBaseWidget : public TStructVariable<FItemDecriptionBaseWidget>
{
	const FName Title = TEXT("Title");

	const FName Text = TEXT("Text");
};

UItemDecriptionBaseWidget::UItemDecriptionBaseWidget(
	const FObjectInitializer& ObjectInitializer
)
{
}

inline void UItemDecriptionBaseWidget::BindData(
	const TSharedPtr<FBasicProxy>& InProxySPtr,
	const TSoftObjectPtr<UItemProxy_Description>& InItemProxy_Description
)
{
	ProxySPtr = InProxySPtr;
	ItemProxy_Description = InItemProxy_Description;

	SetVisibility(ESlateVisibility::Hidden);
}

inline void UItemDecriptionBaseWidget::BindData(
	const FGameplayTag &InProxyType,
	const TSoftObjectPtr<UItemProxy_Description>& InItemProxy_Description
)
{
	ProxyType = InProxyType;
	ItemProxy_Description = InItemProxy_Description;

	SetVisibility(ESlateVisibility::Hidden);
}

void UItemDecriptionBaseWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UpdatePosstion();

	SetUIStyle();

	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UItemDecriptionBaseWidget::NativeTick(
	const FGeometry& MyGeometry,
	float InDeltaTime
)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdatePosstion();
}

void UItemDecriptionBaseWidget::UpdatePosstion()
{
	FVector2D ScreenSpacePosition;
	GetWorld()->GetGameViewport()->GetMousePosition(ScreenSpacePosition);
	SetPositionInViewport(ScreenSpacePosition);
}

void UItemDecriptionBaseWidget::SetUIStyle()
{
	if (ProxySPtr)
	{
		auto WidgetPtr = Cast<UTextBlock>(GetWidgetFromName(FItemDecriptionBaseWidget::Get().Title));
		if (WidgetPtr)
		{
			WidgetPtr->SetText(FText::FromString(ProxySPtr->GetProxyName()));
		}
	}
	else if (ProxyType.IsValid())
	{
		auto WidgetPtr = Cast<UTextBlock>(GetWidgetFromName(FItemDecriptionBaseWidget::Get().Title));
		if (WidgetPtr)
		{
			WidgetPtr->SetText(FText::FromString(ItemProxy_Description.LoadSynchronous()->ProxyName));
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

		auto WidgetPtr = Cast<URichTextBlock>(GetWidgetFromName(FItemDecriptionBaseWidget::Get().Text));
		if (WidgetPtr)
		{
			WidgetPtr->SetText(FText::FromString(Text));
		}
	}
}
