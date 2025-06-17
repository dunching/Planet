#include "BackpackMenu.h"

#include <Kismet/GameplayStatics.h>
#include <Components/Button.h>

#include "BackpackIcon.h"
#include "Components/TileView.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Components/Border.h"
#include "StateTagExtendInfo.h"
#include "AssetRefMap.h"
#include "InventoryComponent.h"
#include "GenerateTypes.h"
#include "BackpackToolIcon.h"
#include "BackpackConsumableIcon.h"
#include "BackpackIconWrapper.h"
#include "GameplayTagsLibrary.h"
#include "UICommon.h"
#include "ItemProxy_Container.h"
#include "CharacterBase.h"
#include "CoinList.h"
#include "ItemProxy_Character.h"

struct FBackpackMenu : public TStructVariable<FBackpackMenu>
{
	const FName BackpackTile = TEXT("BackpackTile");

	const FName WeaponBtn = TEXT("WeaponBtn");

	const FName SkillBtn = TEXT("SkillBtn");

	const FName ConsumableBtn = TEXT("ConsumableBtn");

	const FName ShowAllBtn = TEXT("ShowAllBtn");

	const FName CoinList = TEXT("CoinList");
};

void UBackpackMenu::NativeConstruct()
{
	Super::NativeConstruct();

	BindEvent();
}

void UBackpackMenu::EnableMenu()
{
	ResetUIByData_All();

	auto UIPtr = Cast<UCoinList>(GetWidgetFromName(FBackpackMenu::Get().CoinList));
	if (!UIPtr)
	{
		return;
	}
	UIPtr->Enable();
}

void UBackpackMenu::DisEnableMenu()
{
	auto UIPtr = Cast<UCoinList>(GetWidgetFromName(FBackpackMenu::Get().CoinList));
	if (!UIPtr)
	{
		return;
	}
	UIPtr->DisEnable();
}

EMenuType UBackpackMenu::GetMenuType() const
{
	return EMenuType::kRaffle;
}

TArray<TSharedPtr<FBasicProxy>> UBackpackMenu::GetProxys() const
{
	TArray<TSharedPtr<FBasicProxy>> Result;
	if (CurrentProxyPtr && CurrentProxyPtr->GetCharacterActor().IsValid())
	{
		Result = CurrentProxyPtr->GetCharacterActor()->GetInventoryComponent()->GetProxys();
	}
	return Result;
}

void UBackpackMenu::ResetUIByData_Skill()
{
	auto TileViewPtr = Cast<UTileView>(GetWidgetFromName(FBackpackMenu::Get().BackpackTile));
	if (!TileViewPtr)
	{
		return;
	}

	TileViewPtr->ClearListItems();
	auto EntryClass = TileViewPtr->GetEntryWidgetClass();

	auto ItemAryRef = GetProxys();
	for (const auto& Iter : ItemAryRef)
	{
		if (!Iter)
		{
			continue;
		}
		if (
			Iter->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Active) ||
			Iter->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Passve)
		)
		{
			auto WidgetPtr = CreateWidget<UBackpackIconWrapper>(this, EntryClass);
			if (WidgetPtr)
			{
				WidgetPtr->TargetBasicProxyPtr = Iter;
				WidgetPtr->AllocationSkillsMenuPtr = AllocationSkillsMenuPtr;
				TileViewPtr->AddItem(WidgetPtr);
			}
		}
	}
}

void UBackpackMenu::ResetUIByData_Weapon()
{
	auto TileViewPtr = Cast<UTileView>(GetWidgetFromName(FBackpackMenu::Get().BackpackTile));
	if (!TileViewPtr)
	{
		return;
	}

	TileViewPtr->ClearListItems();
	auto EntryClass = TileViewPtr->GetEntryWidgetClass();
	auto ItemAryRef = GetProxys();
	for (const auto& Iter : ItemAryRef)
	{
		if (!Iter)
		{
			continue;
		}
		if (
			Iter->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Weapon)
		)
		{
			auto WidgetPtr = CreateWidget<UBackpackIconWrapper>(this, EntryClass);
			if (WidgetPtr)
			{
				WidgetPtr->TargetBasicProxyPtr = Iter;
				WidgetPtr->AllocationSkillsMenuPtr = AllocationSkillsMenuPtr;
				TileViewPtr->AddItem(WidgetPtr);
			}
		}
	}
}

void UBackpackMenu::ResetUIByData_Consumable()
{
	auto TileViewPtr = Cast<UTileView>(GetWidgetFromName(FBackpackMenu::Get().BackpackTile));
	if (!TileViewPtr)
	{
		return;
	}

	TileViewPtr->ClearListItems();
	auto EntryClass = TileViewPtr->GetEntryWidgetClass();
	auto ItemAryRef = GetProxys();
	for (const auto& Iter : ItemAryRef)
	{
		if (!Iter)
		{
			continue;
		}
		if (
			Iter->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Consumables)
		)
		{
			auto WidgetPtr = CreateWidget<UBackpackIconWrapper>(this, EntryClass);
			if (WidgetPtr)
			{
				WidgetPtr->TargetBasicProxyPtr = Iter;
				WidgetPtr->AllocationSkillsMenuPtr = AllocationSkillsMenuPtr;
				TileViewPtr->AddItem(WidgetPtr);
			}
		}
	}
}

void UBackpackMenu::ResetUIByData_All()
{
	auto TileViewPtr = Cast<UTileView>(GetWidgetFromName(FBackpackMenu::Get().BackpackTile));
	if (!TileViewPtr)
	{
		return;
	}

	TileViewPtr->ClearListItems();
	auto EntryClass = TileViewPtr->GetEntryWidgetClass();
	auto ItemAryRef = GetProxys();
	for (const auto& Iter : ItemAryRef)
	{
		if (!Iter)
		{
			continue;
		}
		if (
			Iter->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Weapon) ||
			Iter->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Active) ||
			Iter->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Passve) ||
			Iter->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Tool) ||
			Iter->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Consumables) ||
			Iter->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Material)
		)
		{
			auto WidgetPtr = CreateWidget<UBackpackIconWrapper>(this, EntryClass);
			if (WidgetPtr)
			{
				WidgetPtr->TargetBasicProxyPtr = Iter;
				WidgetPtr->AllocationSkillsMenuPtr = AllocationSkillsMenuPtr;
				TileViewPtr->AddItem(WidgetPtr);
			}
		}
	}
}

void UBackpackMenu::ResetProxys(
	const TSet<FGameplayTag>& TargetProxyTypeTag
	)
{
	auto TileViewPtr = Cast<UTileView>(GetWidgetFromName(FBackpackMenu::Get().BackpackTile));
	if (!TileViewPtr)
	{
		return;
	}

	TileViewPtr->ClearListItems();
	
	auto EntryClass = TileViewPtr->GetEntryWidgetClass();
	auto ItemAryRef = GetProxys();
	
	for (const auto& Iter : ItemAryRef)
	{
		if (!Iter)
		{
			continue;
		}
		for (const auto &SecondIter: TargetProxyTypeTag)
		{
			if (
				Iter->GetProxyType().MatchesTag(SecondIter)
			)
			{
				auto WidgetPtr = CreateWidget<UBackpackIconWrapper>(this, EntryClass);
				if (WidgetPtr)
				{
					WidgetPtr->TargetBasicProxyPtr = Iter;
					WidgetPtr->AllocationSkillsMenuPtr = AllocationSkillsMenuPtr;
					TileViewPtr->AddItem(WidgetPtr);
				}
				break;
			}
		}
	}
}

void UBackpackMenu::BindEvent()
{
	{
		auto BtnPtr = Cast<UButton>(GetWidgetFromName(FBackpackMenu::Get().WeaponBtn));
		if (BtnPtr)
		{
			BtnPtr->OnClicked.AddDynamic(this, &ThisClass::OnWeaponBtnCliked);
		}
	}
	{
		auto BtnPtr = Cast<UButton>(GetWidgetFromName(FBackpackMenu::Get().SkillBtn));
		if (BtnPtr)
		{
			BtnPtr->OnClicked.AddDynamic(this, &ThisClass::OnSkillBtnCliked);
		}
	}
	{
		auto BtnPtr = Cast<UButton>(GetWidgetFromName(FBackpackMenu::Get().ConsumableBtn));
		if (BtnPtr)
		{
			BtnPtr->OnClicked.AddDynamic(this, &ThisClass::OnConsumableBtnCliked);
		}
	}
	{
		if (MaterialBtn)
		{
			MaterialBtn->OnClicked.AddDynamic(this, &ThisClass::OnMaterialBtnCliked);
		}
	}
	{
		auto BtnPtr = Cast<UButton>(GetWidgetFromName(FBackpackMenu::Get().ShowAllBtn));
		if (BtnPtr)
		{
			BtnPtr->OnClicked.AddDynamic(this, &ThisClass::OnShowAllBtnCliked);
		}
	}
}

void UBackpackMenu::OnSkillBtnCliked()
{
	ResetUIByData_Skill();
}

void UBackpackMenu::OnConsumableBtnCliked()
{
	ResetUIByData_Consumable();
}

void UBackpackMenu::OnMaterialBtnCliked()
{
	ResetProxys({UGameplayTagsLibrary::Proxy_Material});
}

void UBackpackMenu::OnShowAllBtnCliked()
{
	ResetUIByData_All();
}

void UBackpackMenu::OnWeaponBtnCliked()
{
	ResetUIByData_Weapon();
}
