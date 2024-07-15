
#include "MyUserWidget.h"

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
		Iter->CancelHandle();
	}
}
