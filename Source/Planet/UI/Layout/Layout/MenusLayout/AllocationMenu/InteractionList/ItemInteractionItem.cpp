#include "ItemInteractionItem.h"

#include "Components/TextBlock.h"

#include "InventoryComponent.h"
#include "PlanetPlayerController.h"

FReply UItemInteractionItem::NativeOnMouseButtonDown(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent
	)
{
	auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
	if (PCPtr)
	{
		PCPtr->GetInventoryComponent()->ProcessProxyInteraction(ProxySPtr->GetID(), ItemProxyInteractionType);
	}

	return FReply::Handled();
}

inline void UItemInteractionItem::SetData(
	const TSharedPtr<FBasicProxy>& InProxySPtr,
	EItemProxyInteractionType InItemProxyInteractionType
	)
{
	ProxySPtr = InProxySPtr;
	ItemProxyInteractionType = InItemProxyInteractionType;

	if (!TextBlock)
	{
		return;
	}

	switch (ItemProxyInteractionType)
	{
	case EItemProxyInteractionType::kDiscard:
		{
			TextBlock->SetText(FText::FromString(TEXT("丢弃")));
		}
		break;
	case EItemProxyInteractionType::kBreakDown:
		{
			TextBlock->SetText(FText::FromString(TEXT("分解")));
		}
		break;
	case EItemProxyInteractionType::kUpgrade:
		{
			TextBlock->SetText(FText::FromString(TEXT("升级")));
		}
		break;
	}
}
