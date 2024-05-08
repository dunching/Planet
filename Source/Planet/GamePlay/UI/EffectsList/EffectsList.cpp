
#include "EffectsList.h"

#include "MyWrapBox.h"
#include "EffectItem.h"

void UEffectsList::NativeConstruct()
{
	Super::NativeConstruct();

	ResetUIByData();
}

UEffectItem* UEffectsList::AddEffectItem()
{
	auto UIPtr = Cast<UMyWrapBox>(GetWidgetFromName(TEXT("WrapBox")));
	if (UIPtr)
	{
		auto ChildPtr = CreateWidget<UEffectItem>(this, EffectItemClass);
		auto SlotPtr = UIPtr->AddChildToWrapBox(ChildPtr);

		return ChildPtr;
	}

	return nullptr;
}

void UEffectsList::ResetUIByData()
{
	auto UIPtr = Cast<UMyWrapBox>(GetWidgetFromName(TEXT("WrapBox")));
	if (UIPtr)
	{
		UIPtr->ClearChildren();
	}
}

