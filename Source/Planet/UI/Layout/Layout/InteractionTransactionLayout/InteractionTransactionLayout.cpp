#include "InteractionTransactionLayout.h"

#include "AIComponent.h"
#include "AssetRefMap.h"
#include "CharacterAbilitySystemComponent.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

#include "GameplayCommand.h"
#include "GameplayTagsLibrary.h"
#include "GoodsItem.h"
#include "GuideSubSystem.h"
#include "HumanCharacter_Player.h"
#include "GuideThreadChallenge.h"
#include "HumanInteractionWithNPC.h"
#include "HumanRegularProcessor.h"
#include "InputProcessorSubSystem.h"
#include "InventoryComponent.h"
#include "LayoutCommon.h"
#include "PlanetPlayerController.h"
#include "PlayerGameplayTasks.h"
#include "UIManagerSubSystem.h"
#include "Components/CanvasPanel.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/TileView.h"
#include "Kismet/KismetStringLibrary.h"

struct FTransactionLayout : public TStructVariable<FTransactionLayout>
{
	FName QuitBtn = TEXT("QuitBtn");

	FName AddBtn = TEXT("AddBtn");

	FName SubBtn = TEXT("SubBtn");

	FName MaxBtn = TEXT("MaxBtn");

	FName BuyBtn = TEXT("BuyBtn");

	FName ItemName = TEXT("ItemName");

	FName ItemDescription = TEXT("ItemDescription");

	FName EditableTextBox = TEXT("EditableTextBox");

	FName Value = TEXT("Value");

	FName TileView = TEXT("TileView");

	FName ItemDetails = TEXT("ItemDetails");
};

void UInteractionTransactionLayout::NativeConstruct()
{
	Super::NativeConstruct();

	{
		auto UIPtr = Cast<UButton>(GetWidgetFromName(FTransactionLayout::Get().QuitBtn));
		if (UIPtr)
		{
			UIPtr->OnClicked.AddDynamic(this, &ThisClass::OnQuitClicked);
		}
	}
	{
		auto UIPtr = Cast<UButton>(GetWidgetFromName(FTransactionLayout::Get().BuyBtn));
		if (UIPtr)
		{
			UIPtr->OnClicked.AddDynamic(this, &ThisClass::OnBuyClicked);
		}
	}
	{
		auto UIPtr = Cast<UButton>(GetWidgetFromName(FTransactionLayout::Get().AddBtn));
		if (UIPtr)
		{
			UIPtr->OnClicked.AddDynamic(this, &ThisClass::OnAddClicked);
		}
	}
	{
		auto UIPtr = Cast<UButton>(GetWidgetFromName(FTransactionLayout::Get().SubBtn));
		if (UIPtr)
		{
			UIPtr->OnClicked.AddDynamic(this, &ThisClass::OnSubClicked);
		}
	}
	{
		auto UIPtr = Cast<UButton>(GetWidgetFromName(FTransactionLayout::Get().MaxBtn));
		if (UIPtr)
		{
			UIPtr->OnClicked.AddDynamic(this, &ThisClass::OnMaxClicked);
		}
	}
	{
		auto UIPtr = Cast<UEditableTextBox>(GetWidgetFromName(FTransactionLayout::Get().EditableTextBox));
		if (UIPtr)
		{
			UIPtr->OnTextChanged.AddDynamic(this, &ThisClass::OnEditableTextBoxChangedEvent);
		}
	}
	auto CoinSPtr = Cast<APlanetPlayerController>(UGameplayStatics::GetPlayerController(this, 0))->
					GetInventoryComponent()->FindProxy_Coin(UGameplayTagsLibrary::Proxy_Coin_Regular);
	if (CoinSPtr)
	{
		OnCoinChangedDelegateHandle = CoinSPtr->CallbackContainerHelper.AddOnValueChanged(std::bind(&ThisClass::OnCoinChanged, this, std::placeholders::_1, std::placeholders::_2));
	}
}

void UInteractionTransactionLayout::Enable()
{
	ILayoutInterfacetion::Enable();

	CurrentProxyPtr = nullptr;

	{
		auto UIPtr = Cast<UCanvasPanel>(GetWidgetFromName(FTransactionLayout::Get().ItemDetails));
		if (UIPtr)
		{
			UIPtr->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	auto CurrentActionSPtr =
		DynamicCastSharedPtr<HumanProcessor::FHumanInteractionWithNPCProcessor>(
		                                                                        UInputProcessorSubSystem::GetInstance()
		                                                                        ->GetCurrentAction()
		                                                                       );

	if (CurrentActionSPtr)
	{
		CharacterPtr = CurrentActionSPtr->CharacterPtr;

		auto UIPtr = Cast<UTileView>(GetWidgetFromName(FTransactionLayout::Get().TileView));
		if (UIPtr)
		{
			UIPtr->ClearListItems();
			auto EnteryClass = UIPtr->GetEntryWidgetClass();

			const auto SaleItemsInfo = CharacterPtr->GetAIComponent()->GetSaleItemsInfo();
			auto Proxys = CharacterPtr->GetInventoryComponent()->GetProxys(UGameplayTagsLibrary::Proxy_Weapon);
			Proxys.Append(CharacterPtr->GetInventoryComponent()->GetProxys(UGameplayTagsLibrary::Proxy_Consumables));
			for (const auto& Iter : Proxys)
			{
				auto ChildPtr = CreateWidget<UGoodsItem>(this, EnteryClass);
				if (ChildPtr)
				{
					// 只给数据 
					ChildPtr->CharacterPtr = CharacterPtr;
					ChildPtr->BasicProxyPtr = Iter;
					ChildPtr->OnClickedDelegate.BindUObject(this, &ThisClass::OnItemClicked);
					UIPtr->AddItem(ChildPtr);
				}
			}
		}
	}
}

void UInteractionTransactionLayout::DisEnable()
{
	ILayoutInterfacetion::DisEnable();
}

ELayoutCommon UInteractionTransactionLayout::GetLayoutType() const
{
	return ELayoutCommon::kTransactionLayout;
}

void UInteractionTransactionLayout::OnQuitClicked()
{
	UUIManagerSubSystem::GetInstance()->SwitchLayout(
	                                                 ELayoutCommon::kConversationLayout
	                                                );
}

void UInteractionTransactionLayout::OnBuyClicked()
{
	Cast<APlanetPlayerController>(UGameplayStatics::GetPlayerController(this, 0))->BuyProxys(
		 CurrentProxyPtr->GetProxyType(),
		 CurrentNum,
		 CalculateCost()
		);
}

void UInteractionTransactionLayout::OnAddClicked()
{
	auto UIPtr = Cast<UEditableTextBox>(GetWidgetFromName(FTransactionLayout::Get().EditableTextBox));
	if (UIPtr)
	{
		const auto Num = CurrentProxyPtr->GetNum();
		if (CurrentNum + 1 > Num)
		{
		}
		else
		{
			CurrentNum++;
			UIPtr->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(CurrentNum)));
			NewNum(CurrentNum);
		}
	}
}

void UInteractionTransactionLayout::OnSubClicked()
{
	auto UIPtr = Cast<UEditableTextBox>(GetWidgetFromName(FTransactionLayout::Get().EditableTextBox));
	if (UIPtr)
	{
		const auto Num = CurrentProxyPtr->GetNum();
		if ((CurrentNum - 1) > 0)
		{
			CurrentNum--;
			UIPtr->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(CurrentNum)));
			NewNum(CurrentNum);
		}
	}
}

void UInteractionTransactionLayout::OnMaxClicked()
{
	auto UIPtr = Cast<UEditableTextBox>(GetWidgetFromName(FTransactionLayout::Get().EditableTextBox));
	if (UIPtr)
	{
		CurrentNum = CurrentProxyPtr->GetNum();
		UIPtr->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(CurrentNum)));
		NewNum(CurrentNum);
	}
}

void UInteractionTransactionLayout::OnEditableTextBoxChangedEvent(
	const FText& Text
	)
{
	auto UIPtr = Cast<UEditableTextBox>(GetWidgetFromName(FTransactionLayout::Get().EditableTextBox));
	if (UIPtr)
	{
		if (UKismetStringLibrary::IsNumeric(Text.ToString()))
		{
			const auto Num = CurrentProxyPtr->GetNum();
			const auto TargetNum = UKismetStringLibrary::Conv_StringToInt(Text.ToString());
			if (TargetNum > Num)
			{
				CurrentNum = Num;
				UIPtr->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(Num)));
				NewNum(CurrentNum);
			}
			if (TargetNum <= 0)
			{
				NewNum(CurrentNum);
			}
			else
			{
				NewNum(TargetNum);
			}
		}
		else
		{
			UIPtr->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(CurrentNum)));
			NewNum(CurrentNum);
		}
	}
}

void UInteractionTransactionLayout::OnItemClicked(
	UGoodsItem* ItemPtr
	)
{
	if (ItemPtr && ItemPtr->BasicProxyPtr)
	{
		CurrentProxyPtr = ItemPtr->BasicProxyPtr;
		{
			auto UIPtr = Cast<UCanvasPanel>(GetWidgetFromName(FTransactionLayout::Get().ItemDetails));
			if (UIPtr)
			{
				UIPtr->SetVisibility(ESlateVisibility::Visible);
			}
		}
		{
			auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FTransactionLayout::Get().ItemName));
			if (UIPtr)
			{
				UIPtr->SetText(FText::FromString(CurrentProxyPtr->GetProxyName()));
			}
		}
		{
			auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FTransactionLayout::Get().ItemDescription));
			if (UIPtr)
			{
				const auto DecriptionText = CurrentProxyPtr->GetItemProxy_Description()->DecriptionText;
				if (DecriptionText.IsValidIndex(0))
				{
					UIPtr->SetText(FText::FromString(DecriptionText[0]));
				}
			}
		}
		{
			auto UIPtr = Cast<UEditableTextBox>(GetWidgetFromName(FTransactionLayout::Get().EditableTextBox));
			if (UIPtr)
			{
				const auto Num = CurrentProxyPtr->GetNum();
				CurrentNum = Num > 0 ? 1 : 0;
				UIPtr->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(CurrentNum)));
				NewNum(CurrentNum);
			}
		}
	}
}

void UInteractionTransactionLayout::OnCoinChanged(
	int32,
	int32
	)
{
	if (!CharacterPtr)
	{
		return;
	}
	NewNum(CurrentNum);
}

inline void UInteractionTransactionLayout::NewNum(
	int32 Num
	)
{
	CurrentNum = Num;
	const auto Cost = CalculateCost();
	auto CoinSPtr = Cast<APlanetPlayerController>(UGameplayStatics::GetPlayerController(this, 0))->
	                GetInventoryComponent()->FindProxy_Coin(UGameplayTagsLibrary::Proxy_Coin_Regular);
	if (CoinSPtr)
	{
		auto UIPtr = Cast<UButton>(GetWidgetFromName(FTransactionLayout::Get().BuyBtn));
		if (UIPtr)
		{
			UIPtr->SetIsEnabled(Cost <= CoinSPtr->GetNum());
		}
	}

	auto TextPtr = Cast<UTextBlock>(GetWidgetFromName(FTransactionLayout::Get().Value));
	if (TextPtr)
	{
		TextPtr->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(Cost)));
	}
}

int32 UInteractionTransactionLayout::CalculateCost() const
{
	const auto ProductsForSaleMap = CharacterPtr->GetAIComponent()->GetSaleItemsInfo();
	if (ProductsForSaleMap.Contains(CurrentProxyPtr->GetProxyType()))
	{
		return CurrentNum * ProductsForSaleMap[CurrentProxyPtr->GetProxyType()].Value;
	}

	return -1;
}
