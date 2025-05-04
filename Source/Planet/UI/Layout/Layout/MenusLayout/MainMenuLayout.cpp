#include "MainMenuLayout.h"

#include "GroupManagger.h"
#include "LayoutCommon.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"

#include "UICommon.h"
#include "UIInterfaces.h"
#include "MenuInterface.h"
#include "PlanetControllerInterface.h"
#include "TeamMatesHelperComponent.h"

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

	auto PCPtr =
		Cast<IPlanetControllerInterface>(UGameplayStatics::GetPlayerController(this, 0));
	if (!PCPtr)
	{
		return;
	}

	auto GMCPtr = PCPtr->GetGroupManagger();
	GMCPtr->GetTeamMatesHelperComponent()->SpwanTeammateCharacter();

	ILayoutInterfacetion::DisEnable();
}

ELayoutCommon UMainMenuLayout::GetLayoutType() const
{
	return ELayoutCommon::kMenuLayout;
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

void UMainMenuLayout::SwitchViewer(
	EMenuType MenuType
)
{
	auto UIPtr = Cast<UWidgetSwitcher>(GetWidgetFromName(FMainMenuLayout::Get().WidgetSwitcher));
	if (UIPtr)
	{
		{
			auto MenuInterfacePtr = Cast<IMenuInterface>(UIPtr->GetActiveWidget());
			if (MenuInterfacePtr)
			{
				if (MenuInterfacePtr->GetMenuType() == MenuType)
				{
					// 这里不太对啊？
					MenuInterfacePtr->ResetUIByData();
					return;
				}

				MenuInterfacePtr->SyncData();
			}
		}

		auto ChildrensAry = UIPtr->GetAllChildren();
		for (auto Iter : ChildrensAry)
		{
			auto MenuInterfacePtr = Cast<IMenuInterface>(Iter);
			if (!MenuInterfacePtr)
			{
				continue;
			}
			if (MenuInterfacePtr->GetMenuType() != MenuType)
			{
				continue;
			}
			MenuInterfacePtr->ResetUIByData();
			UIPtr->SetActiveWidget(Iter);
			OnMenuLayoutChanged(MenuType);
			break;
		}
	}
}
