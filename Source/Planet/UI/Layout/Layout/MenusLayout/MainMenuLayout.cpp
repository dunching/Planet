
#include "MainMenuLayout.h"

#include "Components/WidgetSwitcher.h"

#include "UICommon.h"
#include "UIInterfaces.h"
#include "MenuInterface.h"

struct FMainMenuLayout : public TStructVariable<FMainMenuLayout>
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
}

void UMainMenuLayout::Enable()
{
	ILayoutInterfacetion::Enable();
}

void UMainMenuLayout::DisEnable()
{
	SyncData();
	
	ILayoutInterfacetion::DisEnable();
}

void UMainMenuLayout::SyncData()
{
	auto UIPtr = Cast<UWidgetSwitcher>(GetWidgetFromName(FMainMenuLayout::Get().WidgetSwitcher));
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
			auto UIPtr = Cast<UWidgetSwitcher>(GetWidgetFromName(FMainMenuLayout::Get().WidgetSwitcher));
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

	Lambda(static_cast<int32>(MenuType));
}
