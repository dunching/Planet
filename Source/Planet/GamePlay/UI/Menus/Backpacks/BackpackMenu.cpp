
#include "BackpackMenu.h"

#include <Kismet/GameplayStatics.h>

#include "BackpackIcon.h"
#include "Components/TileView.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Components/Border.h"
#include <AssetRefrencePath.h>
#include "AssetRefMap.h"
#include "HoldingItemsComponent.h"
#include "GenerateType.h"

namespace BackpackMenu
{
	const FName BackpackTile = TEXT("BackpackTile");
}

void UBackpackMenu::NativeConstruct()
{
	Super::NativeConstruct();

	ResetUIByData();
}

void UBackpackMenu::SetHoldItemProperty(const FSceneToolsContainer& NewSPHoldItemPerperty)
{
	SPHoldItemPerpertyPtr = NewSPHoldItemPerperty;
}

void UBackpackMenu::ResetUIByData()
{
	auto TileViewPtr = Cast<UTileView>(GetWidgetFromName(BackpackMenu::BackpackTile));
	if (!TileViewPtr)
	{
		return;
	}

	TileViewPtr->ClearListItems();
	auto EntryClass = TileViewPtr->GetEntryWidgetClass();
	auto ItemAryRef = SPHoldItemPerpertyPtr.GetSceneUintAry();
	for (const auto& Iter : ItemAryRef)
	{
		if (!Iter)
		{
			continue;
		}
		if (Iter->GetSceneToolsType() == ESceneToolsType::kTool)
		{
			auto WidgetPtr = CreateWidget<UBackpackIcon>(this, EntryClass);
			if (WidgetPtr)
			{
				TileViewPtr->AddItem(WidgetPtr);
				WidgetPtr->ResetToolUIByData(Iter);
			}
		}
	}
}
