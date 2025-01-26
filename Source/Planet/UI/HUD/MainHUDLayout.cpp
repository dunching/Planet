#include "MainHUD.h"

#include "Components/Border.h"
#include "Components/WidgetSwitcher.h"

#include "MainHUDLayout.h"

#include "ConversationLayout.h"
#include "UICommon.h"
#include "PlanetPlayerController.h"
#include "GetItemInfosList.h"
#include "InteractionList.h"
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

	FName LowerHPSocket = TEXT("LowerHPSocket");

	FName InteractionList = TEXT("InteractionList");

	FName Layout_WidgetSwitcher = TEXT("Layout_WidgetSwitcher");
};

void UMainHUDLayout::SwitchToNewLayout(ELayoutCommon LayoutCommon)
{
	auto Lambda = [this](int32 Index)
	{
		auto UIPtr = Cast<UWidgetSwitcher>(GetWidgetFromName(FMainHUDLayout::Get().Layout_WidgetSwitcher));
		if (UIPtr)
		{
			const auto CurrentIndex = UIPtr->GetActiveWidgetIndex();
			if (CurrentIndex == Index)
			{
				return;
			}
			auto MenuInterfacePtr = Cast<ILayoutInterfacetion>(UIPtr->GetWidgetAtIndex(CurrentIndex));
			if (MenuInterfacePtr)
			{
				MenuInterfacePtr->DisEnable();
			}
			UIPtr->SetActiveWidgetIndex(Index);
			MenuInterfacePtr = Cast<ILayoutInterfacetion>(UIPtr->GetWidgetAtIndex(Index));
			if (MenuInterfacePtr)
			{
				MenuInterfacePtr->Enable();
			}
		}
	};

	Lambda(static_cast<int32>(LayoutCommon));
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

UConversationLayout* UMainHUDLayout::GetConversationLayout()
{
	auto UIPtr = Cast<UWidgetSwitcher>(GetWidgetFromName(FMainHUDLayout::Get().Layout_WidgetSwitcher));
	if (UIPtr)
	{
		const auto CurrentIndex = UIPtr->GetActiveWidgetIndex();
		if (CurrentIndex == static_cast<int32>(ELayoutCommon::kConversationLayout))
		{
			auto LayoutUIPtr = Cast<UConversationLayout>(UIPtr->GetWidgetAtIndex(CurrentIndex));
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

void UMainHUDLayout::SwitchIsLowerHP(bool bIsLowerHP)
{
	auto BorderPtr = Cast<UBorder>(GetWidgetFromName(FMainHUDLayout::Get().LowerHPSocket));
	if (!BorderPtr)
	{
		return;
	}

	BorderPtr->SetVisibility(bIsLowerHP ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
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
