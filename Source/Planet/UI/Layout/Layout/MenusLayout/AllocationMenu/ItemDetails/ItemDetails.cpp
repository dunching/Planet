
#include "ItemDetails.h"

#include "Components/VerticalBox.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetStringLibrary.h"

#include "ItemInteractionItem.h"
#include "Components/WidgetSwitcher.h"

void UItemDetails::Reset()
{
	if (WidgetSwitcher)
	{
		WidgetSwitcher->SetActiveWidgetIndex(0);
	}
}

void UItemDetails::BindData(
	const TSharedPtr<FBasicProxy>& ProxySPtr
	)
{
	if (ProxySPtr)
	{
		if (ProxySPtr == PreviousProxySPtr)
		{
			return;
		}

		if (WidgetSwitcher)
		{
			WidgetSwitcher->SetActiveWidgetIndex(1);
		}
		
		if (NameText)
		{
			NameText->SetText(FText::FromString(ProxySPtr->GetProxyName()));
		}

		if (DescriptionText)
		{
			const auto ItemProxy_DescriptionPtr = ProxySPtr->GetItemProxy_Description();
			if (ItemProxy_DescriptionPtr->DecriptionText.IsValidIndex(0))
			{
				FString Text = ItemProxy_DescriptionPtr->DecriptionText[0];
				for (const auto& Iter : ItemProxy_DescriptionPtr->Values)
				{
					if (Iter.Value.PerLevelValue.IsEmpty())
					{
						continue;
					}

					Text = Text.Replace(*Iter.Key, *UKismetStringLibrary::Conv_IntToString(Iter.Value.PerLevelValue[0]));
				}

				DescriptionText->SetText(FText::FromString(Text));
			}
		}

		InteractionVerticalBox->ClearChildren();
		const auto InteractionsTypeSet = ProxySPtr->GetInteractionsType();
		if (InteractionsTypeSet.IsEmpty())
		{
		}
		else
		{
			for (const auto& Iter : InteractionsTypeSet)
			{
				auto InteractionItemPtr = CreateWidget<UItemInteractionItem>(this, InteractionItemClass);
				if (InteractionItemPtr)
				{
					InteractionItemPtr->SetData(ProxySPtr, Iter);
					
					InteractionVerticalBox->AddChild(InteractionItemPtr);
				}
			}
		}
	}
	else
	{
		Reset();
	}
}
