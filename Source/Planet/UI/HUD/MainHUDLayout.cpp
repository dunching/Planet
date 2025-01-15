#include "MainHUD.h"

#include "Components/Border.h"
#include "Components/WidgetSwitcher.h"

#include "MainHUDLayout.h"
#include "UICommon.h"
#include "PlanetPlayerController.h"
#include "GetItemInfosList.h"
#include "InteractionList.h"
#include "LayoutInterfacetion.h"

struct FMainHUDLayout : public TStructVariable<FMainHUDLayout>
{
	FName GetItemInfos_Socket = TEXT("GetItemInfos_Socket");

	FName RaffleMenu_Socket = TEXT("RaffleMenu_Socket");

	FName AllocationSkills_Socket = TEXT("AllocationSkills_Socket");

	FName PawnStateConsumablesHUD_Socket = TEXT("PawnStateConsumablesHUD_Socket");

	FName PawnActionStateHUDSocket = TEXT("PawnActionStateHUDSocket");

	FName LowerHPSocket = TEXT("LowerHPSocket");

	FName InteractionList = TEXT("InteractionList");

	FName Layout_WidgetSwichter = TEXT("Layout_WidgetSwichter");
};

void UMainHUDLayout::SwitchToNewLayout(ELayoutCommon LayoutCommon)
{
	auto Lambda = [this](int32 Index)
	{
		auto UIPtr = Cast<UWidgetSwitcher>(GetWidgetFromName(FMainHUDLayout::Get().Layout_WidgetSwichter));
		if (UIPtr)
		{
			const auto CurrentIndex = UIPtr->GetActiveWidgetIndex();
			if (CurrentIndex == Index)
			{
				auto MenuInterfacePtr = Cast<ILayoutInterfacetion>(UIPtr->GetWidgetAtIndex(Index));
				if (MenuInterfacePtr)
				{
					MenuInterfacePtr->Enable();
				}

				return;
			}
			UIPtr->SetActiveWidgetIndex(Index);
			auto MenuInterfacePtr = Cast<ILayoutInterfacetion>(UIPtr->GetWidgetAtIndex(Index));
			if (MenuInterfacePtr)
			{
				MenuInterfacePtr->DisEnable();
			}
		}
	};

	switch (LayoutCommon)
	{
	case ELayoutCommon::kActionLayout:
		{
			Lambda(0);
		}
		break;
	case ELayoutCommon::kMenuLayout:
		{
			Lambda(1);
		}
		break;
	case ELayoutCommon::KConversationLayout:
		{
			Lambda(2);
		}
		break;
	case ELayoutCommon::kEndangeredLayout:
		{
			Lambda(3);
		}
		break;
	default:
		{
		}
		break;
	}
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

UInteractionList* UMainHUDLayout::GetInteractionList()
{
	auto UIPtr = Cast<UInteractionList>(GetWidgetFromName(FMainHUDLayout::Get().InteractionList));
	if (!UIPtr)
	{
		return nullptr;
	}

	return UIPtr;
}
