#include "ItemDetails.h"

#include "GameplayTagsLibrary.h"
#include "Components/VerticalBox.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetStringLibrary.h"
#include "Components/WidgetSwitcher.h"
#include "Components/ScrollBox.h"

#include "ItemInteractionItem.h"
#include "PropertyEntryDescription.h"

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

					Text = Text.Replace(
					                    *Iter.Key,
					                    *UKismetStringLibrary::Conv_IntToString(Iter.Value.PerLevelValue[0])
					                   );
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

		// 如果是被动机能的话，需要特殊显示的内容
		// 这里是否要扩展？
		if (PropertyEntrysBox)
		{
			PropertyEntrysBox->SetVisibility(ESlateVisibility::Hidden);
		}
		if (ProxySPtr->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Passve))
		{
			auto SkillProxySPtr = DynamicCastSharedPtr<FPassiveSkillProxy>(ProxySPtr);
			if (SkillProxySPtr)
			{
				if (PropertyEntrysBox)
				{
					PropertyEntrysBox->SetVisibility(ESlateVisibility::Visible);
					PropertyEntrysBox->ClearChildren();

					for (const auto& Iter : SkillProxySPtr->GeneratedPropertyEntryAry)
					{
						auto UIPtr = CreateWidget<UPropertyEntryDescription>(this, PropertyEntryDescriptionClass);
						if (UIPtr)
						{
							UIPtr->SetDta(Iter.second);
							PropertyEntrysBox->AddChild(UIPtr);
						}
					}
				}
			}
			else
			{
			}
		}
	}
	else
	{
		Reset();
	}
}
