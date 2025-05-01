
#include "GroupManaggerMenu.h"

#include <Kismet/GameplayStatics.h>

#include "BackpackIcon.h"
#include "Components/TileView.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Components/Border.h"
#include "StateTagExtendInfo.h"
#include "AssetRefMap.h"
#include "InventoryComponent.h"
#include "GenerateType.h"
#include "TeamMatesHelperComponent.h"
#include "CharacterBase.h"
#include "GroupMateInfo.h"
#include "PlanetControllerInterface.h"
#include "ItemProxy_Minimal.h"
#include "HumanCharacter.h"
#include "GameplayTagsLibrary.h"
#include "GroupManagger.h"
#include "HumanCharacter_Player.h"
#include "ItemProxy_Character.h"
#include "TeamMatesList.h"

namespace GroupManaggerMenu
{
	const FName GroupMatesTileView = TEXT("GroupMatesTileView");

	const FName TeamMatesList = TEXT("TeamMatesList");
}

void UGroupManaggerMenu::NativeConstruct()
{
	Super::NativeConstruct();

	ResetUIByData();
}

void UGroupManaggerMenu::ResetUIByData()
{
	ResetGroupmates();
}

void UGroupManaggerMenu::SyncData()
{
	auto TeamMatesListPtr = Cast<UTeamMatesList>(GetWidgetFromName(GroupManaggerMenu::TeamMatesList));
	if (TeamMatesListPtr)
	{
		TeamMatesListPtr->SyncData();
	}
}

EMenuType UGroupManaggerMenu::GetMenuType() const
{
	return EMenuType::kGroupManagger;
}

void UGroupManaggerMenu::ResetGroupmates()
{
	auto TileViewPtr = Cast<UTileView>(GetWidgetFromName(GroupManaggerMenu::GroupMatesTileView));
	if (!TileViewPtr)
	{
		return;
	}

	TileViewPtr->ClearListItems();
	auto EntryClass = TileViewPtr->GetEntryWidgetClass();

	auto PlayerCharacterPtr = Cast<AHumanCharacter_Player>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!PlayerCharacterPtr)
	{
		return;
	}

	auto GroupManaggerPtr = PlayerCharacterPtr->GetGroupManagger();
	auto HICPtr = GroupManaggerPtr->GetInventoryComponent();

	auto CharacterProxyAry = HICPtr->GetCharacterProxyAry();
	for (auto Iter : CharacterProxyAry)
	{
		if (UGameplayTagsLibrary::Proxy_Character_Player == Iter->GetProxyType())
		{
			continue;
		}
		auto WidgetPtr = CreateWidget<UGroupMateInfo>(this, EntryClass);
		if (WidgetPtr)
		{
			TileViewPtr->AddItem(WidgetPtr);

			WidgetPtr->ResetToolUIByData(Iter);
		}
	}
}
