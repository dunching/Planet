
#include "EquipMenu.h"

#include "Components/GridPanel.h"
#include "EquipIcon.h"

void UEquipMenu::NativeConstruct()
{
	Super::NativeConstruct();
}

void UEquipMenu::ResetFiled()
{
	Super::ResetFiled();

	auto EquimentGridPtr = Cast<UGridPanel>(GetWidgetFromName("EquimentGrid"));
	if (EquimentGridPtr)
	{
		auto ChildAry = EquimentGridPtr->GetAllChildren();
		for (auto Iter : ChildAry)
		{
			auto ItemPtr = Cast<UEquipIcon>(Iter);
			if (ItemPtr)
			{
	//			ItemPtr->SetNum();
			}
		}
	}
}

FItemNum UEquipMenu::GetItem(int32 SlotNum)
{
	UEquipIcon* BaseItemPtr = nullptr;

	auto WidgetName = FString::Printf(TEXT("EquipIcon%d"), SlotNum);

	BaseItemPtr = Cast<UEquipIcon>(GetWidgetFromName(*WidgetName));

	if (BaseItemPtr)
	{
		return BaseItemPtr->GetItemPropertyBase();
	}

	return FItemNum();
}
