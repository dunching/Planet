#include "CoinInfo.h"

#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include <Blueprint/WidgetTree.h>

#include "CharacterBase.h"
#include "ItemProxy_Coin.h"
#include "HumanCharacter_Player.h"
#include "InventoryComponent.h"
#include "ModifyItemProxyStrategy.h"
#include "TemplateHelper.h"
#include "ProxyIcon.h"
#include "ItemProxy_Skills.h"

struct FCoinInfo : public TStructVariable<FCoinInfo>
{
	const FName Number = TEXT("Number");

	const FName ProxyIcon = TEXT("ProxyIcon");

	const FName Texture = TEXT("Texture");
};

void UCoinInfo::NativeConstruct()
{
	Super::NativeConstruct();
}

void UCoinInfo::Enable()
{
	ILayoutItemInterfacetion::Enable();

	auto CharacterPtr = Cast<AHumanCharacter_Player>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!CharacterPtr)
	{
		return;
	}

	{
		auto ProxyIconPtr = Cast<UProxyIcon>(GetWidgetFromName(FCoinInfo::Get().ProxyIcon));
		if (ProxyIconPtr)
		{
			auto CoinProxyAry = CharacterPtr->GetInventoryComponent()->FindProxyType<
				FModifyItemProxyStrategy_Coin>(CoinType);
			if (CoinProxyAry.IsValidIndex(0))
			{
				SetNum(CoinProxyAry[0]->GetNum());
				ProxyIconPtr->ResetToolUIByData(CoinProxyAry[0]);
			}
			else
			{
				GetWorld()->GetTimerManager().SetTimerForNextTick(
				                                                  FTimerDelegate::CreateUObject(
					                                                   this,
					                                                   &ThisClass::RemoveFromParent
					                                                  )
				                                                 );
			}
		}

		auto ModifyItemProxyStrategySPtr = CharacterPtr->GetInventoryComponent()->GetModifyItemProxyStrategy<
			FModifyItemProxyStrategy_Coin>();
		if (ModifyItemProxyStrategySPtr)
		{
			auto Handle =
				ModifyItemProxyStrategySPtr->OnCoinProxyChanged.AddCallback(
				                                                            std::bind(
					                                                             &ThisClass::OnCoinProxyChanged,
					                                                             this,
					                                                             std::placeholders::_1,
					                                                             std::placeholders::_2,
					                                                             std::placeholders::_3
					                                                            )
				                                                           );
			Handle->bIsAutoUnregister = false;
		}
	}
}

void UCoinInfo::SetNum(
	int32 NewNum
	)
{
	auto NumTextPtr = Cast<UTextBlock>(GetWidgetFromName(FCoinInfo::Get().Number));
	if (!NumTextPtr)
	{
		return;
	}
	if (NewNum >= 0)
	{
		const auto NumStr = FString::Printf(TEXT("%d"), NewNum);

		NumTextPtr->SetText(FText::FromString(NumStr));
	}
	else
	{
		// checkNoEntry();
		const auto NumStr = FString::Printf(TEXT("%d"), NewNum);

		NumTextPtr->SetText(FText::FromString(NumStr));
	}
}

void UCoinInfo::OnCoinProxyChanged(
	const TSharedPtr<
		FCoinProxy>& CoinProxySPtr,
	EProxyModifyType ProxyModifyType,
	int32 Num
	)
{
	if (CoinProxySPtr)
	{
		SetNum(CoinProxySPtr->GetNum());
	}
}
