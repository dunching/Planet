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

	auto CoinSPtr = Cast<APlanetPlayerController>(UGameplayStatics::GetPlayerController(this, 0))->
	                GetInventoryComponent()->FindProxy_Coin(UGameplayTagsLibrary::Proxy_Coin_Regular);
	if (CoinSPtr)
	{
		OnCoinChangedDelegateHandle = CoinSPtr->CallbackContainerHelper.AddOnValueChanged(
			 std::bind(&ThisClass::OnCoinChanged, this, std::placeholders::_1, std::placeholders::_2)
			);
	}

	if (CurrentActionSPtr)
	{
		CharacterPtr = CurrentActionSPtr->CharacterPtr;
		auto InventoryComponentPtr = CharacterPtr->GetInventoryComponent();

		OnSkillProxyChangedDelegateHandle = InventoryComponentPtr->OnSkillProxyChanged.AddCallback(
			 std::bind(
			           &ThisClass::OnTraderSkillProxyChanged,
			           this,
			           std::placeholders::_1,
			           std::placeholders::_2
			          )
			);

		OnWeaponProxyChangedDelegateHandle = InventoryComponentPtr->OnWeaponProxyChanged.AddCallback(
			 std::bind(
			           &ThisClass::OnTraderWeaponProxyChanged,
			           this,
			           std::placeholders::_1,
			           std::placeholders::_2
			          )
			);

		OnConsumableProxyChangedDelegateHandle = InventoryComponentPtr->OnConsumableProxyChanged.AddCallback(
			 std::bind(
			           &ThisClass::OnTraderConsumableProxyChanged,
			           this,
			           std::placeholders::_1,
			           std::placeholders::_2,
			           std::placeholders::_3
			          )
			);

		auto UIPtr = Cast<UTileView>(GetWidgetFromName(FTransactionLayout::Get().TileView));
		if (UIPtr)
		{
			UIPtr->ClearListItems();
			auto EnteryClass = UIPtr->GetEntryWidgetClass();

			const auto SaleItemsInfo = CharacterPtr->GetAIComponent()->GetSaleItemsInfo();
			auto Proxys = InventoryComponentPtr->GetProxys(UGameplayTagsLibrary::Proxy_Weapon);
			Proxys.Append(InventoryComponentPtr->GetProxys(UGameplayTagsLibrary::Proxy_Consumables));
			Proxys.Append(InventoryComponentPtr->GetProxys(UGameplayTagsLibrary::Proxy_Skill_Active));
			Proxys.Append(InventoryComponentPtr->GetProxys(UGameplayTagsLibrary::Proxy_Skill_Passve));
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
	if (OnSkillProxyChangedDelegateHandle)
	{
		OnSkillProxyChangedDelegateHandle->UnBindCallback();
	}

	if (OnWeaponProxyChangedDelegateHandle)
	{
		OnWeaponProxyChangedDelegateHandle->UnBindCallback();
	}

	if (OnConsumableProxyChangedDelegateHandle)
	{
		OnConsumableProxyChangedDelegateHandle->UnBindCallback();
	}

	if (OnCoinChangedDelegateHandle)
	{
		OnCoinChangedDelegateHandle->UnBindCallback();
	}

	ILayoutInterfacetion::DisEnable();
}

ELayoutCommon UInteractionTransactionLayout::GetLayoutType() const
{
	return ELayoutCommon::kTransactionLayout;
}

void UInteractionTransactionLayout::OnQuitClicked()
{
	auto CurrentActionSPtr =
		DynamicCastSharedPtr<HumanProcessor::FHumanInteractionBaseProcessor>(
			UInputProcessorSubSystem::GetInstance()->GetCurrentAction()
		);

	if (CurrentActionSPtr)
	{
		CurrentActionSPtr->StopInteraciton();
	}
	UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<HumanProcessor::FHumanRegularProcessor>();
}

void UInteractionTransactionLayout::OnBuyClicked()
{
	Cast<APlanetPlayerController>(UGameplayStatics::GetPlayerController(this, 0))->BuyProxys(
		 CharacterPtr,
		 CurrentProxyPtr->GetProxyType(),
		 CurrentProxyPtr->GetID(),
		 CurrentNum,
		 UGameplayTagsLibrary::Proxy_Coin_Regular,
		 CalculateCost()
		);

	auto UIPtr = Cast<UEditableTextBox>(GetWidgetFromName(FTransactionLayout::Get().EditableTextBox));
	if (UIPtr)
	{
		CurrentNum = 1;
		UIPtr->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(CurrentNum)));
		NewNum(CurrentNum);
	}
}

void UInteractionTransactionLayout::OnAddClicked()
{
	auto UIPtr = Cast<UEditableTextBox>(GetWidgetFromName(FTransactionLayout::Get().EditableTextBox));
	if (UIPtr)
	{
		const auto Num = GetProxyNum(CurrentProxyPtr);
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
		const auto Num = GetProxyNum(CurrentProxyPtr);
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
		const auto Num = GetProxyNum(CurrentProxyPtr);
		if (Num > 0)
		{
			CurrentNum = Num;
			UIPtr->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(CurrentNum)));
			NewNum(CurrentNum);
		}
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
			const auto Num = GetProxyNum(CurrentProxyPtr);
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
				const auto Num = GetProxyNum(CurrentProxyPtr);
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

void UInteractionTransactionLayout::OnTraderSkillProxyChanged(
	const TSharedPtr<FSkillProxy>& ProxySPtr,
	EProxyModifyType ProxyModifyType
	)
{
	if (ProxySPtr)
	{
		auto UIPtr = Cast<UTileView>(GetWidgetFromName(FTransactionLayout::Get().TileView));
		if (UIPtr)
		{
			auto Items = UIPtr->GetDisplayedEntryWidgets();
			for (auto Iter : Items)
			{
				auto GoodsItemPtr = Cast<UGoodsItem>(Iter);
				if (GoodsItemPtr && GoodsItemPtr->BasicProxyPtr == ProxySPtr)
				{
					switch (ProxyModifyType)
					{
					case EProxyModifyType::kNumChanged:
						break;
					case EProxyModifyType::kRemove:
						{
							// 刷新一下
							GoodsItemPtr->EnableIcon(false);
							GoodsItemPtr->SetNum(0);
						}
						break;
					case EProxyModifyType::kPropertyChange:
						break;
					}
					UIPtr->RequestRefresh();
					return;
				}
			}
		}
	}
}

void UInteractionTransactionLayout::OnTraderWeaponProxyChanged(
	const TSharedPtr<FWeaponProxy>& ProxySPtr,
	EProxyModifyType ProxyModifyType
	)
{
	if (ProxySPtr)
	{
		auto UIPtr = Cast<UTileView>(GetWidgetFromName(FTransactionLayout::Get().TileView));
		if (UIPtr)
		{
			auto Items = UIPtr->GetDisplayedEntryWidgets();
			for (auto Iter : Items)
			{
				auto GoodsItemPtr = Cast<UGoodsItem>(Iter);
				if (GoodsItemPtr && GoodsItemPtr->BasicProxyPtr == ProxySPtr)
				{
					switch (ProxyModifyType)
					{
					case EProxyModifyType::kNumChanged:
						break;
					case EProxyModifyType::kRemove:
						{
							// 刷新一下
							GoodsItemPtr->EnableIcon(false);
							GoodsItemPtr->SetNum(0);
						}
						break;
					case EProxyModifyType::kPropertyChange:
						break;
					}
					UIPtr->RequestRefresh();
					return;
				}
			}
		}
	}
}

void UInteractionTransactionLayout::OnTraderConsumableProxyChanged(
	const TSharedPtr<FConsumableProxy>& ProxySPtr,
	EProxyModifyType ProxyModifyType,
	int32 Num
	)
{
	if (ProxySPtr)
	{
		auto UIPtr = Cast<UTileView>(GetWidgetFromName(FTransactionLayout::Get().TileView));
		if (UIPtr)
		{
			auto Items = UIPtr->GetDisplayedEntryWidgets();
			for (auto Iter : Items)
			{
				auto GoodsItemPtr = Cast<UGoodsItem>(Iter);
				if (GoodsItemPtr && GoodsItemPtr->BasicProxyPtr == ProxySPtr)
				{
					switch (ProxyModifyType)
					{
					case EProxyModifyType::kNumChanged:
						{
							// 刷新一下
							const auto ProxyPtrNum = GetProxyNum(GoodsItemPtr->BasicProxyPtr);
							GoodsItemPtr->SetNum(ProxyPtrNum);
						}
						break;
					case EProxyModifyType::kRemove:
						{
							// 刷新一下
							GoodsItemPtr->EnableIcon(false);
							GoodsItemPtr->SetNum(0);
						}
						break;
					case EProxyModifyType::kPropertyChange:
						break;
					}
					UIPtr->RequestRefresh();
					return;
				}
			}
		}
	}
}

inline void UInteractionTransactionLayout::NewNum(
	int32 Num
	)
{
	CurrentNum = Num;

	auto PCPtr = Cast<APlanetPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	const auto Cost = CalculateCost();

	auto TextPtr = Cast<UTextBlock>(GetWidgetFromName(FTransactionLayout::Get().Value));
	if (TextPtr)
	{
		TextPtr->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(Cost)));
	}

	auto CoinSPtr = PCPtr->
	                GetInventoryComponent()->FindProxy_Coin(UGameplayTagsLibrary::Proxy_Coin_Regular);

	if (CoinSPtr && CurrentProxyPtr)
	{
		// 确认目标商人身上的存量是否足够
		auto TargetProxyPtr = CharacterPtr->
		                      GetInventoryComponent()->FindProxy(CurrentProxyPtr->GetID());

		auto UIPtr = Cast<UButton>(GetWidgetFromName(FTransactionLayout::Get().BuyBtn));
		if (UIPtr)
		{
			UIPtr->SetIsEnabled(
			                    (Cost <= CoinSPtr->GetNum()) && TargetProxyPtr && (
				                    GetProxyNum(TargetProxyPtr) >= CurrentNum)
			                   );
		}
	}
}

int32 UInteractionTransactionLayout::CalculateCost() const
{
	if (CurrentProxyPtr)
	{
		const auto ProductsForSaleMap = CharacterPtr->GetAIComponent()->GetSaleItemsInfo();
		if (ProductsForSaleMap.Contains(CurrentProxyPtr->GetProxyType()))
		{
			return CurrentNum * ProductsForSaleMap[CurrentProxyPtr->GetProxyType()].Value;
		}
	}

	return -1;
}
