
#include "RaffleMenu.h"

#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include <Blueprint/WidgetTree.h>
#include <Components/HorizontalBox.h>
#include <Components/VerticalBox.h>
#include <Components/CanvasPanel.h>
#include <Components/Button.h>

#include "CharacterBase.h"
#include "TalentAllocationComponent.h"
#include "TalentIcon.h"
#include "SceneElement.h"
#include "CoinList.h"
#include "RaffleType.h"
#include "RaffleBtn.h"
#include "UICommon.h"
#include "RaffleSubSystem.h"
#include "Raffle_Unit.h"
#include "SceneUnitExtendInfo.h"
#include "SceneUnitContainer.h"

struct FRaffleMenu : public TStructVariable<FRaffleMenu>
{
	FName CoinList = TEXT("CoinList");

	FName RaffleType_VerticalBox = TEXT("RaffleType_VerticalBox");

	FName RaffleBtn_VerticalBox = TEXT("RaffleBtn_VerticalBox");

	FName GetUnit_HorizaltalBox = TEXT("GetUnit_HorizaltalBox");
	
	FName GetUnitUI = TEXT("GetUnitUI");

	FName RaffleUI = TEXT("RaffleUI");

	FName ConfirmBtn = TEXT("ConfirmBtn");
};

void URaffleMenu::NativeConstruct()
{
	Super::NativeConstruct();

	InitialRaffleType();
	InitialRaffleBtn();

	SwitchDisplay(true);

	auto UIPtr = Cast<UButton>(GetWidgetFromName(FRaffleMenu::Get().ConfirmBtn));
	if (UIPtr)
	{
		UIPtr->OnClicked.AddDynamic(this, &ThisClass::OnClickedConfirmGetUnitBtn);
	}

	OnGetUnitDelegateHandle = URaffleSubSystem::GetInstance()->OnGetUnitAry.AddCallback(
		std::bind(&ThisClass::ResetGetUnitAry, this, std::placeholders::_1)
	);
}

void URaffleMenu::NativeDestruct()
{
	Super::NativeDestruct();

	OnGetUnitDelegateHandle->UnBindCallback();
}

void URaffleMenu::InitialRaffleType()
{
	auto UIPtr = Cast<UVerticalBox>(GetWidgetFromName(FRaffleMenu::Get().RaffleType_VerticalBox));
	if (UIPtr)
	{
		auto ChildAry = UIPtr->GetAllChildren();
		URaffleType* FirstUIPtr = nullptr;
		for (auto Iter : ChildAry)
		{
			auto RafflePtr = Cast<URaffleType>(Iter);
			if (RafflePtr)
			{
				if (!FirstUIPtr)
				{
					FirstUIPtr = RafflePtr;
				}

				RafflePtr->UnSelect();
				auto Handle = RafflePtr->OnClicked.AddCallback(std::bind(&ThisClass::OnRaffleTypeSelected, this, std::placeholders::_1));
				Handle->bIsAutoUnregister = false;
			}
		}
		if (FirstUIPtr)
		{
			FirstUIPtr->Select();
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

void URaffleMenu::SetHoldItemProperty(const TSharedPtr<FSceneUnitContainer>& NewSPHoldItemPerperty)
{
	auto UIPtr = Cast<UCoinList>(GetWidgetFromName(FRaffleMenu::Get().CoinList));
	if (UIPtr)
	{
		UIPtr->ResetUIByData(NewSPHoldItemPerperty->GetCoinUintAry());
	}
}

void URaffleMenu::SwitchDisplay(bool bIsDisplayRaffleUI)
{
	{
		auto UIPtr = Cast<UCanvasPanel>(GetWidgetFromName(FRaffleMenu::Get().RaffleUI));
		if (UIPtr)
		{
			if (bIsDisplayRaffleUI)
			{
				UIPtr->SetVisibility(ESlateVisibility::Visible);
			}
			else
			{
//				UIPtr->SetVisibility(ESlateVisibility::Hidden);
				UIPtr->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
	{
		auto UIPtr = Cast<UCanvasPanel>(GetWidgetFromName(FRaffleMenu::Get().GetUnitUI));
		if (UIPtr)
		{
			if (bIsDisplayRaffleUI)
			{
				UIPtr->SetVisibility(ESlateVisibility::Hidden);
			}
			else
			{
				UIPtr->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
}

void URaffleMenu::ResetGetUnitAry(const TArray<FTableRowUnit*>& Ary)
{
	SwitchDisplay(false);
	auto HorizotalUIPtr = Cast<UHorizontalBox>(GetWidgetFromName(FRaffleMenu::Get().GetUnit_HorizaltalBox));
	if (HorizotalUIPtr)
	{
		HorizotalUIPtr->ClearChildren();

		for (const auto& Iter : Ary)
		{
			auto NewUnitPtr = CreateWidget<URaffle_Unit>(GetWorld(), Raffle_UnitClass);
			if (NewUnitPtr)
			{
				NewUnitPtr->ResetToolUIByData(Iter->UnitClass.GetDefaultObject());
				NewUnitPtr->ResetToolUIByData(Iter);
				HorizotalUIPtr->AddChild(NewUnitPtr);
			}
		}
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

void URaffleMenu::OnClickedConfirmGetUnitBtn()
{
	SwitchDisplay(true);
}
