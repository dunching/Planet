
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

	// �ڴ˴��Ƴ�Ĭ�ϲ���ʾ��UI
}
