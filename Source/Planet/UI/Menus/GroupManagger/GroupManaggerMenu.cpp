
#include "GroupManaggerMenu.h"

#include <Kismet/GameplayStatics.h>

#include "BackpackIcon.h"
#include "Components/TileView.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Components/Border.h"
#include "StateTagExtendInfo.h"
#include "AssetRefMap.h"
#include "HoldingItemsComponent.h"
#include "GenerateType.h"
#include "GroupMnaggerComponent.h"
#include "CharacterBase.h"
#include "GroupMateInfo.h"
#include "PlanetControllerInterface.h"
#include "SceneElement.h"
#include "HumanCharacter.h"

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

	auto GMCPtr = PCPtr->GetGroupMnaggerComponent();

	auto GroupsHelperSPtr = GMCPtr->GetGroupHelper();
	if (GroupsHelperSPtr)
	{
		for (auto Iter : GroupsHelperSPtr->MembersSet)
		{
			if (Iter == GroupsHelperSPtr->OwnerCharacterUnitPtr)
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
}
