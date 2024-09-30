
#include "MenuLayout.h"

#include "Components/WidgetSwitcher.h"

#include "UICommon.h"
#include "UIInterfaces.h"
#include "MenuInterface.h"

struct FMenuLayout : public TStructVariable<FMenuLayout>
{
	FName WidgetSwitcher = TEXT("WidgetSwitcher");
};

void UMenuLayout::NativeConstruct()
{
	Super::NativeConstruct();
}

void UMenuLayout::SwitchViewer(EMenuType MenuType)
{
	switch (MenuType)
	{
	case EMenuType::kAllocationSkill:
	{
		auto UIPtr = Cast<UWidgetSwitcher>(GetWidgetFromName(FMenuLayout::Get().WidgetSwitcher));
		if (UIPtr)
		{
			UIPtr->SetActiveWidgetIndex(0);
			auto MenuInterfacePtr = Cast<IMenuInterface>(UIPtr->GetWidgetAtIndex(0));
			if (MenuInterfacePtr)
			{
				MenuInterfacePtr->ResetUIByData();
			}
		}
	}
	break;
	case EMenuType::kAllocationTalent:
	{
		auto UIPtr = Cast<UWidgetSwitcher>(GetWidgetFromName(FMenuLayout::Get().WidgetSwitcher));
		if (UIPtr)
		{
			UIPtr->SetActiveWidgetIndex(1);
			auto MenuInterfacePtr = Cast<IMenuInterface>(UIPtr->GetWidgetAtIndex(0));
			if (MenuInterfacePtr)
			{
				MenuInterfacePtr->ResetUIByData();
			}
		}
	}
	break;
	case EMenuType::kGroupManagger:
		break;
	case EMenuType::kRaffle:
		break;
	default:
		break;
	}
}
