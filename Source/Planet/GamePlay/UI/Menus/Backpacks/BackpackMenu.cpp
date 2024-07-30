
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
#include "HoldingItemsComponent.h"
#include "GenerateType.h"
#include "BackpackToolIcon.h"
#include "BackpackConsumableIcon.h"
#include "BackpackIconWrapper.h"
#include "GameplayTagsSubSystem.h"
#include "UICommon.h"

struct FBackpackMenu : public TGetSocketName<FBackpackMenu>
{
	const FName BackpackTile = TEXT("BackpackTile");

	const FName WeaponBtn = TEXT("WeaponBtn");

	const FName SkillBtn = TEXT("SkillBtn");
};

void UBackpackMenu::NativeConstruct()
{
	Super::NativeConstruct();

	ResetUIByData();
}

void UBackpackMenu::SetHoldItemProperty(
	const FSceneUnitContainer& InPlayerStateSceneUnitContariner,
	const FSceneUnitContainer& InCharacterSceneUnitContariner
)
{
	PlayerStateSceneUnitContariner = InPlayerStateSceneUnitContariner;
	CharacterSceneUnitContariner = InCharacterSceneUnitContariner;
}

void UBackpackMenu::ResetUIByData()
{
	ResetUIByData_All();
}

void UBackpackMenu::ResetUIByData_Skills()
{
	auto TileViewPtr = Cast<UTileView>(GetWidgetFromName(FBackpackMenu::Get().BackpackTile));
	if (!TileViewPtr)
	{
		return;
	}

	TileViewPtr->ClearListItems();
	auto EntryClass = TileViewPtr->GetEntryWidgetClass();
	auto ItemAryRef = PlayerStateSceneUnitContariner.GetSceneUintAry();
	for (const auto& Iter : ItemAryRef)
	{
		if (!Iter)
		{
			continue;
		}
		if (
			Iter->GetUnitType().MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill)
			)
		{
			auto WidgetPtr = CreateWidget<UBackpackIconWrapper>(this, EntryClass);
			if (WidgetPtr)
			{
				WidgetPtr->TargetBasicUnitPtr = Iter;
				WidgetPtr->OnDragSkillIconDelegate = OnDragSkillIconDelegate;
				TileViewPtr->AddItem(WidgetPtr);
			}
		}
	}
}

void UBackpackMenu::ResetUIByData_Weapons()
{
	auto TileViewPtr = Cast<UTileView>(GetWidgetFromName(FBackpackMenu::Get().BackpackTile));
	if (!TileViewPtr)
	{
		return;
	}

	TileViewPtr->ClearListItems();
	auto EntryClass = TileViewPtr->GetEntryWidgetClass();
	auto ItemAryRef = PlayerStateSceneUnitContariner.GetSceneUintAry();
	for (const auto& Iter : ItemAryRef)
	{
		if (!Iter)
		{
			continue;
		}
		if (
			Iter->GetUnitType().MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Weapon)
			)
		{
			auto WidgetPtr = CreateWidget<UBackpackIconWrapper>(this, EntryClass);
			if (WidgetPtr)
			{
				WidgetPtr->TargetBasicUnitPtr = Iter;
				WidgetPtr->OnDragWeaponIconDelegate = OnDragWeaponIconDelegate;
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
	auto ItemAryRef = PlayerStateSceneUnitContariner.GetSceneUintAry();
	for (const auto& Iter : ItemAryRef)
	{
		if (!Iter)
		{
			continue;
		}
		if (
			Iter->GetUnitType().MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Weapon) ||
			Iter->GetUnitType().MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill) ||
			Iter->GetUnitType().MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Tool) ||
			Iter->GetUnitType().MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Consumables)
			)
		{
			auto WidgetPtr = CreateWidget<UBackpackIconWrapper>(this, EntryClass);
			if (WidgetPtr)
			{
				WidgetPtr->TargetBasicUnitPtr = Iter;
				WidgetPtr->OnDragSkillIconDelegate = OnDragSkillIconDelegate;
				WidgetPtr->OnDragWeaponIconDelegate = OnDragWeaponIconDelegate;
				TileViewPtr->AddItem(WidgetPtr);
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
			BtnPtr->OnClicked.AddDynamic(this, &ThisClass::OnWeaponsBtnCliked);
		}
	}
	{
		auto BtnPtr = Cast<UButton>(GetWidgetFromName(FBackpackMenu::Get().SkillBtn));
		if (BtnPtr)
		{
			BtnPtr->OnClicked.AddDynamic(this, &ThisClass::OnSkillsBtnCliked);
		}
	}
}

void UBackpackMenu::OnSkillsBtnCliked()
{
	ResetUIByData_Skills();
}

void UBackpackMenu::OnWeaponsBtnCliked()
{
	ResetUIByData_Weapons();
}
