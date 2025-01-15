
#include "MainMenuLayout.h"

#include "Components/WidgetSwitcher.h"

#include "UICommon.h"
#include "UIInterfaces.h"
#include "MenuInterface.h"

struct FMenuLayout : public TStructVariable<FMenuLayout>
{
	FName WidgetSwitcher = TEXT("WidgetSwitcher");
};

void UMainMenuLayout::NativeConstruct()
{
	Super::NativeConstruct();
}

void UMainMenuLayout::NativeDestruct()
{
	ON_SCOPE_EXIT
	{
		Super::NativeDestruct();
	};

	SyncData();
}

void UMainMenuLayout::ResetUIByData()
{

}

void UMainMenuLayout::SyncData()
{
	auto UIPtr = Cast<UWidgetSwitcher>(GetWidgetFromName(FMenuLayout::Get().WidgetSwitcher));
	if (UIPtr)
	{
		const auto CurrentIndex = UIPtr->GetActiveWidgetIndex();
		auto MenuInterfacePtr = Cast<IMenuInterface>(UIPtr->GetWidgetAtIndex(CurrentIndex));
		if (MenuInterfacePtr)
		{
			MenuInterfacePtr->SyncData();
		}
	}
}

void UMainMenuLayout::SwitchViewer(EMenuType MenuType)
{
	auto Lambda = [this](int32 Index)
		{
			auto UIPtr = Cast<UWidgetSwitcher>(GetWidgetFromName(FMenuLayout::Get().WidgetSwitcher));
			if (UIPtr)
			{
				const auto CurrentIndex = UIPtr->GetActiveWidgetIndex();
				if (CurrentIndex == Index)
				{
					// 这里不太对啊？
					auto MenuInterfacePtr = Cast<IMenuInterface>(UIPtr->GetWidgetAtIndex(Index));
					if (MenuInterfacePtr)
					{
						MenuInterfacePtr->ResetUIByData();
					}

					return;
				}
				{
					auto MenuInterfacePtr = Cast<IMenuInterface>(UIPtr->GetWidgetAtIndex(CurrentIndex));
					if (MenuInterfacePtr)
					{
						MenuInterfacePtr->SyncData();
					}
				}
				UIPtr->SetActiveWidgetIndex(Index);
				auto MenuInterfacePtr = Cast<IMenuInterface>(UIPtr->GetWidgetAtIndex(Index));
				if (MenuInterfacePtr)
				{
					MenuInterfacePtr->ResetUIByData();
				}
			}
		};

	switch (MenuType)
	{
	case EMenuType::kAllocationSkill:
	{
		Lambda(0);
	}
	break;
	case EMenuType::kAllocationTalent:
	{
		Lambda(1);
	}
	break;
	case EMenuType::kGroupManagger:
	{
		Lambda(2);
	}
	break;
	case EMenuType::kRaffle:
	{
		Lambda(1);
	}
	break;
	default:
		break;
	}
}
