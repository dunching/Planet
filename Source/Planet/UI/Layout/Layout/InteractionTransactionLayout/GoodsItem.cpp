#include "GoodsItem.h"

#include "AIComponent.h"
#include "HumanCharacter_AI.h"
#include "ProxyIcon.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetStringLibrary.h"

struct FGoodsItem : public TStructVariable<FGoodsItem>
{
	FName Value = TEXT("Value");

	FName Num = TEXT("Num");

	FName ProxyIcon = TEXT("ProxyIcon");

	FName Btn = TEXT("Btn");
};

void UGoodsItem::NativeConstruct()
{
	Super::NativeConstruct();

	auto UIPtr = Cast<UButton>(GetWidgetFromName(FGoodsItem::Get().Btn));
	if (UIPtr)
	{
		UIPtr->OnClicked.AddDynamic(this, &ThisClass::OnClicked);
	}
}

void UGoodsItem::NativeOnListItemObjectSet(
	UObject* ListItemObject
	)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	InvokeReset(Cast<ThisClass>(ListItemObject));
}

void UGoodsItem::InvokeReset(
	UUserWidget* BaseWidgetPtr
	)
{
	auto LeftPtr = Cast<ThisClass>(BaseWidgetPtr);
	if (LeftPtr)
	{
		CharacterPtr = LeftPtr->CharacterPtr;
		BasicProxyPtr = LeftPtr->BasicProxyPtr;
		OnClickedDelegate = LeftPtr->OnClickedDelegate;
		ResetToolUIByData(LeftPtr->BasicProxyPtr);
	}
}

void UGoodsItem::ResetToolUIByData(
	const TSharedPtr<FBasicProxy>& InBasicProxyPtr
	)
{
	BasicProxyPtr = InBasicProxyPtr;

	if (BasicProxyPtr)
	{
		SetItemType(BasicProxyPtr);
		SetNum(BasicProxyPtr);
		SetValue(BasicProxyPtr);
	}
}

void UGoodsItem::EnableIcon(
	bool bIsEnable
	)
{
}

void UGoodsItem::SetItemType(
	const TSharedPtr<FBasicProxy>& InBasicProxyPtr
	)
{
	auto ProxyIconPtr = Cast<UProxyIcon>(GetWidgetFromName(FGoodsItem::Get().ProxyIcon));
	if (ProxyIconPtr)
	{
		ProxyIconPtr->ResetToolUIByData(InBasicProxyPtr);
	}
}

inline void UGoodsItem::SetValue(
	const TSharedPtr<FBasicProxy>& InBasicProxyPtr
	)
{
	if (!InBasicProxyPtr)
	{
		return;
	}

	const auto ProductsForSaleMap = CharacterPtr->GetAIComponent()->GetSaleItemsInfo();
	if (ProductsForSaleMap.Contains(InBasicProxyPtr->GetProxyType()))
	{
		auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FGoodsItem::Get().Value));
		if (UIPtr)
		{
			const auto Text = FString::Printf(
			                                  TEXT("价格：%d"),
			                                  ProductsForSaleMap[InBasicProxyPtr->GetProxyType()].Value
			                                 );
			UIPtr->SetText(
			               FText::FromString(Text)
			              );
		}
	}
}

void UGoodsItem::OnClicked()
{
	OnClickedDelegate.ExecuteIfBound(this);
}

inline void UGoodsItem::SetNum(
	const TSharedPtr<FBasicProxy>& InBasicProxyPtr
	)
{
	auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FGoodsItem::Get().Num));
	if (UIPtr)
	{
		const auto Text = FString::Printf(
										  TEXT("剩余数量：%d"),
										  InBasicProxyPtr->GetNum()
										 );
		UIPtr->SetText(FText::FromString(Text));
	}
}
