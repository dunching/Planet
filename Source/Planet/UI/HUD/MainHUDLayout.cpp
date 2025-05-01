#include "MainHUDLayout.h"

#include "MainHUD.h"

#include "Components/Border.h"
#include "Components/WidgetSwitcher.h"
#include "Components/CanvasPanel.h"

#include "MainHUDLayout.h"

#include "InteractionConversationLayout.h"
#include "UICommon.h"
#include "PlanetPlayerController.h"
#include "GetItemInfosList.h"
#include "InteractionList.h"
#include "InteractionOptionsLayout.h"
#include "LayoutCommon.h"
#include "LayoutInterfacetion.h"
#include "MainMenuLayout.h"
#include "RegularActionLayout.h"

struct FMainHUDLayout : public TStructVariable<FMainHUDLayout>
{
	FName GetItemInfos_Socket = TEXT("GetItemInfos_Socket");

	FName RaffleMenu_Socket = TEXT("RaffleMenu_Socket");

	FName AllocationSkills_Socket = TEXT("AllocationSkills_Socket");

	FName PawnStateConsumablesHUD_Socket = TEXT("PawnStateConsumablesHUD_Socket");

	FName PawnActionStateHUDSocket = TEXT("PawnActionStateHUDSocket");

	FName InteractionList = TEXT("InteractionList");

	FName Layout_WidgetSwitcher = TEXT("Layout_WidgetSwitcher");

	FName ItemDecriptionCanvas = TEXT("ItemDecriptionCanvas");
};

void UMainHUDLayout::NativeConstruct()
{
	Super::NativeConstruct();

	auto UIPtr = Cast<UCanvasPanel>(GetWidgetFromName(FMainHUDLayout::Get().ItemDecriptionCanvas));
	if (UIPtr)
	{
		UIPtr->ClearChildren();
	}
}

void UMainHUDLayout::SwitchToNewLayout(
	ELayoutCommon LayoutCommon,
	const ILayoutInterfacetion::FOnQuit& OnQuit
	)
{
	auto UIPtr = Cast<UWidgetSwitcher>(GetWidgetFromName(FMainHUDLayout::Get().Layout_WidgetSwitcher));
	if (UIPtr)
	{
		{
			auto MenuInterfacePtr = Cast<ILayoutInterfacetion>(UIPtr->GetActiveWidget());
			if (MenuInterfacePtr)
			{
				if (MenuInterfacePtr->GetLayoutType() == LayoutCommon)
				{
					return;
				}

				MenuInterfacePtr->DisEnable();
			}
		}

		auto ChildrensAry = UIPtr->GetAllChildren();
		for (auto Iter : ChildrensAry)
		{
			auto MenuInterfacePtr = Cast<ILayoutInterfacetion>(Iter);
			if (!MenuInterfacePtr)
			{
				continue;
			}
			if (MenuInterfacePtr->GetLayoutType() != LayoutCommon)
			{
				continue;
			}
			MenuInterfacePtr->OnQuit = OnQuit;
			MenuInterfacePtr->Enable();
			UIPtr->SetActiveWidget(Iter);
			break;
		}
	}
}

UMainMenuLayout* UMainHUDLayout::GetMenuLayout()
{
	auto UIPtr = Cast<UWidgetSwitcher>(GetWidgetFromName(FMainHUDLayout::Get().Layout_WidgetSwitcher));
	if (UIPtr)
	{
		const auto CurrentIndex = UIPtr->GetActiveWidgetIndex();
		if (CurrentIndex == static_cast<int32>(ELayoutCommon::kMenuLayout))
		{
			auto MenuInterfacePtr = Cast<UMainMenuLayout>(UIPtr->GetWidgetAtIndex(CurrentIndex));
			if (MenuInterfacePtr)
			{
				return MenuInterfacePtr;
			}
		}
	}
	return nullptr;
}

UInteractionConversationLayout* UMainHUDLayout::GetConversationLayout()
{
	auto UIPtr = Cast<UWidgetSwitcher>(GetWidgetFromName(FMainHUDLayout::Get().Layout_WidgetSwitcher));
	if (UIPtr)
	{
		const auto CurrentIndex = UIPtr->GetActiveWidgetIndex();
		if (CurrentIndex == static_cast<int32>(ELayoutCommon::kConversationLayout))
		{
			auto LayoutUIPtr = Cast<UInteractionConversationLayout>(UIPtr->GetWidgetAtIndex(CurrentIndex));
			if (LayoutUIPtr)
			{
				return LayoutUIPtr;
			}
		}
	}
	return nullptr;
}

UInteractionOptionsLayout* UMainHUDLayout::GetInteractionOptionsLayout()
{
	auto UIPtr = Cast<UWidgetSwitcher>(GetWidgetFromName(FMainHUDLayout::Get().Layout_WidgetSwitcher));
	if (UIPtr)
	{
		const auto CurrentIndex = UIPtr->GetActiveWidgetIndex();
		if (CurrentIndex == static_cast<int32>(ELayoutCommon::kOptionLayout))
		{
			auto LayoutUIPtr = Cast<UInteractionOptionsLayout>(UIPtr->GetWidgetAtIndex(CurrentIndex));
			if (LayoutUIPtr)
			{
				return LayoutUIPtr;
			}
		}
	}
	return nullptr;
}

URegularActionLayout* UMainHUDLayout::GetRegularActionLayout() const
{
	auto UIPtr = Cast<UWidgetSwitcher>(GetWidgetFromName(FMainHUDLayout::Get().Layout_WidgetSwitcher));
	if (UIPtr)
	{
		const auto CurrentIndex = UIPtr->GetActiveWidgetIndex();
		if (CurrentIndex == static_cast<int32>(ELayoutCommon::kMenuLayout))
		{
			auto LayoutUIPtr = Cast<URegularActionLayout>(UIPtr->GetWidgetAtIndex(CurrentIndex));
			if (LayoutUIPtr)
			{
				return LayoutUIPtr;
			}
		}
	}
	return nullptr;
}

UGetItemInfosList* UMainHUDLayout::GetItemInfos()
{
	auto BorderPtr = Cast<UBorder>(GetWidgetFromName(FMainHUDLayout::Get().GetItemInfos_Socket));
	if (!BorderPtr)
	{
		return nullptr;
	}

	for (auto Iter : BorderPtr->GetAllChildren())
	{
		auto UIPtr = Cast<UGetItemInfosList>(Iter);
		if (UIPtr)
		{
			return UIPtr;
		}
	}

	return nullptr;
}

void UMainHUDLayout::SwitchIsLowerHP(
	bool bIsLowerHP
	)
{
}

// UInteractionList* UMainHUDLayout::GetInteractionList()
// {
// 	auto UIPtr = Cast<UInteractionList>(GetWidgetFromName(FMainHUDLayout::Get().InteractionList));
// 	if (!UIPtr)
// 	{
// 		return nullptr;
// 	}
//
// 	return UIPtr;
// }
