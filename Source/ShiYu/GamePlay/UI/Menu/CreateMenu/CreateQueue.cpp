
#include "CreateQueue.h"

#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include <Kismet/GameplayStatics.h>

#include "CreateQueueItem.h"
#include "PlayerState/ShiYuPlayerState.h"
#include "CreateQueue/CreateQueueMG.h"
#include "Pawn/HoldItemComponent.h"
#include <AssetRefrencePath.h>
#include "AssetRefMap.h"

UCreateQueue::UCreateQueue(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{

}

void UCreateQueue::NativeConstruct()
{
	Super::NativeConstruct();

	auto BtnPtr = Cast<UButton>(GetWidgetFromName(TEXT("CancelBtn")));
	if (BtnPtr)
	{
		BtnPtr->OnClicked.AddDynamic(this, &UCreateQueue::OnAllCancelBtnClick);
	}

	auto ScrollBoxPtr = Cast<UScrollBox>(GetWidgetFromName(TEXT("CreateScrollBox")));
	if (ScrollBoxPtr)
	{
		ScrollBoxPtr->ClearChildren();
	}
}

UCreateQueueItem* UCreateQueue::AddCreateQueueItem(FItemNum NewItemsType)
{
	auto WidgetClass = UAssetRefMap::GetInstance()->CreateQueueItemClass;
	auto CreateQueueItemPtr = CreateWidget<UCreateQueueItem>(this, WidgetClass);
	if (CreateQueueItemPtr)
	{
		CreateQueueItemPtr->SetItemType(NewItemsType);
		CreateQueueItemPtr->SetCreateQueueMG(SPCreateQueueMGPtr);
		CreateQueueItemPtr->SetHoldItemProperty(SPHoldItemPerpertyPtr);

		auto ScrollBoxPtr = Cast<UScrollBox>(GetWidgetFromName(TEXT("CreateScrollBox")));
		if (ScrollBoxPtr)
		{
			ScrollBoxPtr->AddChild(CreateQueueItemPtr);
		}
		return CreateQueueItemPtr;
	}

	return nullptr;
}

void UCreateQueue::OnAllCancelBtnClick()
{
	if (SPCreateQueueMGPtr) 
	{
		SPCreateQueueMGPtr->Clear();
	}
}

void UCreateQueue::AddCreateItem(FItemNum ItemsType)
{
	AddCreateQueueItem(ItemsType);
}

void UCreateQueue::SetCreateQueueMG(const TSharedPtr<FCreateQueueMG>& NewSPCreateQueueMGPtr)
{
	SPCreateQueueMGPtr = NewSPCreateQueueMGPtr;
};
