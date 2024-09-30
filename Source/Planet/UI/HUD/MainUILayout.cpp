
#include "MainUILayout.h"

#if WITH_EDITOR
void UMainUILayout::NativeConstruct()
#else
void UMainUILayout::NativePreConstruct()
#endif
{
#if WITH_EDITOR
	Super::NativeConstruct();
#else
	Super::NativePreConstruct();
#endif

	// 在此处移除默认不显示的UI
}
