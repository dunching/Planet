
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

void UMenuLayout::NativeDestruct()
{
	ON_SCOPE_EXIT
	{
		Super::NativeDestruct();
	};

	SyncData();
}

void UMenuLayout::ResetUIByData()
{

}

void UMenuLayout::SyncData()
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

void UMenuLayout::SwitchViewer(EMenuType MenuType)
{
	auto Lambda = [this](int32 Index) 
		{
			auto UIPtr = Cast<UWidgetSwitcher>(GetWidgetFromName(FMenuLayout::Get().WidgetSwitcher));
			if (UIPtr)
			{
				const auto CurrentIndex = UIPtr->GetActiveWidgetIndex();
				if (CurrentIndex == Index)
				{
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
		break;
	case EMenuType::kRaffle:
		break;
	default:
		break;
	}
}
