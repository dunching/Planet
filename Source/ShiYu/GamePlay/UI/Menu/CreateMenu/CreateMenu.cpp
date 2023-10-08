
#include "CreateMenu.h"

#include <Kismet/GameplayStatics.h>

#include "CreateQueue.h"
#include "Components/TileView.h"
#include "CreateItem.h"
#include "Pawn/HoldItemComponent.h"
#include <AssetRefrencePath.h>
#include "AssetRefMap.h"

void UCreateMenu::InitCreateQueue()
{
	CreateQueuePtr = Cast<UCreateQueue>(GetWidgetFromName(TEXT("CreateQueue_BP")));
	if (CreateQueuePtr)
	{
		CreateQueuePtr->SetCreateQueueMG(SPCreateQueueMGPtr);
	}
}

void UCreateMenu::InitCreateTile()
{
	auto TilePtr = Cast<UTileView>(GetWidgetFromName(TEXT("CreateTile")));
	if (TilePtr)
	{
		TilePtr->ClearListItems();

		auto BaseItemClassPtr = UAssetRefMap::GetInstance()->CreateItemClass;

		if (!BaseItemClassPtr)
		{
			return;
		}

// 		for (auto Iter : SPHoldItemPerpertyPtr->GetCanCreateMap())
// 		{
// 			auto ItemPtr = CreateWidget<UCreateItem>(this, BaseItemClassPtr);
// 			if (ItemPtr)
// 			{
// 				ItemPtr->SetOnCreateItem(std::bind(&UCreateQueue::AddCreateItem, CreateQueuePtr, std::placeholders::_1));
// 				ItemPtr->SetItemType(Iter);
// 				TilePtr->AddItem(ItemPtr);
// 			}
// 		}
	}
}

void UCreateMenu::NativeConstruct()
{
	Super::NativeConstruct();

	SPCreateQueueMGPtr = FCreateQueueMG::CreateInst();

	InitCreateQueue();

	InitCreateTile();
}

void UCreateMenu::ResetFiled()
{
	Super::ResetFiled();

	auto TilePtr = Cast<UTileView>(GetWidgetFromName(TEXT("CreateTile")));
	if (TilePtr)
	{
		for (int32 Index = 0; Index < TilePtr->GetNumItems(); Index++)
		{
			auto ItemPtr = Cast<UCreateItem>(TilePtr->GetItemAt(Index));
			if (ItemPtr)
			{
//				auto ItemType = ItemPtr->GetItemType();

//				ItemPtr->IsAbleCreateItem();
			}
		}
	}
}

void UCreateMenu::SetHoldItemProperty(TSharedPtr<FHoldItemsData>NewHoldItemPerpertyPtr)
{
	SPHoldItemPerpertyPtr = NewHoldItemPerpertyPtr;
}
