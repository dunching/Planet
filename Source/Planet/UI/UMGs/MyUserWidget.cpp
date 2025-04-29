#include "MyUserWidget.h"

#include "Components/Image.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

void UMyUserWidget::NativeDestruct()
{
	ON_SCOPE_EXIT
	{
		Super::NativeDestruct();
	};

	for (auto Iter : AsyncLoadTextureHandleAry)
	{
		if (Iter)
		{
			Iter->CancelHandle();
		}
	}
}

void UMyUserWidget::AsyncLoadText(
	const TSoftObjectPtr<UTexture2D>& Texture,
	UImage* ImagePtr
)
{
	if (!Texture.ToSoftObjectPath().IsAsset())
	{
		return;
	}
	
	FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
	AsyncLoadTextureHandleAry.Add(
		StreamableManager.RequestAsyncLoad(
			Texture.ToSoftObjectPath(),
			[ImagePtr, Texture]()
			{
				ImagePtr->SetBrushFromTexture(Texture.Get());
			}
		)
	);
}
