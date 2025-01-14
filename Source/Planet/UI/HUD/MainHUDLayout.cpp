#include "MainHUD.h"

#include "Components/Border.h"

#include "MainHUDLayout.h"
#include "UICommon.h"
#include "PlanetPlayerController.h"
#include "GetItemInfosList.h"
#include "InteractionList.h"

struct FMainHUDLayout : public TStructVariable<FMainHUDLayout>
{
	FName GetItemInfos_Socket = TEXT("GetItemInfos_Socket");

	FName RaffleMenu_Socket = TEXT("RaffleMenu_Socket");

	FName AllocationSkills_Socket = TEXT("AllocationSkills_Socket");

	FName PawnStateConsumablesHUD_Socket = TEXT("PawnStateConsumablesHUD_Socket");

	FName PawnActionStateHUDSocket = TEXT("PawnActionStateHUDSocket");

	FName LowerHPSocket = TEXT("LowerHPSocket");

	FName InteractionList = TEXT("InteractionList");
};

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
