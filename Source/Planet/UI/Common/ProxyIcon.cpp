#include "ProxyIcon.h"

#include "Components/Image.h"
#include "Engine/AssetManager.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "DataTableCollection.h"
#include "ItemDecription.h"
#include "ItemProxy_Description.h"
#include "UICommon.h"
#include "VisitorSubsystem.h"

struct FProxyIcon : public TStructVariable<FProxyIcon>
{
	const FName Content = TEXT("Content");

	const FName Icon = TEXT("Icon");
};

inline void UProxyIcon::ResetToolUIByData(
	const TSharedPtr<FBasicProxy>& InBasicProxyPtr
	)
{
	if (InBasicProxyPtr)
	{
		ProxySPtr = InBasicProxyPtr;
	}
	else
	{
		ProxySPtr = nullptr;
	}

	SetItemType();
}

void UProxyIcon::ResetToolUIByData(
	const FGameplayTag& InProxyType
	)
{
	ProxyType = InProxyType;

	SetItemType();
}

void UProxyIcon::NativeOnMouseEnter(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent
	)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	if (bIsDisplayInfo)
	{
		if (ProxySPtr)
		{
			UVisitorSubsystem::GetInstance()->HoverInProxy(ProxySPtr);
		}
		else if (ProxyType.IsValid())
		{
			UVisitorSubsystem::GetInstance()->HoverInProxy(ProxyType);
		}
	}
}

void UProxyIcon::NativeOnMouseLeave(
	const FPointerEvent& InMouseEvent
	)
{
	UVisitorSubsystem::GetInstance()->StopHoverInProxy();

	Super::NativeOnMouseLeave(InMouseEvent);
}

void UProxyIcon::SetItemType()
{
	auto ImagePtr = Cast<UImage>(GetWidgetFromName(FProxyIcon::Get().Icon));
	if (ImagePtr)
	{
		if (ProxySPtr)
		{
			auto ProxyDTSPtr = GetTableRowProxy(ProxySPtr->GetProxyType());
			if (ProxyDTSPtr)
			{
				if (auto ItemProxy_Description = ProxyDTSPtr->ItemProxy_Description.LoadSynchronous())
				{
					ImagePtr->SetVisibility(ESlateVisibility::Visible);

					AsyncLoadText(ItemProxy_Description->DefaultIcon, ImagePtr);

					return;
				}
			}
			else
			{
				ImagePtr->SetVisibility(ESlateVisibility::Hidden);
			}
		}
		else if (ProxyType.IsValid())
		{
			auto ProxyDTSPtr = GetTableRowProxy(ProxySPtr->GetProxyType());
			if (ProxyDTSPtr)
			{
				if (auto ItemProxy_Description = ProxyDTSPtr->ItemProxy_Description.LoadSynchronous())
				{
					ImagePtr->SetVisibility(ESlateVisibility::Visible);

					AsyncLoadText(ItemProxy_Description->DefaultIcon, ImagePtr);

					return;
				}
			}
			else
			{
				ImagePtr->SetVisibility(ESlateVisibility::Hidden);
			}
		}
		else
		{
			ImagePtr->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}
