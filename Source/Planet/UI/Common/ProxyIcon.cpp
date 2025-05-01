#include "ProxyIcon.h"

#include "Components/Image.h"
#include "Engine/AssetManager.h"
#include "Blueprint/SlateBlueprintLibrary.h"

#include "SceneProxyExtendInfo.h"
#include "ItemDecription.h"
#include "ItemProxy_Description.h"
#include "UICommon.h"
#include "Kismet/GameplayStatics.h"

struct FProxyIcon : public TStructVariable<FProxyIcon>
{
	const FName Content = TEXT("Content");

	const FName Icon = TEXT("Icon");
};

inline void UProxyIcon::ResetToolUIByData(
	const TSharedPtr<FBasicProxy>& InBasicProxyPtr
)
{
	if (InBasicProxyPtr == BasicProxyPtr)
	{
		return;
	}

	BasicProxyPtr = InBasicProxyPtr;

	SetItemType();
}

void UProxyIcon::NativeOnMouseEnter(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent
)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	if (BasicProxyPtr && bIsDisplayInfo)
	{
		auto ProxyDTSPtr = USceneProxyExtendInfoMap::GetInstance()->GetTableRowProxy(BasicProxyPtr->GetProxyType());
		if (ProxyDTSPtr)
		{
			if (ItemDecriptionPtr)
			{
			}
			else
			{
				if (!ProxyDTSPtr->ItemProxy_Description || !ProxyDTSPtr->ItemDecriptionClass)
				{
					return;
				}

				ItemDecriptionPtr = CreateWidget<UItemDecription>(this, ProxyDTSPtr->ItemDecriptionClass);
			}
			if (ItemDecriptionPtr)
			{
				ItemDecriptionPtr->BindData(BasicProxyPtr, ProxyDTSPtr->ItemProxy_Description);

				ItemDecriptionPtr->AddToViewport(EUIOrder::kHoverDecription);
			}
		}
	}
}

void UProxyIcon::NativeOnMouseLeave(
	const FPointerEvent& InMouseEvent
)
{
	if (ItemDecriptionPtr)
	{
		ItemDecriptionPtr->RemoveFromParent();
	}
	ItemDecriptionPtr = nullptr;

	Super::NativeOnMouseLeave(InMouseEvent);
}

void UProxyIcon::SetItemType()
{
	auto ImagePtr = Cast<UImage>(GetWidgetFromName(FProxyIcon::Get().Icon));
	if (ImagePtr)
	{
		if (BasicProxyPtr)
		{
			ImagePtr->SetVisibility(ESlateVisibility::Visible);

			AsyncLoadText(BasicProxyPtr->GetIcon(), ImagePtr);
		}
		else
		{
			ImagePtr->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}
