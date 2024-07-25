
#include "RaffleMenu.h"

#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include <Blueprint/WidgetTree.h>
#include <Components/HorizontalBox.h>
#include <Components/VerticalBox.h>

#include "CharacterBase.h"
#include "TalentAllocationComponent.h"
#include "TalentIcon.h"
#include "SceneElement.h"
#include "CoinList.h"
#include "RaffleType.h"
#include "RaffleBtn.h"
#include "UICommon.h"
#include "RaffleSubSystem.h"

struct FRaffleMenu : public TGetSocketName<FRaffleMenu>
{
	FName CoinList = TEXT("CoinList");

	FName RaffleType_VerticalBox = TEXT("RaffleType_VerticalBox");

	FName RaffleBtn_VerticalBox = TEXT("RaffleBtn_VerticalBox");
};


void URaffleMenu::NativeConstruct()
{
	Super::NativeConstruct();

	InitialRaffleType();

}

void URaffleMenu::InitialRaffleType()
{
	auto UIPtr = Cast<UVerticalBox>(GetWidgetFromName(FRaffleMenu::Get().RaffleType_VerticalBox));
	if (UIPtr)
	{
		auto ChildAry = UIPtr->GetAllChildren();
		URaffleType* LastUIPtr = nullptr;
		for (auto Iter : ChildAry)
		{
			auto RafflePtr = Cast<URaffleType>(Iter);
			if (RafflePtr)
			{
				LastUIPtr = RafflePtr;
				RafflePtr->UnSelect();
				auto Handle = RafflePtr->OnClicked.AddCallback(std::bind(&ThisClass::OnRaffleTypeSelected, this, std::placeholders::_1));
				Handle->bIsAutoUnregister = false;
			}
		}
		if (LastUIPtr)
		{
			LastUIPtr->Select();
		}
	}
}

void URaffleMenu::InitialRaffleBtn()
{
	auto UIPtr = Cast<UVerticalBox>(GetWidgetFromName(FRaffleMenu::Get().RaffleBtn_VerticalBox));
	if (UIPtr)
	{
		auto ChildAry = UIPtr->GetAllChildren();
		for (auto Iter : ChildAry)
		{
			auto RafflePtr = Cast<URaffleBtn>(Iter);
			if (RafflePtr)
			{
				auto Handle = RafflePtr->OnClicked.AddCallback(std::bind(&ThisClass::OnRaffleBtnSelected, this, std::placeholders::_1));
				Handle->bIsAutoUnregister = false;
			}
		}
	}
}

void URaffleMenu::NativeDestruct()
{
	Super::NativeDestruct();
}

void URaffleMenu::SetHoldItemProperty(const FSceneToolsContainer& NewSPHoldItemPerperty)
{
	auto UIPtr = Cast<UCoinList>(GetWidgetFromName(FRaffleMenu::Get().CoinList));
	if (UIPtr)
	{
		UIPtr->ResetUIByData(NewSPHoldItemPerperty.GetCoinUintAry());
	}
}

void URaffleMenu::OnRaffleTypeSelected(URaffleType* RaffleTypePtr)
{
	if (PreviouRaffleTypePtr)
	{
		if (RaffleTypePtr != PreviouRaffleTypePtr)
		{
			PreviouRaffleTypePtr->UnSelect();
		}
	}
	LastRaffleType = RaffleTypePtr->RaffleType;
	PreviouRaffleTypePtr = RaffleTypePtr;
}

void URaffleMenu::OnRaffleBtnSelected(URaffleBtn* RaffleTypePtr)
{
	URaffleSubSystem::GetInstance()->Raffle(LastRaffleType, RaffleTypePtr->ReffleCount);
}
