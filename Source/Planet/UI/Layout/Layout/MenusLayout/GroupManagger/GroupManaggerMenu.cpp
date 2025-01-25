
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
#include "GroupSharedInfo.h"
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
	auto PCPtr =
		Cast<IPlanetControllerInterface>(UGameplayStatics::GetPlayerController(this, 0));
	if (!PCPtr)
	{
		return;
	}

	auto TeamMatesListPtr = Cast<UTeamMatesList>(GetWidgetFromName(GroupManaggerMenu::TeamMatesList));
	if (TeamMatesListPtr)
	{
		TeamMatesListPtr->SyncData();
	}

	auto GMCPtr = PCPtr->GetGroupSharedInfo();
	GMCPtr->GetTeamMatesHelperComponent()->SpwanTeammateCharacter();
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

	auto PCPtr = Cast<IPlanetControllerInterface>(UGameplayStatics::GetPlayerController(this, 0));
	if (!PCPtr)
	{
		return;
	}

	auto GMCPtr = PCPtr->GetGroupSharedInfo();
	auto HICPtr = PCPtr->GetHoldingItemsComponent();

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
