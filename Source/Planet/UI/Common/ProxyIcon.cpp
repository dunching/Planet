#include "ProxyIcon.h"

#include "Components/Image.h"
#include "Engine/AssetManager.h"

struct FProxyIcon : public TStructVariable<FProxyIcon>
{
	const FName Content = TEXT("Content");

	const FName Enable = TEXT("Enable");

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

void UProxyIcon::SetItemType()
{
	auto ImagePtr = Cast<UImage>(GetWidgetFromName(FProxyIcon::Get().Icon));
	if (ImagePtr)
	{
		if (BasicProxyPtr)
		{
			ImagePtr->SetVisibility(ESlateVisibility::Visible);

			FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
			AsyncLoadTextureHandleAry.Add(
				StreamableManager.RequestAsyncLoad(
					BasicProxyPtr->GetIcon().ToSoftObjectPath(),
					[this, ImagePtr]()
					{
						ImagePtr->SetBrushFromTexture(BasicProxyPtr->GetIcon().Get());
					}
				)
			);
		}
		else
		{
			ImagePtr->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}
