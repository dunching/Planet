
#include "CreateQueueItem.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Button.h"
#include <Kismet/GameplayStatics.h>

#include "ToolsLibrary.h"
#include "PlayerState/ShiYuPlayerState.h"
#include "CreateQueue/CreateQueueMG.h"
#include "Pawn/HoldItemComponent.h"
#include "CreateQueue/CreateRequest.h"

#include <CacheAssetManager.h>

UCreateQueueItem::UCreateQueueItem(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{

}

void UCreateQueueItem::NativeConstruct()
{
	Super::NativeConstruct();

	auto BtnPtr = Cast<UButton>(GetWidgetFromName(TEXT("CancelBtn")));
	if (BtnPtr)
	{
		BtnPtr->OnClicked.AddDynamic(this, &UCreateQueueItem::OnCancelBtnClick);
	}

	auto ProgressBarPtr = Cast<UProgressBar>(GetWidgetFromName(TEXT("ProgressBar")));
	if (ProgressBarPtr)
	{
		ProgressBarPtr->SetPercent(0);
	}

	if (SPCreateQueueMGPtr.IsValid())
	{
		SPCreateRequestPtr = SPCreateQueueMGPtr->Add(ItemsType.ItemType);
		SPCreateRequestPtr->SetUpdateCreateStateCB(std::bind(
			&UCreateQueueItem::UpdateProgress, this,
			std::placeholders::_1, std::placeholders::_2
		));
	}
}

void UCreateQueueItem::SetItemType(FItemNum NewItemsType)
{
	ItemsType = NewItemsType;

	auto ImagePtr = Cast<UImage>(GetWidgetFromName(TEXT("Image")));
	if (ImagePtr)
	{
		auto CacheAssetManagerPtr = UCacheAssetManager::GetInstance();
		CacheAssetManagerPtr->GetTextureByItemType(ItemsType, [ImagePtr](auto TexturePtr)
			{
			ImagePtr->SetBrushFromTexture(TexturePtr);
			});
	}
}

FItemNum UCreateQueueItem::GetItemType() const
{
	return ItemsType;
}

void UCreateQueueItem::UpdateProgress(ECreateState CreateState, float Progress)
{
	switch (CreateState)
	{
	case ECreateState::kStart:
	{
		SPHoldItemPerpertyPtr->CreatedItem(ItemsType);
	}
	break;
	case ECreateState::kCreating:
	{
		auto ProgressBarPtr = Cast<UProgressBar>(GetWidgetFromName(TEXT("ProgressBar")));
		if (ProgressBarPtr)
		{
			ProgressBarPtr->SetPercent(Progress);
		}
	}
	break;
	case ECreateState::kPause:
		break;
	case ECreateState::kSuc:
	{
//		SPHoldItemPerpertyPtr->AddItem(ItemsType);
		OnCreateComplete();
	}
	break;
	case ECreateState::kCancel:
	case ECreateState::kFail:
	{
	//	SPHoldItemPerpertyPtr->UnCreatedItem(ItemsType);
		OnCreateComplete();
	}
		break;
	default:
		break;
	}
}

void UCreateQueueItem::OnCancelBtnClick()
{
	if (SPCreateRequestPtr.IsValid())
	{
		SPCreateRequestPtr->Cancel();
	}
}

void UCreateQueueItem::SetCreateQueueMG(const TSharedPtr<FCreateQueueMG>& NewSPCreateQueueMGPtr)
{
	SPCreateQueueMGPtr = NewSPCreateQueueMGPtr;
}

void UCreateQueueItem::OnCreateComplete()
{
	RemoveFromParent();
}

void UCreateQueueItem::SetHoldItemProperty(const TSharedPtr<FHoldItemsData>& NewSPHoldItemPerperty)
{
	SPHoldItemPerpertyPtr = NewSPHoldItemPerperty;
}
