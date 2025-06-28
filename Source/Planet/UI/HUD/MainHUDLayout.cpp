#include "MainHUDLayout.h"

#include "MainHUD.h"

#include "Components/Border.h"
#include "Components/WidgetSwitcher.h"
#include "Components/CanvasPanel.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"

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
	FName GetItemInfosList = TEXT("GetItemInfosList");

	FName RaffleMenu_Socket = TEXT("RaffleMenu_Socket");

	FName AllocationSkills_Socket = TEXT("AllocationSkills_Socket");

	FName PawnStateConsumablesHUD_Socket = TEXT("PawnStateConsumablesHUD_Socket");

	FName PawnActionStateHUDSocket = TEXT("PawnActionStateHUDSocket");

	FName InteractionList = TEXT("InteractionList");

	FName Layout_WidgetSwitcher = TEXT("Layout_WidgetSwitcher");

	FName ItemDecriptionCanvas = TEXT("ItemDecriptionCanvas");

	FName OtherWidgets = TEXT("OtherWidgets");
};

void UMainHUDLayout::NativeConstruct()
{
	Super::NativeConstruct();

	// 常驻的UI
	{
		auto UIPtr = Cast<UCanvasPanel>(GetWidgetFromName(FMainHUDLayout::Get().ItemDecriptionCanvas));
		if (UIPtr)
		{
			UIPtr->ClearChildren();
		}
	}
	{
		auto UIPtr = Cast<UOverlay>(GetWidgetFromName(FMainHUDLayout::Get().OtherWidgets));
		if (UIPtr)
		{
			UIPtr->ClearChildren();
		}
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
			OnLayoutChanged(LayoutCommon);
			break;
		}
	}
}

ILayoutInterfacetion* UMainHUDLayout::GetCurrentLayout() const
{
	auto UIPtr = Cast<UWidgetSwitcher>(GetWidgetFromName(FMainHUDLayout::Get().Layout_WidgetSwitcher));
	if (UIPtr)
	{
		auto MenuInterfacePtr = Cast<ILayoutInterfacetion>(UIPtr->GetActiveWidget());
		if (MenuInterfacePtr)
		{
			return MenuInterfacePtr;
		}
	}

	return nullptr;
}

UMainMenuLayout* UMainHUDLayout::GetMenuLayout()
{
	auto UIPtr = Cast<UWidgetSwitcher>(GetWidgetFromName(FMainHUDLayout::Get().Layout_WidgetSwitcher));
	if (UIPtr)
	{
		auto ChildrensAry = UIPtr->GetAllChildren();
		for (auto Iter : ChildrensAry)
		{
			auto MenuInterfacePtr = Cast<ILayoutInterfacetion>(Iter);
			if (!MenuInterfacePtr)
			{
				continue;
			}
			if (MenuInterfacePtr->GetLayoutType() != ELayoutCommon::kMenuLayout)
			{
				continue;
			}

			return Cast<UMainMenuLayout>(MenuInterfacePtr);
		}
	}

	return nullptr;
}

UInteractionConversationLayout* UMainHUDLayout::GetConversationLayout()
{
	auto UIPtr = Cast<UWidgetSwitcher>(GetWidgetFromName(FMainHUDLayout::Get().Layout_WidgetSwitcher));
	if (UIPtr)
	{
		auto ChildrensAry = UIPtr->GetAllChildren();
		for (auto Iter : ChildrensAry)
		{
			auto MenuInterfacePtr = Cast<ILayoutInterfacetion>(Iter);
			if (!MenuInterfacePtr)
			{
				continue;
			}
			if (MenuInterfacePtr->GetLayoutType() != ELayoutCommon::kConversationLayout)
			{
				continue;
			}

			return Cast<UInteractionConversationLayout>(MenuInterfacePtr);
		}
	}

	return nullptr;
}

UInteractionOptionsLayout* UMainHUDLayout::GetInteractionOptionsLayout()
{
	auto UIPtr = Cast<UWidgetSwitcher>(GetWidgetFromName(FMainHUDLayout::Get().Layout_WidgetSwitcher));
	if (UIPtr)
	{
		auto ChildrensAry = UIPtr->GetAllChildren();
		for (auto Iter : ChildrensAry)
		{
			auto MenuInterfacePtr = Cast<ILayoutInterfacetion>(Iter);
			if (!MenuInterfacePtr)
			{
				continue;
			}
			if (MenuInterfacePtr->GetLayoutType() != ELayoutCommon::kOptionLayout)
			{
				continue;
			}

			return Cast<UInteractionOptionsLayout>(MenuInterfacePtr);
		}
	}

	return nullptr;
}

URegularActionLayout* UMainHUDLayout::GetRegularActionLayout() const
{
	auto UIPtr = Cast<UWidgetSwitcher>(GetWidgetFromName(FMainHUDLayout::Get().Layout_WidgetSwitcher));
	if (UIPtr)
	{
		auto ChildrensAry = UIPtr->GetAllChildren();
		for (auto Iter : ChildrensAry)
		{
			auto MenuInterfacePtr = Cast<ILayoutInterfacetion>(Iter);
			if (!MenuInterfacePtr)
			{
				continue;
			}
			if (MenuInterfacePtr->GetLayoutType() != ELayoutCommon::kActionLayout)
			{
				continue;
			}

			return Cast<URegularActionLayout>(MenuInterfacePtr);
		}
	}

	return nullptr;
}

void UMainHUDLayout::DisplayWidgetInOtherCanvas(
	UUserWidget* WidgetPtr
	)
{
	auto UIPtr = Cast<UOverlay>(GetWidgetFromName(FMainHUDLayout::Get().OtherWidgets));
	if (UIPtr)
	{
		auto SlotPtr = Cast<UOverlaySlot>(UIPtr->AddChild(WidgetPtr));
		if (SlotPtr)
		{
			SlotPtr->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
			SlotPtr->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
		}
	}
}

UGetItemInfosList* UMainHUDLayout::GetItemInfos()
{
	auto UIPtr = Cast<UGetItemInfosList>(GetWidgetFromName(FMainHUDLayout::Get().GetItemInfosList));
	if (!UIPtr)
	{
		return nullptr;
	}

	return UIPtr;
}

UOverlaySlot* UMainHUDLayout::DisplayWidget(
	const TSubclassOf<UUserWidget>& WidgetClass,
	const std::function<void(
		UUserWidget*
		
		)>& Initializer
	)
{
	auto UIPtr = Cast<UOverlay>(GetWidgetFromName(FMainHUDLayout::Get().OtherWidgets));
	if (!UIPtr)
	{
		return nullptr;
	}

	auto WidgetPtr = CreateWidget(UIPtr, WidgetClass);
	if (Initializer && WidgetPtr)
	{
		Initializer(WidgetPtr);
	}

	WidgetPtr->AddToViewport(kOtherWidget);
	return nullptr;
	// auto SlotPtr = UIPtr->AddChildToOverlay(WidgetPtr);
	// if (SlotPtr)
	// {
	// 	SlotPtr->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
	// 	SlotPtr->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
	// }

	// return SlotPtr; 
}

void UMainHUDLayout::RemoveWidget(
	const TSubclassOf<UUserWidget>& WidgetClass
	)
{
	auto UIPtr = Cast<UOverlay>(GetWidgetFromName(FMainHUDLayout::Get().OtherWidgets));
	if (!UIPtr)
	{
		return;
	}
	auto ChildrensAry = UIPtr->GetAllChildren();
	for (auto Iter : ChildrensAry)
	{
		if (Iter->IsA(WidgetClass))
		{
			Iter->RemoveFromParent();
			return;
		}
	}
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
