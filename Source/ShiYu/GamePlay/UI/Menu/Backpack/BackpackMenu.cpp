
#include "BackpackMenu.h"

#include <Kismet/GameplayStatics.h>

#include "BackpackIcon.h"
#include "Components/TileView.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Components/Border.h"
#include <AssetRefrencePath.h>
#include "AssetRefMap.h"

void UBackpackMenu::SetHoldItemProperty(const TSharedPtr<FHoldItemsData>& NewSPHoldItemPerperty)
{
	SPHoldItemPerpertyPtr = NewSPHoldItemPerperty;
}

void UBackpackMenu::NativeConstruct()
{
	Super::NativeConstruct();
}

void UBackpackMenu::ResetFiled()
{
	Super::ResetFiled();

	auto TilePtr = Cast<UTileView>(GetWidgetFromName(TEXT("BackpackTile")));
	if (TilePtr)
	{
		TilePtr->ClearListItems();

		auto BaseItemClassPtr = UAssetRefMap::GetInstance()->BackpackIconClass;

		if (!BaseItemClassPtr)
		{
			return;
		}

		for (auto Iter : SPHoldItemPerpertyPtr->GetItemsAry().ItemsAry)
		{
			if (!CheckItemNumIsValid(Iter))
			{
				continue;
			}
			auto ItemPtr = CreateWidget<UBackpackIcon>(this, BaseItemClassPtr);
			if (!ItemPtr)
			{
				continue;
			}

			ItemPtr->ResetUIByData(Iter);
			TilePtr->AddItem(ItemPtr);
		}
	}
}

void UBackpackMenu::SetPutPostion(EPutPostion PutPostion)
{
	switch (PutPostion)
	{
	case UBackpackMenu::EPutPostion::kLeft:
	{
		 auto UBorderPtr = Cast<UBorder>(GetWidgetFromName(TEXT("ItemBorder")));
		 if (UBorderPtr)
		 {
			 auto GridPanelPtr = Cast<UGridPanel>(GetWidgetFromName(TEXT("GridPanel")));
			 if (GridPanelPtr)
			 {
				 auto CurrentSlotPtr =  Cast<UGridSlot>(UBorderPtr->Slot);
				 if (CurrentSlotPtr)
				 {
					 CurrentSlotPtr->SetColumn(0);
				 }
			 }
		 }
	}
		break;
	case UBackpackMenu::EPutPostion::kRight:
	{
		auto UBorderPtr = Cast<UBorder>(GetWidgetFromName(TEXT("ItemBorder")));
		if (UBorderPtr)
		{
			auto GridPanelPtr = Cast<UGridPanel>(GetWidgetFromName(TEXT("GridPanel")));
			if (GridPanelPtr)
			{
				auto CurrentSlotPtr = Cast<UGridSlot>(UBorderPtr->Slot);
				if (CurrentSlotPtr)
				{
					CurrentSlotPtr->SetColumn(1);
				}
			}
		}
	}
		break;
	default:
		break;
	}
}
